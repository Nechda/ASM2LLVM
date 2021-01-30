#pragma once
#include "Tools/Logger.h"
#include "Tools/CallStack.h"
#include "Types.h"


/**
\brief  Структура, описывающая входные данные командной строки
*/
struct InputParams
{
    char* inputFilename = NULL;
    char* outputFilename = NULL;
    char* logFilename = "log.log";
    ui32 memorySize = 0;
    ui32 entryPoint = 0;
    bool noLogFileFlag = 0;
    bool useGraphMode = 0;
    bool useStepByStepMode = 0;
};


/**
\brief  Функция парса аргументов командной строки
*/
void parseConsoleArguments(int argc, char** argv, InputParams* ptrInParams);