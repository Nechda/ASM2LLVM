#include "ASM2LLVM.h"
#include "Tools/LibStr.h"
#include <unordered_map>

using namespace Assembler;
using std::unordered_map;

bool isBranchCommand(const ui8 cmdOpCode);              //< Defined in ASM2LLVM.cpp
ui32& getOperandConsistAddrInBrchCommand(Command& cmd); //< Defined in ASM2LLVM.cpp

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


void recalcAdrToCmdNumber(vector<Command>& commandList)
{
    typedef ui32 Addr;
    unordered_map<Addr,ui32> table;
    for (auto& cmd : commandList)
        if (isBranchCommand(cmd.bits.opCode))
        {
            const ui32& addr = getOperandConsistAddrInBrchCommand(cmd);
            table[addr] = 0;
        }

    Addr curAdr = 0;
    for (ui32 i = 0; i < commandList.size(); i++)
    {
        if (table.count(curAdr))
            table[curAdr] = i;
        curAdr += commandList[i].sizeCommand;
    }

    for (auto& cmd : commandList)
        if (isBranchCommand(cmd.bits.opCode))
        {
            ui32& addr = getOperandConsistAddrInBrchCommand(cmd);
            addr = table[addr];
        }
}

TranslatorError Translator::parseBinaryStage(const C_string inputFile)
{
    bool isErrorOccur = 0;
    i8* bytes = NULL;
    ui32 nBytes = 0;
    isErrorOccur |= initInStream(inputFile, &bytes, &nBytes);
    isErrorOccur |= Disassembler::generateCommandList(m_commandList, m_bytesFromDataSection, bytes, nBytes) != ASM_OK;

    //читай описание метода generateCommandList()!
    m_textSectionSize = m_commandList.back().extend[0];
    m_commandList.pop_back();

    //теперь займемся пересчетом адресов в номера команд
    recalcAdrToCmdNumber(m_commandList);

    if(bytes)
        free(bytes);
    return isErrorOccur ? TR_ERROR_PARSE_BINARY : TR_OK;
}