#include "CallStack.h"
#include "Logger.h"
#include <stdlib.h>
#include <string.h>


FILE* fptr = NULL;
unsigned level = 0;

void initCallStack()
{

    fptr = fopen("callStack", "w");
    if (!fptr)
        return;
    setvbuf(fptr, NULL, _IONBF, 0);
}

/**
\brief Функция уничтожения логгера
\note  Данная функция просто вызывает fclose()
*/
void callStackDestr()
{
    if (fptr)
        fclose(fptr);
}

void pushInfo(unsigned line, char* func)
{
    level++;
    for (int i = 0; i < level-1; i++)
        fprintf(fptr,"  ");
    fprintf(fptr,"function: %s\n", func);
}

void popInfo(unsigned line, char* func,char* msg)
{
    for (int i = 0; i < level - 1; i++)
        fprintf(fptr, "  ");
    if(!strcmp("", msg))
        fprintf(fptr, "return OK\n", line);
    else
        fprintf(fptr, "return at line: %d   reason:%s \n", line, msg);
    level--;
}