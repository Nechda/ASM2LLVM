#pragma once
#include "Types.h"


/*
    \brief Структура, описывающая минимальную единицу языка
    */
struct Lexema
{
    char command[10];
    ui16 machineCode;
    char* validFirstOperand;
    char* validSecondOperand;
};


/*
\brief Таблица с описанием всех команд, которые поддерживает компилятор
*/
extern Lexema Table[];


/*
\brief Таблица с описанием всех регистов, которые поддерживает компилятор
*/
extern const Lexema Registers[];


/*
\brief Описание констант, задающих размер таблиц
@{
*/
extern const ui32 COMMAND_TABLE_SIZE;
extern const ui32 REGISTER_TABLE_SIZE;
/*
@}
*/