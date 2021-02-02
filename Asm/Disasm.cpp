#include "Asm.h"
#include "AsmTables.h"
#include "Tools/LibStr.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <vector>

using namespace std;
using namespace Assembler;

/*
\brief  Функция возвращает имя команды, основываясь на машинном коде
\param  [in]  cmd  Структура команды
\return Строка, содержащее имя команды, взятое из таблицы с лексемами
\note   В случае ошибки будет возвращаться NULL
*/
static C_string getCommandName(Command cmd)
{
    for (int i = 0; i < COMMAND_TABLE_SIZE; i++)
        if (commandTable[i].machineCode >> 8 == cmd.code.bits.opCode)
            return (C_string)commandTable[i].command;
    return NULL;
}

/*
\brief  Функция возвращает имя регистра, основываясь на машинном коде
\param  [in]  machineCode  Машинный код
\return Строка, содержащее имя регистра, взятое из таблицы с лексемами
\note   В случае ошибки будет возвращаться NULL
*/
static C_string getRegisterName(ui8 machineCode)
{
    for (int i = 0; i < REGISTER_TABLE_SIZE; i++)
        if (registerTable[i].machineCode == machineCode)
            return (C_string)registerTable[i].command;
    return NULL;
}

/*
\brief  Функция переводит число шеснадцатиричное представление, возвращая строку с преобразованием
\param  [in]  num   Число
\return Строка с шеснадцатиричным числом
*/
static C_string getStrByNumber(ui32 num)
{
    static char buff[32] = {};
    sprintf(buff, "0x%X", num);
    return buff;
}


AsmError Disassembler::getCode(ui8* bytes, ui32 nBytes, FILE* outStream = stdout)
{
    Assert_c(bytes);
    Assert_c(outStream);
    if (!bytes || !outStream)
        return ASM_ERROR_INVALID_INPUT_DATA;
    if (ferror(outStream))
        return ASM_ERROR_CANT_WRITE_INTO_FILE;

    ui8* endPtr = bytes + nBytes;
    ui8* strPtr = bytes;
    Command cmd;
    C_string commandName = NULL;
    C_string operandStr[2] = { NULL, NULL };

    fprintf(outStream, "Offset: Lexema:\n");
    while (bytes < endPtr)
    {
        cmd.code.marchCode = *((Mcode*)bytes);
        fprintf(outStream, "0x%04X: ", bytes - strPtr);
        bytes += sizeof(Mcode);
        commandName = getCommandName(cmd);
        if (!commandName)
        {
            logger("Disassembler error", "The 0x%X code doesn't match any commands", cmd.code.marchCode & 0xFFFF);
            return ASM_ERROR_INVALID_MACHINE_CODE;
        }

        if (cmd.code.bits.nOperands > 2)
        {
            logger("Disassembler error", "Invalid number of operands: %d, should be less than 3", cmd.code.bits.nOperands);
            return ASM_ERROR_INVALID_OPERANDS_NUMBER;
        }

        bool isInvalidOperands = 0;
        for (int i = 0; i < cmd.code.bits.nOperands; i++)
        {
            OperandType opType = getOperandType(cmd, i);
            if (opType == OPERAND_NUMBER || opType == OPERAND_MEMORY)
            {
                cmd.operand[i].ivalue = *((ui32*)bytes);
                bytes += sizeof(ui32);
                operandStr[i] = getStrByNumber(cmd.operand[i].ivalue);

            }
            if (opType == OPERAND_REGISTER || opType == OPERAND_MEM_BY_REG)
            {
                cmd.operand[i].ivalue = *((ui8*)bytes);
                bytes += sizeof(ui8);
                operandStr[i] = getRegisterName(cmd.operand[i].ivalue);
            }
            isInvalidOperands |= !operandStr[i];
        }

        if (isInvalidOperands)
        {
            logger("Disassembler error", "Invalid code for operands!");
            return ASM_ERROR_INVALID_OPERAND_SYNTAX;
        }


        fprintf(outStream, "%s ", commandName);
        for (int i = 0; i < cmd.code.bits.nOperands; i++)
        {
            OperandType opType = getOperandType(cmd, i);
            if (opType == OPERAND_REGISTER || opType == OPERAND_NUMBER)
                fprintf(outStream, "%s", operandStr[i]);
            if (opType == OPERAND_MEMORY || opType == OPERAND_MEM_BY_REG)
                fprintf(outStream, "[%s]", operandStr[i]);
            if (cmd.code.bits.nOperands == 2 && i == 0)
                fprintf(outStream, ", ");
        }
        fprintf(outStream, "\n");
    }
    return ASM_OK;
}


