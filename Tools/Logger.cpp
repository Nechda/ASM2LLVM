#include "Logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctime>


Logger& logger = Logger::Instance();

/**
\brief У винды такой функции нет, поэтому напишем свой вариант
*/
#if defined(_WIN32)
    #include <chrono>

typedef struct timeval
{
    time_t tv_sec;
    time_t tv_usec;
} timeval;

int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
    namespace sc = std::chrono;
    sc::system_clock::duration d = sc::system_clock::now().time_since_epoch();
    sc::seconds s = sc::duration_cast<sc::seconds>(d);
    tp->tv_sec = s.count();
    tp->tv_usec = sc::duration_cast<sc::microseconds>(d - s).count();
    return 0;
}
#else
#include <sys/time.h>
#endif


Logger Logger::theInstance;

const Stream Logger::getStream()
{
    return logStream;
}

void Logger::printTime()
{
    if (!isValid) return;
    static char buffer[32] = {};
    int millisec;
    tm* tm_info;
    timeval tv;

    gettimeofday(&tv, NULL);

    millisec = (int)(tv.tv_usec / 1000.0);
    if (millisec >= 1000)
    {
        millisec -= 1000;
        tv.tv_sec++;
    }

    tm_info = localtime(&tv.tv_sec);
    strftime(buffer, 32, "%d.%m.%Y %H:%M:%S", tm_info);
    fprintf(logStream, "[%s.%03d] ", buffer, millisec);
}

void Logger::init(const C_string filename)
{
    if (isValid) return;
    if (filename)
    {
        logFile = std::string(filename);
        logStream = fopen(filename, "w");
        isValid = logStream;
        if (isValid)
            setvbuf(logStream, NULL, _IONBF, 0);
    }
    else
    {
        logFile = "";
        logStream = stdout;
        isValid = 1;
    }
}

void Logger::push(const C_string tag, const C_string format, ...)
{
    if (!isValid) return;
    printTime();

    fprintf(logStream, "[%s]: ", tag);

    va_list argList;
    va_start(argList, format);
    vfprintf(logStream, format, argList);
    va_end(argList);

    fprintf(logStream, "\n");
}

void Logger::assertion(const C_string expr, const C_string file, const C_string function, ui32 line)
{
    if (!isValid) return;
    push("Assert",
         "Expression %s is false.\nIn file: %s\nfunction: %s\nline: %d",
         expr,
         file,
         function,
         line);
}

Logger::~Logger()
{
    if (logStream && logStream != stdout)
        fclose(logStream);
}

