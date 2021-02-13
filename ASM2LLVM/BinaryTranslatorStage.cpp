#include "ASM2LLVM.h"
#include "Tools/LibStr.h"
#include <unordered_map>

using namespace Assembler;
using std::unordered_map;

static i32 initInStream(const C_string inputFileName, i8** buffer, ui32* readBytes)
{
    Assert_c(inputFileName);
    Assert_c(buffer);
    Assert_c(readBytes);
    if (!buffer || !readBytes)
        return 1;

    if (!inputFileName)
    {
        logger.push("Error", "{Reading bin file}: No input file specified!");
        return 1;
    }

    *readBytes = readFullFile(inputFileName, buffer);
    if (*readBytes == ASM_ERROR_CODE)
    {
        logger.push("Error",
                    "{Reading bin file}: "
                    "We have some troubles with read code from file:%s",
                    inputFileName);
        return 1;
    }

    return 0;
}


/*
\brief чуть более простой способ пересчета адреса в номер команды
\note  асимптотика алгоритма O(N), пам¤ти в худшем случае будет 4*N
       N - количество команд в программе
*/
void recalcAdrToCmdNumber(vector<Command>& commandList)
{
    typedef ui32 Addr;
    unordered_map<Addr,ui32> table;
    for (auto& cmd : commandList)
        if (isBranchCommand(cmd.code.bits.opCode))
            table[cmd.operand[0].ivalue] = 0;

    Addr curAdr = 0;
    for (ui32 i = 0; i < commandList.size(); i++)
    {
        if (table.count(curAdr))
            table[curAdr] = i;
        curAdr += commandList[i].sizeCommand;
    }

    for (auto& cmd : commandList)
        if (isBranchCommand(cmd.code.bits.opCode))
            cmd.operand[0].ivalue = table[cmd.operand[0].ivalue];
}

/*
\brief самый простой способ пересчитать адреса в номера команд
\note  асимптотика алгоритма O(N), в свою очередь доп памать будет 4*N байт,
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
        if (isBranchCommand(cmd.code.bits.opCode))
            cmd.operand[0].ivalue = nCommand[cmd.operand[0].ivalue];
}

TranslatorError Translator::parseBinaryStage(const C_string inputFile)
{
    bool isErrorOccur = 0;
    i8* bytes = NULL;
    ui32 nBytes = 0;
    isErrorOccur |= initInStream(inputFile, &bytes, &nBytes);
    isErrorOccur |= m_disasembler.generateCommandList(m_commandList, bytes, nBytes) != ASM_OK;
    //теперь займемся пересчетом адресов в номера команд
    recalcAdrToCmdNumber(m_commandList);
    if(bytes)
        free(bytes);
    return isErrorOccur ? TR_ERROR_PARSE_BINARY : TR_OK;
}