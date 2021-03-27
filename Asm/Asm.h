#pragma once
#include <stdio.h>
#include "Tools/Logger.h"
#include "Types.h"
#include <vector>


typedef ui16 Mcode;
const int ASM_ERROR_CODE = -1;

using std::vector;


namespace Assembler
{
    /*
    \brief Коды ошибок, возвращаемые компилятором и дизасемблером
    */
    enum AsmError
    {
        ASM_OK = 0,
        ASM_ERROR_INVALID_INPUT_DATA,
        ASM_ERROR_OUT_OF_MEMORY,
        ASM_ERROR_GEN_LABLE_TABLE,
        ASM_ERROR_GEN_MACHINE_CODE,
        ASM_ERROR_CANT_WRITE_INTO_FILE,
        ASM_ERROR_INVALID_SYNTAX,
        ASM_ERROR_INVALID_MACHINE_CODE,
        ASM_ERROR_INVALID_OPERANDS_NUMBER,
        ASM_ERROR_INVALID_OPERAND_SYNTAX,
        ASM_ERROR_INVALID_OPERAND_TYPE_FOR_COMMAND,
        ASM_ERROR_CANT_READ_LEXEMA
    };
    C_string getStringByErrorCode(AsmError errorCode); 


    union OperandUnion
    {
        ui32 ivalue;
        float fvalue;
    };

    /*
    \brief Посредствам данной стуктуры реализуется "команда" процессора
    */
    struct Command
    {
        union
        {
            ui16 marchCode;
            struct
            {
                ui8 nOperands  : 2;
                ui8 typeFirst  : 2;
                ui8 typeSecond : 2;
                ui8 typeThird  : 2;
                ui8 longCommand: 1;
                ui8 reserved   : 1;
                ui8 opCode     : 6;
            };
        }bits;
        OperandUnion operand[3] = { 0, 0, 0 };
        ui32 extend[3] = { 0, 0, 0 };
        ui8 sizeCommand = 0;
    };

    enum CommandType
    {
        #define DEF(name, mCode, vStr1, vSrt2, code)\
            CMD_##name,
            #include "Extend.h"
        #undef DEF
    };


    /*
    \brief Допустимые типы операндов
    */
    enum OperandType
    {
        OPERAND_REGISTER,
        OPERAND_NUMBER,
        OPERAND_MEMORY,
        OPERAND_MEM_BY_REG
    };
    void setOperandType(Command& marchCode, ui8 opIndex, OperandType type);
    OperandType getOperandType(Command marchCode, ui8 opIndex);


    class Disassembler
    {
        public:
            static Disassembler& Instance()
            {
                static Disassembler theInstance;
                return theInstance;
            }
            AsmError generateCommandList(vector<Command>& commands, vector<ui8>& bytesFromDataSection, i8* bytes, i32 nBytes);
            AsmError disasm(const ui8* code, int size, FILE* stream);
            void disasmCommand(Command cmd, FILE* stream);
        private:
            Disassembler() {};
            Disassembler(const Disassembler&) = delete;
            Disassembler& operator=(const Disassembler&) = delete;
            AsmError getCode(ui8* bytes, ui32 nBytes, FILE* outStream);
    };

    class Compilator
    {
        public:
            static Compilator& Instance()
            {
                static Compilator theInstance;
                return theInstance;
            }
            AsmError compile(const char* code, FILE* outStream);
        private:
            Compilator();
            Compilator(const Compilator&) = delete;
            Compilator& operator=(const Compilator&) = delete;
    };

}
