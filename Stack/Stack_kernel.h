#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Tools/Logger.h"
#include "Types.h"


#define Assert_c(expr) if(!(expr)) loggerAssert(#expr,__FILE__,__FUNCSIG__,__LINE__);

#ifndef NDEBUG
    //#define STK_CANARY_PROTECTION           ///<включаем защиту канарейками
    //#define STK_HASH_PROTECTION             ///<включаем защиту хешами
#endif


/**
\brief Несколько ветвлений в условной компиляции, позволит просто реализовать 2 режима работы стека
*/
#ifdef STK_CANARY_PROTECTION
    #define ON_STK_CANARY_PROTECTION(code) code
#else
    #define ON_STK_CANARY_PROTECTION(code)
#endif

#ifdef STK_HASH_PROTECTION
    #define ON_STK_HASH_PROTECTION(code) code
#else
    #define ON_STK_HASH_PROTECTION(code)
#endif

/**
\brief Для краткости дальнейших описаний вводим короткие имена стандартных типов
@{
*/
ON_STK_CANARY_PROTECTION(typedef ui64 CanaryType);
ON_STK_HASH_PROTECTION(typedef ui64 Hash);
/**}@*/


/**
\brief Структура для хранения отладочной информации
*/
struct dbgCallInfo
{
    const char* file;
    const char* func;
    long        line;
    const char* varName;
};

/**
\brief Константы, регулирующие поведение стека
@{
*/
ON_STK_CANARY_PROTECTION(const CanaryType STK_CANARY_VALUE = 0x99996666AAAA5555);  ///< Magic number, который записывается в переменные-канарейки, для отслеживания атак на структуру слева и справа.
const ui8 STK_CANARY_SIZE = ON_STK_CANARY_PROTECTION(sizeof(CanaryType)) + 0;      ///< Размер канареечной переменной
const ui64 STK_BUFFER_ADDITION = 8;                                                ///< Константа, на которую увеличиывается буфер capacity, после того, как место заканчивается
/**}@*/


/**
\brief Константы ошибок
@{
*/
enum StackError { 
    STK_OK                          =  0,   ///< Ошибок не возникло
    STK_ERROR_NULL_PTR              = -1,   ///< Ошибка возникает, если в функцию передали нулевой указатель
    STK_ERROR_OUT_OF_MEMORY         = -2,   ///< Ошибка возникает, если не удалось выделить память calloc или возникла ошибка при вызове realloc
    STK_ERROR_INVALID_PTR           = -3,   ///< Ошибка возникает, если указатель на считываемые данные или data в структуре является NULL
    STK_ERROR_OUT_OF_RANGE          = -4,   ///< Ошибка возникает, если размер стека больше, чем выделенная память
    STK_ERROR_ATTACK                = -5,   ///< Ошибка возникает, если структура стека или данные были испорчены, посредством изменения канареечных переменных
    STK_ERROR_STK_IS_EMPTY          = -6,   ///< Ошибка возникает, если из пустого стека пытаются вытащить данные
    STK_ERROR_CHANGED_DATA          = -7,   ///< Ошибка возникает, если данные были испочены, проверяется посредством подсчета хеша по массиву данных
    STK_ERROR_CHANGED_STRUCTURE     = -8    ///< Ошибка возникает, если структура стека была испорчена, проверяется посредством подсчета хеша по структуре стека
};
/**}@*/


/**
\brief    Абстрактная структура стека, которая не сореджит информации о типе данных.
\detail   Весь функционал стека реализуется посредством данной структуры, затем макросами
          создаются конкретные стеки с нужным типом данных.
*/
struct _BaseStack
{
    ON_STK_CANARY_PROTECTION(const CanaryType leftSide = STK_CANARY_VALUE);
    ON_STK_HASH_PROTECTION (Hash structHash);
    ON_STK_HASH_PROTECTION (Hash dataHash  );
    ui32 elementSize;
    ui32 size;
    ui32 capacity;
    void* data;
    ON_STK_CANARY_PROTECTION(const CanaryType rightSide = STK_CANARY_VALUE);
};


/**
\brief   Функции для работы с абстрактным стеком.
@{
*/
StackError _stackInit(void* stk, const ui32 capacity, const ui32 elementSize);
StackError _stackValidity(const void* stk, const dbgCallInfo dbgInfo = {});
StackError _stackPush(void* stk, void* value, const dbgCallInfo dbgInfo = {});
StackError _stackPop(void* stk, void* dest, const dbgCallInfo dbgInfo = {});
void _stackDump(const void* stk, const dbgCallInfo dbgInfo, FILE* outStream = NULL);
void _stackDest(void* stk);
/**
}@
\note Подробное описание каждой функции дано в файле Stack.cpp
*/