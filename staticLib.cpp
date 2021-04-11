#include "Types.h"
#include "CPU/CPUInfo.h"
#include <stdio.h>
#include <math.h>

#include <iostream>
#include "CPU/CPU.h"

union OperandUnion
{
    ui32 ivalue;
    float fvalue;
};

static void* getOperand(ui16 marchCode, ui8 numOperand, ui32& operandValue, i32 extend = 0);

static inline void setBit(ui32* marchCode, ui8 n, bool value)
{
    *marchCode &= ~(1 << n);
    *marchCode |= (value << n);
}

static inline bool isZero(float value)
{
    return fabs(value) < 0.001;
}

extern "C"
{
    #ifndef STATIC_LIB_BUILD
        /// обявление глобальных переменных для IR
        ui32* regTable = nullptr;
        ui8* memory = nullptr;
    #else
        ui32 regTable[COUNT_REGISTERS];
        /// при компиляции необходимо указать размер памяти
        /// через параметр -D
        ui8 memory[STATIC_MEMORY_SIZE];
        void main_func();
    #endif
    /// объявление функций, которые будут интерпретироваться
    #define INTERPRETATED_FUNCTION(name, code)\
        void run_##name(ui16 marchCode, ui32 op1, ui32 op2, ui32 ex1, ui32 ex2) code
    #include "InterpretedFunctions.inc"
    #undef INTERPRETATED_FUNCTION
}

static void* getOperand(ui16 marchCode, ui8 numOperand, ui32& operandValue, i32 extend)
{
    ui8 type = (marchCode >> (2 + 2 * numOperand)) & 0b11;
    bool isLongMem = (marchCode >> 8) & 0b1;

    if (type == 0b00) // register
        return &regTable[operandValue - 1];
    if (type == 0b01) // number
        return &operandValue;
    if (type == 0b10 && !isLongMem) // mem based by number
        return &memory[operandValue];
    if (type == 0b11 && isLongMem) // mem based by reg
        return &memory[regTable[operandValue - 1] + extend];
}

#ifdef STATIC_LIB_BUILD


static void printRegisters()
{
    printf("CPU{\n");
    printf("%4sRegisters{\n", "");

    #define printRegInfo(regName, offset)\
        printf("%8s" #regName ":0x%08X  (int: %011d) \t(float: %f)\n","", regTable[offset],regTable[offset],regTable[offset])
    printRegInfo(eax, 0); printRegInfo(ebx, 1); printRegInfo(ecx, 2); printRegInfo(edx, 3);
    printRegInfo(esi, 4); printRegInfo(edi, 5); printRegInfo(ebp, 6); printRegInfo(esp, 7);
    #undef printRegInfo

    #define printLongRegInfo(regName, offset)\
        printf("%8s"#regName":","");\
        for(ui8 i = 0; i < 4; i++)\
            printf("%08X\'", regTable[4*offset+9 + i]);\
        printf("\n");
    printLongRegInfo(lr0, 0); printLongRegInfo(lr1, 1); printLongRegInfo(lr2, 2); printLongRegInfo(lr3, 3);
    printLongRegInfo(lr4, 4); printLongRegInfo(lr5, 5); printLongRegInfo(lr6, 6); printLongRegInfo(lr7, 7);
    #undef printLongRegInfo
    printf("%4s}\n", "");
}

static void memoryDump()
{
    FILE* output = fopen("memory.bin", "wb");
    if (!output)
    {
        printf("Can`t open `memory.bin` file for writing.\n");
        return;
    }
    fwrite(memory, sizeof(ui8), STATIC_MEMORY_SIZE, output);
    fclose(output);
    printf("Memory dump has wroten into memory.bin\n");
}

void parseInputParams(int argc, char** argv)
{
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-p"))
            printRegisters();
        if (!strcmp(argv[i], "-d"))
            memoryDump();
    }
}

int main(int argc, char** argv)
{
    memset(regTable, 0x00, sizeof(ui32) * COUNT_REGISTERS);
    main_func();
    parseInputParams(argc, argv);
    return 0;
}
#endif