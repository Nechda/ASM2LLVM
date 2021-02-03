#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "Types.h"

#ifdef __GNUG__
#define __FUNCSIG__ __PRETTY_FUNCTION__
#endif


typedef FILE* Stream;

class Logger
{
    public:
        static Logger& Instance()
        {
            return theInstance;
        }
        void init(const C_string filename);
        void push(const C_string tag, const C_string format, ...);
        void assertion(const C_string expr, const C_string file, const C_string function, ui32 line);
        const Stream getStream();
        ~Logger();
    private:
        bool isValid = 0;
        std::string logFile;
        Stream logStream;
        static Logger theInstance;
        void printTime();
        Logger() {};
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
};

extern Logger& logger;

#define Assert_c(expr) if(!(expr)) Logger::Instance().assertion(#expr,__FILE__,__FUNCSIG__,__LINE__);
