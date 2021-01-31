#include "ASM2LLVM.h"
#include "Tools/LibStr.h"
#include <unordered_map>

using namespace Assembler;
using std::unordered_map;

static void initInStream(const C_string inputFileName, i8** buffer, ui32* readBytes)
{
    Assert_c(inputFileName);
    Assert_c(buffer);
    Assert_c(readBytes);
    if (!buffer || !readBytes)
        return;

    if (!inputFileName)
    {
        printf("Error: No input file specified!\n");
        return;
    }

    *readBytes = readFullFile(inputFileName, buffer);
    if (*readBytes == ASM_ERROR_CODE)
    {
        printf("Error: We have some troubles with read code from file:%s\n", inputFileName);
        return;
    }

    return;
}


/*
\brief „уть более простой способ пересчета адреса в номер команды
\note  јсимптотика алгоритма O(N), пам¤ти в худшем случае будет 4*N
       N - количество команд в программе
*/
void recalcAdrToCmdNumber(vector<Command>& commandList)
{
    typedef ui32 Addr;
    unordered_map<Addr,ui32> table;
    for (auto& cmd : commandList)
        if (isBrachComand(cmd.code.bits.opCode))
            table[cmd.operand[0].ivalue] = 0;

    Addr curAdr = 0;
    for (ui32 i = 0; i < commandList.size(); i++)
    {
        if (table.count(curAdr))
            table[curAdr] = i;
        curAdr += commandList[i].sizeCommand;
    }

    for (auto& cmd : commandList)
        if (isBrachComand(cmd.code.bits.opCode))
            cmd.operand[0].ivalue = table[cmd.operand[0].ivalue];
}

/*
\brief —амый простой способ пересчитать адреса в номера команд
\note  јсимптотика алгоритма O(N), в свою очередь доп пам¤ть будет 4*N байт,
       N - количество команд в программе
*/
void stupidRecalculate(vector<Command>& commandList)
{
    vector<ui32> nCommand;
    ui32 cmdIndex = 0;
    for (auto cmd : commandList)
    {
        for (ui8 i = 0; i < cmd.sizeCommand; i++)
            nCommand.push_back(cmdIndex);
        cmdIndex++;
    }

    for (auto& cmd : commandList)
        if (isBrachComand(cmd.code.bits.opCode))
            cmd.operand[0].ivalue = nCommand[cmd.operand[0].ivalue];
}

void ASM2LLVMBuilder::parseBinaryStage(const C_string inputFile)
{
    i8* bytes = NULL;
    ui32 nBytes = 0;
    initInStream(inputFile, &bytes, &nBytes);
    ASM2LLVMBuilder::Instance().disasembler.generateCommandList(commandList, bytes, nBytes);
    //теперь займемс¤ пересчетом адресов в номера команд
    recalcAdrToCmdNumber(commandList);
    free(bytes);
}