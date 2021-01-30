#include "ASM2LLVM.h"

struct Params
{
    vector<pair<ui32, i32>>& nComands;
    vector<Command>& commandList;
    map<ui32, ui32>& mapCmdToBB;
    ui32& funcIndex;
};

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
            if (cmd.code.bits.opCode == Disassembler::CMD_TYPE_CALL)
                isCall = 1;
            if (isBrachComand(cmd.code.bits.opCode))
                parseFunctions(params, params.mapCmdToBB[cmd.operand[0].ivalue], isCall, currentFunctionIndex);
        }
        if (cmd.code.bits.opCode == Disassembler::CMD_TYPE_RET)
            return;

        
    }
}

void ASM2LLVMBuilder::genBBListStage()
{
    ui32 currentPtr = 0;

    //помечаем все команды, которые будут раздел¤ть базовые блоки
    //(измен¤ем самый старший бит в команде на 1, если команда начинает новый базовый блок)
    ui16 BIT_MASK = 1 << (8 * sizeof(Mcode) - 1);
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
            nComands.push_back(pair<ui32, ui32>(i, 0));
        commandList[i].code.marchCode &= ~BIT_MASK;
    }
    nComands.push_back(pair<ui32, ui32>(commandList.size(),1));

    //займемс¤ генерацией наборов базовых блоков дл¤ каждой функции
    map<ui32, ui32> mapCmdToBB;
    ui32 nFunctions = 0;
    for (ui32 i = 0; i < nComands.size(); i++)
        mapCmdToBB[nComands[i].first] = i;
    parseFunctions({nComands, commandList, mapCmdToBB, nFunctions}, 0, 1, 1);
    printf("Found functions: %d\n", nFunctions);
    funcArray.reserve(nFunctions);
    funcArray.resize(nFunctions);
    for (ui32 i = 0; i < nComands.size(); i++)
    {
        if (nComands[i].second < 0)
        {
            funcArray[-nComands[i].second - 1].second = nComands[i].first;
            nComands[i].second = -nComands[i].second - 1;
        }
        else
        {
            nComands[i].second = nComands[i].second - 1;
        }
    }

    BlockInfo* nextBlockInfoPtr = nullptr;
    for (i32 i = nComands.size() - 2; i >= 0; i--)
    {
        bbArray[nComands[i].first] = { nullptr, nComands[i].first, nComands[i + 1].first - 1, nComands[i].second, nextBlockInfoPtr };
        nextBlockInfoPtr = &bbArray[nComands[i].first];
    }

}
