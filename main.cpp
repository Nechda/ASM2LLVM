#include <stdio.h>
#include <stdlib.h>
#include "Tools/Argparser.h"
#include "Tools/Logger.h"
#include "Tools/CallStack.h"
#include "CPU/CPU.h"
#include "ASM2LLVM/ASM2LLVM.h"

#include <llvm/Support/InitLLVM.h>

int main(int argc, char** argv)
{
    InitLLVM X(argc, argv);
    InputParams inputParams;
    initCallStack();
    $
    parseConsoleArguments(argc, argv, &inputParams);
    inputParams.memorySize = inputParams.memorySize > 128 ?
        inputParams.memorySize :
        128;

    Logger::Instance().init(inputParams.noLogFileFlag ? nullptr : inputParams.logFilename);
    CPU::Instance().init(inputParams);

    Translator& translator = Translator::Instance();
    TranslatorError error = translator.ASM2LLVM(
        inputParams.inputFilename,
        inputParams.outputFilename
    );

    if (inputParams.runJIT && error == TR_OK)
        translator.runJIT();

    system("pause");
    $$ return 0;
}