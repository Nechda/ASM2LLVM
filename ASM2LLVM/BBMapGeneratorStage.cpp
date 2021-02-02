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
\brief   ‘ункци¤ рекурсивно заполн¤ет структуру nComands
\delait  ‘ункци¤ рекурсивно заполн¤ет структуру vector<pair<ui32, i32>> nComands
         ѕервый аргумент пары -- номер команды в базовом блоке
         ¬торой аргумент пары -- номер функции в которой располагаетс¤ базовый блок
\note    ‘ункции нумеруютс¤ с единицы, причем если номер оказалс¤ отрицательным, то
         это значит, что данный блок ¤вл¤етс¤ самым первым в функции (именно этому блоку
         передаетс¤ управление после вызова call)
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
            params.nComands[i].second = isCall && i == pos ? -currentFunctionIndex : currentFunctionIndex; //фиксируем наличие первого блока в функции
            isCall = 0;
            if (cmd.code.bits.opCode == Assembler::CMD_CALL)
                isCall = 1;
            if (isBrachComand(cmd.code.bits.opCode))
                parseFunctions(params, cmd.operand[0].ivalue, isCall, currentFunctionIndex);
        }
        if (cmd.code.bits.opCode == Assembler::CMD_RET)
            return;

        
    }
}

void ASM2LLVMBuilder::genBBListStage()
{
    ui32 currentPtr = 0;

    //помечаем все команды, которые будут раздел¤ть базовые блоки
    //измен¤ем зарезервированный бит в команде
    //(см структуру машинного кода команды в Asm/Ams.h)
    ui16 BIT_MASK = 1 << 6;
    bool wasPrevJmp = 0;
    bool isBrchOp = 0;
    for (auto& cmd : commandList)
    {
        isBrchOp = isBrachComand(cmd.code.bits.opCode);
        
        //помечаем команду, на которую ссылаютс¤
        if (isBrchOp)
            commandList[cmd.operand[0].ivalue].code.marchCode |= BIT_MASK;
        //и следующую команду
        if (wasPrevJmp)
            cmd.code.marchCode |= BIT_MASK;
            
        wasPrevJmp = isBrchOp;
        currentPtr += cmd.sizeCommand;
    }


    //получаем номера команд, которые раздел¤ют базовые блоки
    //(номер первой команды в базовом блоке)
    //номер команды  номер функции (считаем с 1!!!)
    //           |    |
    //           V    V
    vector<pair<ui32,i32>> nComands;
    nComands.push_back(pair<ui32,i32>(0,0));
    for (ui32 i = 0; i < commandList.size(); i++)
    {
        if (commandList[i].code.marchCode & BIT_MASK)
            nComands.push_back(pair<ui32, ui32>(i, 0)); //в начале все базовые блоки принадлежат "нулевой" функции
        commandList[i].code.marchCode &= ~BIT_MASK;
    }
    nComands.push_back(pair<ui32, ui32>(commandList.size(),1));


    //займемс¤ генерацией наборов базовых блоков дл¤ каждой функции
    unordered_map<ui32, ui32> mapCmdToBB;
    ui32 nFunctions = 0;
    for (ui32 i = 0; i < nComands.size(); i++)
        mapCmdToBB[nComands[i].first] = i;

    //пересчитаем операнды branch команд в номера базовых блоков,
    //на которые данные команды ссылаютс¤
    for (auto& cmd : commandList)
        if (isBrachComand(cmd.code.bits.opCode))
            cmd.operand[0].ivalue = mapCmdToBB[cmd.operand[0].ivalue];

    //запускаем поиск поиск функци
    parseFunctions({nComands, commandList, nFunctions}, 0, 1, 1);
    printf("Found functions: %d\n", nFunctions);
    //теперь гереним таблицу функций vector<Function*,ui32> funcArray
    //                                         ^       ^
    //                                         |       |
    //                            llvm ir функци¤   номер базового блока, с которого начинаетс¤ функци¤
    funcArray.reserve(nFunctions);
    funcArray.resize(nFunctions);
    for (ui32 i = 0; i < nComands.size(); i++)
    {
        if (nComands[i].second < 0)
        {
            funcArray[-nComands[i].second - 1].second = mapCmdToBB[nComands[i].first];
            nComands[i].second = -nComands[i].second - 1;
        }
        else
        {
            nComands[i].second = nComands[i].second - 1;
        }
    }

    //генерим массив базовых блоков
    bbArray.reserve(nComands.size());
    bbArray.resize(nComands.size());
    for (ui32 i = 0; i < nComands.size() - 1; i++)
        bbArray[i] = { nullptr, nComands[i].first, nComands[i + 1].first - 1, nComands[i].second };
    bbArray.back() = {}; //завершающий базовый блок, в котором нет ни одной команды

}
