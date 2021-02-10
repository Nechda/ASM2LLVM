#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include "Tools/Argparser.h"
#include "Tools/Logger.h"
#include "Tools/CallStack.h"
#include "CPU/CPU.h"
#include "ASM2LLVM/ASM2LLVM.h"
#include <iostream>
#include <fstream>

#include <llvm/Support/InitLLVM.h>


/**
\brief Константы, описывающие коды ошибок, возникающие в main
*/
const ui8 ERROR_INIT_INPUT_STREAM           = 1 << 0;
const ui8 ERROR_INIT_OUTPUT_STREAM          = 1 << 1;
const ui8 ERROR_JUMP_DEFAULT_CASE_IN_SWITCH = 1 << 2;


/**
\brief Функция инициализирует поток вывода, основываясь на данных из InputParams
*/
int initOutStream(char* outputFileName, FILE** outStreamPtr, char* mode,InputParams inputParams)
{$
    Assert_c(outStreamPtr);
    Assert_c(mode);
    if (!outStreamPtr || !mode)
    {
        $$$("Null ptr in input data");
        return ERROR_INIT_OUTPUT_STREAM;
    }

    if (!outputFileName)
    {
        printf("Warning: No output file specified. By default result will write into a.out\n");
        outputFileName = "a.out";
        inputParams.outputFilename = "a.out";
    }

    *outStreamPtr = fopen(outputFileName, mode);
    if (!*outStreamPtr)
    {
        printf("Error: We can't open file %s for writing result.\n", outputFileName);
        $$$("Can't read file");
        return ERROR_INIT_OUTPUT_STREAM;
    }
    if (ferror(*outStreamPtr))
    {
        printf("Error: Stream for writing result has error.\n");
        $$$("Error in stream");
        return ERROR_INIT_OUTPUT_STREAM;
    }
    $$ return 0;
}


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