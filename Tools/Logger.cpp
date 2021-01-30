#include "Logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctime>

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


static FILE *fptr = NULL; ///< переменная файла - лога


/**
\brief Функция возвращает файловую переменную лога
\return Файловая переменная лога
*/
FILE* getLoggerStream()
{
    return fptr;
}



void logger(const char* tag, const char* format, ...)
{
    if (!fptr)
        return;
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
    fprintf(fptr, "[%s.%03d] [%s]: ", buffer, millisec, tag);


    va_list argList;
    va_start(argList, format);
    vfprintf(fptr, format, argList);
    va_end(argList);

    fprintf(fptr, "\n");
}

/**
\brief Функция, направляющая сообщение Assert_c в лог
\note  Смотри макрос Assert_c(exp)
*/
void loggerAssert(const char* expr, const char* file,const char* funciton, unsigned line)
{
    if (!fptr)
        return;
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
    fprintf(fptr,"[%s.%03d] [%s]: Expression %s is false.\nIn file: %s\nfunction: %s\nline: %d\n",buffer,millisec,"Assert", expr, file, funciton, line);
}

/**
\brief Функция инициализации файла-лога
*/
void loggerInit(const char* filename,const char* mode)
{
    if (!filename)
    {
        fptr = stdout;
        return;
    }
    if (fptr)
        return;
    fptr = fopen(filename, mode);
    if (!fptr)
        return;
    setvbuf(fptr, NULL, _IONBF, 0);
}

/**
\brief Функция уничтожения логгера
\note  Данная функция просто вызывает fclose()
*/
void loggerDestr()
{
    if(fptr)
        fclose(fptr);
}