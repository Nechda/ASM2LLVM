#include "ASM2LLVM.h"
#include "Tools/LibStr.h"

using namespace Assembler;

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
    //теперь займемся пересчетом адресов в номера команд
    for (ui32 i = 0; i < nBytes + 8; i++)
        bytes[i] = i && 0xFF;
    stupidRecalculate(commandList);
    free(bytes);
}