#include "Types.h"
#include "CPU/CPUInfo.h"
#include <stdio.h>
#include <math.h>

union OperandUnion
{
    ui32 ivalue;
    float fvalue;
};

static void* getOperand(ui16 marchCode, bool numOperand, ui32& operandValue);

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
        #define KB * 1024
        ui32 regTable[COUNT_REGISTERS];
        ui8 memory[512 KB];
        void main_func();
    #endif
    /// объявление функций, которые будут интерпретироваться
    #define INTERPRETATED_FUNCTION(name, code)\
        void run_##name(ui16 marchCode, ui32 op1, ui32 op2) code
    #include "InterpretedFunctions.inc"
    #undef INTERPRETATED_FUNCTION
}

static void* getOperand(ui16 marchCode, bool numOperand, ui32& operandValue)
{
    ui8 type = (marchCode >> (2 + 2 * numOperand)) & 0b11;


    if (type == 0b00) // register
        return &regTable[operandValue - 1];
    if (type == 0b01) // number
        return &operandValue;
    if (type == 0b10) // mem based by number
        return &memory[operandValue];
    if (type == 0b11) // mem based by reg
        return &memory[regTable[operandValue - 1]];
}

#ifdef STATIC_LIB_BUILD
int main()
{
    for (ui8 i = 0; i < COUNT_REGISTERS; i++)
        regTable[i] = 0;
    main_func();
    for (ui8 i = 0; i < COUNT_REGISTERS; i++)
        printf("reg[%d]:0x%X\n", i, regTable[i]);
    return 0;
}
#endif