#pragma once
#include "Logger.h"

#define $ pushInfo(__LINE__, __FUNCSIG__);
#define $$ popInfo(__LINE__, __FUNCSIG__,"");
#define $$$(msg) popInfo(__LINE__, __FUNCSIG__, msg);

void initCallStack();
void callStackDestr();

void pushInfo(unsigned line, char* func);
void popInfo(unsigned line, char* func,char* msg);