#include "Argparser.h"
#include <string.h>

/**
\brief  Функция парса аргументов командной строки
*/
void parseConsoleArguments(int argc, char** argv, InputParams* ptrInParams)
{
    $
    char** curStr = NULL;
    char* buf = NULL;
    char option = 0;
    int intBuf = 0;
    for (int i = 1; i < argc; i++)
    {
        option = argv[i][0] == '-' ? argv[i][1] : 0;
        switch (option)
        {
        case 'l':
            curStr = &ptrInParams->logFilename;
            break;
        case 'i':
            curStr = &ptrInParams->inputFilename;
            break;
        case 'o':
            curStr = &ptrInParams->outputFilename;
            break;
        case 'n':
            ptrInParams->noLogFileFlag = 1;
            break;
        case 'r':
            ptrInParams->runJIT = 1;
            break;
        case '-':
            argv[i] += 2;
            buf = strtok(argv[i], "=");
            if (!strcmp(buf, "memorySize"))
            {
                buf = strtok(NULL, "=");
                sscanf(buf, "%d", &intBuf);
                ptrInParams->memorySize = intBuf;
            }
            if (!strcmp(buf, "makeItBetter"))
                ptrInParams->wantOptimization = 1;

            argv[i] -= 2;
            break;
        default:
            *curStr = argv[i];
            break;
        }
    }
    $$
}