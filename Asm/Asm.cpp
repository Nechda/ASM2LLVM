#include "Asm.h"
#include "AsmTables.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <iostream>

using namespace Assembler;

Lexema commandTable[] = {
    #define DEF(name, mCode, vStr1, vStr2, code) \
        {#name, mCode, vStr1, vStr2},
    #include "Extend.h"
    #undef DEF
};

const Lexema registerTable[] =
{
    {"eax",     1},{"ebx",     2},{"ecx",     3},{"edx",     4},
    {"esi",     5},{"edi",     6},{"esp",     7},{"ebp",     8},
    {"eip",     9},{"eflags", 10},{"ecs",    11},{"eds",    12},
    {"ess",    13}
};

const ui32 COMMAND_TABLE_SIZE = sizeof(commandTable) / sizeof(Lexema);
const ui32 REGISTER_TABLE_SIZE = sizeof(registerTable) / sizeof(Lexema);


/*
\brief  Функция присваивает машинной команде тип операнда
\param  [in]  ptrMarchCode  Указатель на машинную команду
\param  [in]  opIndex       Номер операнда
\param  [in]  type          Тип операнда
*/
void Assembler::setOperandType(Command& cmd, ui8 opIndex, OperandType type)
{
    if (opIndex == 1)
        cmd.code.bits.typeFirst = (ui8)type & 0b11;
    else
        cmd.code.bits.typeSecond = (ui8)type & 0b11;
}


/*
\brief  Функция определяет тип операнда по машинной команде
\param  [in]  marchCode  Машинная команда
\param  [in]  opIndex    Номер операнда
\return Тип операнда, имеющий индекс opIndex
*/
OperandType Assembler::getOperandType(Command cmd, ui8 opIndex)
{
    if (opIndex == 1)
        return (OperandType)cmd.code.bits.typeFirst;
    else
        return (OperandType)cmd.code.bits.typeSecond;
}


/*
\brief  Функция возвращает поясняющую строку, по коду ошибки компилятора\дизасемблера
\param  [in]  errorCode  Код ошибки
\return Возвращается строка, поясняющее код ошибки
*/
C_string Assembler::getStringByErrorCode(AsmError errorCode)
{
    switch (errorCode)
    {
        case ASM_OK:
            return "Ok";
            break;
        case ASM_ERROR_INVALID_INPUT_DATA:
            return "Due to compilating occur error ralated to invalid input data";
            break;
        case ASM_ERROR_OUT_OF_MEMORY:
            return "Due to compilating occur error realted to calloc or realloc function";
            break;
        case ASM_ERROR_GEN_LABLE_TABLE:
            return "The compiler could not generate the label table";
            break;
        case ASM_ERROR_GEN_MACHINE_CODE:
            return "We could not generate the machine code";
            break;
        case ASM_ERROR_CANT_WRITE_INTO_FILE:
            return "There is error with access to file";
            break;
        case ASM_ERROR_INVALID_SYNTAX:
            return "Syntax error";
            break;
        case ASM_ERROR_INVALID_MACHINE_CODE:
            return "Generate invalid machine code";
            break;
        case ASM_ERROR_INVALID_OPERANDS_NUMBER:
            return "Invalid number of operands";
            break;
        case ASM_ERROR_INVALID_OPERAND_SYNTAX:
            return "Invalid operand syntax";
            break;
        case ASM_ERROR_INVALID_OPERAND_TYPE_FOR_COMMAND:
            return "Invalid type of operands";
            break;
        case ASM_ERROR_CANT_READ_LEXEMA:
            return "Error occur, when we try to read lexema from code.\n"
                   "Perhabs this error related to null pointer.";
            break;
        default:
            return "Undefined error code";
            break;
    }
}