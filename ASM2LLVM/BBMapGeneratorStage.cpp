#include "ASM2LLVM.h"
#include <unordered_map>

using std::unordered_map;

bool isBranchCommand(const ui8 cmdOpCode);              //< Defined in ASM2LLVM.cpp
ui32& getOperandConsistAddrInBrchCommand(Command& cmd); //< Defined in ASM2LLVM.cpp

struct Params
{
    vector<pair<ui32, i32>>& nComands;
    vector<Command>& commandList;
    ui32& funcIndex;
};

/*
\brief   Функция рекурсивно заполняет структуру funcDescriptors
\delait  Функция рекурсивно заполняет структуру vector<pair<ui32, i32>> funcDescriptors
         Первый аргумент пары -- номер команды в базовом блоке
         Второй аргумент пары -- номер функции в которой располагается базовый блок
\note    Функции нумеруются с единицы, причем если номер оказался отрицательным, то
         это значит, что данный блок является самым первым в функции (именно этому блоку
         передается управление после вызова call)
*/
void parseFunctions(Params params, ui32 pos, bool isCall, ui32 currentFunctionIndex)
{
    if (!params.nComands[pos].second && isCall)
        currentFunctionIndex = ++params.funcIndex;


    #define LAST_BB_CMD(num) params.commandList[params.nComands[num+1].first-1]
    Command cmd;
    for (int i = pos; i < params.nComands.size()-1; i++)
    {
        cmd = LAST_BB_CMD(i);
        if (!params.nComands[i].second)
        {
            params.nComands[i].second = isCall && i == pos ?
                -currentFunctionIndex : //фиксируем наличие первого блока в функции
                currentFunctionIndex; 
            isCall = 0;
            if (cmd.bits.opCode == Assembler::CMD_CALL)
                isCall = 1;
            if (isBranchCommand(cmd.bits.opCode))
            {
                const ui32& addr = getOperandConsistAddrInBrchCommand(cmd);
                parseFunctions(params, addr, isCall, currentFunctionIndex);
            }
        }
        if (cmd.bits.opCode == Assembler::CMD_RET)
            return;
    }
}

TranslatorError Translator::genBBStructureStage()
{
    ui32 currentPtr = 0;

    //помечаем все команды, которые будут разделять базовые блоки
    //изменяем зарезервированный бит в команде
    //(см структуру машинного кода команды в Asm/Ams.h)
    bool wasPrevJmp = 0;
    bool isBrchOp = 0;
    for (auto& cmd : m_commandList)
    {
        isBrchOp = isBranchCommand(cmd.bits.opCode);
        
        //помечаем команду, на которую ссылаются
        if (isBrchOp)
        {
            const ui32& addr = getOperandConsistAddrInBrchCommand(cmd);
            m_commandList[addr].bits.reserved = 1;
        }
        //и следующую команду
        if (wasPrevJmp)
            cmd.bits.reserved = 1;
            
        wasPrevJmp = isBrchOp;
        currentPtr += cmd.sizeCommand;
    }


    using NCommand_t = ui32;
    //получаем номера команд, которые разделяют базовые блоки
    //(номер первой команды в базовом блоке)
    //      номер команды  номер функции (считаем с 1!!!)
    //                 |    |
    //                 V    V
    vector<pair<NCommand_t,i32>> funcDescriptors;
    funcDescriptors.push_back({0,0});
    for (ui32 i = 0; i < m_commandList.size(); i++)
    {
        if (m_commandList[i].bits.reserved)
            funcDescriptors.push_back({ i, 0 }); //в начале все базовые блоки принадлежат "нулевой" функции

        m_commandList[i].bits.reserved = 0;
    }
    funcDescriptors.push_back(pair<ui32, ui32>(m_commandList.size(),1));


    using NBasicBlock_t = ui32;
    //займемся генерацией наборов базовых блоков для каждой функции
    unordered_map<NCommand_t, NBasicBlock_t> mapCmdToBB;
    ui32 nFunctions = 0;
    for (ui32 i = 0; i < funcDescriptors.size(); i++)
        mapCmdToBB[funcDescriptors[i].first] = i;

    //пересчитаем операнды branch команд в номера базовых блоков,
    //на которые данные команды ссылаются
    for (auto& cmd : m_commandList)
        if (isBranchCommand(cmd.bits.opCode))
        {
            ui32& addr = getOperandConsistAddrInBrchCommand(cmd);
            addr = mapCmdToBB[addr];
        }

    //запускаем поиск поиск функци
    parseFunctions({funcDescriptors, m_commandList, nFunctions}, 0, 1, 1);
    //printf("Found functions: %d\n", nFunctions);
    //теперь гереним таблицу функций vector<Function*,ui32> m_funcArray
    //                                         ^       ^
    //                                         |       |
    //                            llvm ir функция   номер базового блока, с которого начинается функция
    m_funcArray.reserve(nFunctions);
    m_funcArray.resize(nFunctions);
    for (ui32 i = 0; i < funcDescriptors.size(); i++)
    {
        if (funcDescriptors[i].second < 0)
        {
            m_funcArray[-funcDescriptors[i].second - 1].second = mapCmdToBB[funcDescriptors[i].first];
            funcDescriptors[i].second = -funcDescriptors[i].second - 1;
        }
        else
        {
            funcDescriptors[i].second = funcDescriptors[i].second - 1;
        }
    }

    //генерим массив базовых блоков
    m_bbArray.reserve(funcDescriptors.size());
    m_bbArray.resize(funcDescriptors.size());
    for (ui32 i = 0; i < funcDescriptors.size() - 1; i++)
        m_bbArray[i] = { nullptr, funcDescriptors[i].first, funcDescriptors[i + 1].first - 1, funcDescriptors[i].second };
    m_bbArray.back() = {}; //завершающий базовый блок, в котором нет ни одной команды

    return TR_OK;
}