/*
\brief  Производящая дизасемблирование бинарника
\param  [in]  code       Строка байтов, которую будем дизасемблировать
\param  [in]  size       Количество байтов в массиве
\param  [in]  outStream  Поток вывода (может быть файлом)
\return Код ошибки. В случае успеха возвращается ASM_OK.
*/
AsmError Disassembler::disasm(const ui8* code, int size, FILE* outStream)
{
    Assert_c(size > 0);
    if (size <= 0)
        return ASM_ERROR_INVALID_INPUT_DATA;
    return getCode((ui8*)code, size, outStream);
}

/*
\brief  Функция дизасемблирования команды, результат закидывается в outStream
\param  [in]  cmd        Команда, которую надо дизасемблировать
\param  [in]  outStream  Поток вывода, куда будем записывать результат
*/
void Disassembler::disasmCommand(Command cmd, FILE* outStream = stdout)
{
    C_string commandName = NULL;
    C_string operandStr[2] = { NULL, NULL };
    Assert_c(outStream);
    if (!outStream)
        return;
    if (ferror(outStream))
    {
        logger("Disassembler error", "In function %s. Stream \'outStream\' has errors.\n", __FUNCSIG__);
        return;
    }


    commandName = getCommandName(cmd);
    if (!commandName)
    {
        logger("Disassembler error", "The 0x%X code doesn't match any commands", cmd.code.bits.opCode & 0xFFFF);
        return;
    }

    bool isInvalidOperands = 0;
    for (int i = 0; i < cmd.code.bits.nOperands; i++)
    {
        OperandType opType = getOperandType(cmd, i);
        if (opType == OPERAND_NUMBER || opType == OPERAND_MEMORY)
            operandStr[i] = getStrByNumber(cmd.operand[i].ivalue);
        if (opType == OPERAND_REGISTER || opType == OPERAND_MEM_BY_REG)
            operandStr[i] = getRegisterName(cmd.operand[i].ivalue);
        isInvalidOperands |= !operandStr[i];
    }

    if (isInvalidOperands)
    {
        logger("Disassembler error", "Invalid code for operands!");
        return;
    }

    fprintf(outStream, "%s ", commandName);
    for (int i = 0; i < cmd.code.bits.nOperands; i++)
    {
        OperandType opType = getOperandType(cmd, i);
        if (opType == OPERAND_REGISTER || opType == OPERAND_NUMBER)
            fprintf(outStream, "%s", operandStr[i]);
        if (opType == OPERAND_MEMORY || opType == OPERAND_MEM_BY_REG)
            fprintf(outStream, "[%s]", operandStr[i]);
        if (cmd.code.bits.nOperands == 2 && i == 0)
            fprintf(outStream, ", ");
    }
    fprintf(outStream, "\n");
}


AsmError Disassembler::generateCommandList(vector<Command>& commands, i8* bytes, i32 nBytes)
{
    Assert_c(bytes);
    if (!bytes)
        return ASM_ERROR_INVALID_INPUT_DATA;

    i8* endPtr = bytes + nBytes;
    i8* strPtr = bytes;
    Command cmd;
    C_string commandName = NULL;
    C_string operandStr[2] = { NULL, NULL };

    while (bytes < endPtr)
    {
        cmd = {};
        cmd.code.marchCode = *((Mcode*)bytes);
        bytes += sizeof(Mcode);
        cmd.sizeCommand += sizeof(Mcode);

        if (!getCommandName(cmd))
        {
            logger("Disassembler error", "The 0x%X code doesn't match any commands", cmd.code.bits.opCode & 0xFFFF);
            return ASM_ERROR_INVALID_MACHINE_CODE;
        }

        if (cmd.code.bits.nOperands > 2)
        {
            logger("Disassembler error", "Invalid number of operands: %d, should be less than 3", cmd.code.bits.nOperands);
            return ASM_ERROR_INVALID_OPERANDS_NUMBER;
        }

        bool isInvalidOperands = 0;
        for (int i = 0; i < cmd.code.bits.nOperands; i++)
        {
            OperandType opType = getOperandType(cmd, i);
            if (opType == OPERAND_NUMBER || opType == OPERAND_MEMORY)
            {
                cmd.operand[i].ivalue = *((ui32*)bytes);
                bytes += sizeof(ui32);
                cmd.sizeCommand += sizeof(ui32);
            }
            else
                if (opType == OPERAND_REGISTER || opType == OPERAND_MEM_BY_REG)
                {
                    cmd.operand[i].ivalue = *((ui8*)bytes);
                    bytes += sizeof(ui8);
                    cmd.sizeCommand += sizeof(ui8);
                }
                else
                    isInvalidOperands |= 1;
        }

        if (isInvalidOperands)
        {
            logger("Disassembler error", "Invalid code for operands!");
            return ASM_ERROR_INVALID_OPERAND_SYNTAX;
        }

        commands.push_back(cmd);
    }
    return ASM_OK;
}

