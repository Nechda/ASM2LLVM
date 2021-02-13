#include "ASM2LLVM.h"
#include <unordered_map>

using std::unordered_map;

struct Params
{
    vector<pair<ui32, i32>>& nComands;
    vector<Command>& commandList;
    ui32& funcIndex;
};

/*
\brief   функция рекурсивно заполняет структуру nComands
\delait  функция рекурсивно заполняет структуру vector<pair<ui32, i32>> nComands
         первый аргумент пары -- номер команды в базовом блоке
         второй аргумент пары -- номер функции в которой располагается базовый блок
\note    функции нумеруются с единицы, причем если номер оказался отрицательным, то
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
            if (cmd.code.bits.opCode == Assembler::CMD_CALL)
                isCall = 1;
            if (isBranchCommand(cmd.code.bits.opCode))
                parseFunctions(params, cmd.operand[0].ivalue, isCall, currentFunctionIndex);
        }
        if (cmd.code.bits.opCode == Assembler::CMD_RET)
            return;
    }
}

TranslatorError Translator::genBBStructureStage()
{
    ui32 currentPtr = 0;

    //помечаем все команды, которые будут разделять базовые блоки
    //изменяем зарезервированный бит в команде
    //(см структуру машинного кода команды в Asm/Ams.h)
    ui16 BIT_MASK = 1 << 6;
    bool wasPrevJmp = 0;
    bool isBrchOp = 0;
    for (auto& cmd : m_commandList)
    {
        isBrchOp = isBranchCommand(cmd.code.bits.opCode);
        
        //помечаем команду, на которую ссылаются
        if (isBrchOp)
            m_commandList[cmd.operand[0].ivalue].code.marchCode |= BIT_MASK;
        //и следующую команду
        if (wasPrevJmp)
            cmd.code.marchCode |= BIT_MASK;
            
        wasPrevJmp = isBrchOp;
        currentPtr += cmd.sizeCommand;
    }


    //получаем номера команд, которые разделяют базовые блоки
    //(номер первой команды в базовом блоке)
    //номер команды  номер функции (считаем с 1!!!)
    //           |    |
    //           V    V
    vector<pair<ui32,i32>> nComands;
    nComands.push_back(pair<ui32,i32>(0,0));
    for (ui32 i = 0; i < m_commandList.size(); i++)
    {
        if (m_commandList[i].code.marchCode & BIT_MASK)
            nComands.push_back(pair<ui32, ui32>(i, 0)); //в начале все базовые блоки принадлежат "нулевой" функции
        m_commandList[i].code.marchCode &= ~BIT_MASK;
    }
    nComands.push_back(pair<ui32, ui32>(m_commandList.size(),1));


    //займемся генерацией наборов базовых блоков для каждой функции
    unordered_map<ui32, ui32> mapCmdToBB;
    ui32 nFunctions = 0;
    for (ui32 i = 0; i < nComands.size(); i++)
        mapCmdToBB[nComands[i].first] = i;

    //пересчитаем операнды branch команд в номера базовых блоков,
    //на которые данные команды ссылаются
    for (auto& cmd : m_commandList)
        if (isBranchCommand(cmd.code.bits.opCode))
            cmd.operand[0].ivalue = mapCmdToBB[cmd.operand[0].ivalue];

    //запускаем поиск поиск функци
    parseFunctions({nComands, m_commandList, nFunctions}, 0, 1, 1);
    //printf("Found functions: %d\n", nFunctions);
    //теперь гереним таблицу функций vector<Function*,ui32> m_funcArray
    //                                         ^       ^
    //                                         |       |
    //                            llvm ir функция   номер базового блока, с которого начинается функция
    m_funcArray.reserve(nFunctions);
    m_funcArray.resize(nFunctions);
    for (ui32 i = 0; i < nComands.size(); i++)
    {
        if (nComands[i].second < 0)
        {
            m_funcArray[-nComands[i].second - 1].second = mapCmdToBB[nComands[i].first];
            nComands[i].second = -nComands[i].second - 1;
        }
        else
        {
            nComands[i].second = nComands[i].second - 1;
        }
    }

    //генерим массив базовых блоков
    m_bbArray.reserve(nComands.size());
    m_bbArray.resize(nComands.size());
    for (ui32 i = 0; i < nComands.size() - 1; i++)
        m_bbArray[i] = { nullptr, nComands[i].first, nComands[i + 1].first - 1, nComands[i].second };
    m_bbArray.back() = {}; //завершающий базовый блок, в котором нет ни одной команды

    return TR_OK;
}
