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
\brief Таблицы с описанием всех команд и регистров, с которыми может работать компилятор
*/
extern Lexema commandTable[];
extern const Lexema registerTable[];

/*
\brief Размеры таблиц
*/
extern const ui32 COMMAND_TABLE_SIZE;
extern const ui32 REGISTER_TABLE_SIZE;