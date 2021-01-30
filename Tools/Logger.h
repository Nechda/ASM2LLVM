#pragma once
#include <stdio.h>
#include <stdlib.h>

#ifdef __GNUG__
#define __FUNCSIG__ __PRETTY_FUNCTION__
#endif

#define Assert_c(expr) if(!(expr)) loggerAssert(#expr,__FILE__,__FUNCSIG__,__LINE__);


FILE* getLoggerStream();


void logger(const char* tag, const char* format, ...);
void loggerAssert(const char* expr, const char* file,const char* function, unsigned line);
void loggerInit(const char* filename,const char* mode = "a");
void loggerDestr();