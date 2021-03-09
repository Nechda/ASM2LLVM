#pragma once
#include "Asm/Asm.h"
#include "Tools/Argparser.h"

#include "Stack/Stack_kernel.h"
#define TYPE_ ui8
#include "Stack/Stack.h"
#undef TYPE_


/*
\brief Коды ошибок, возвращаемые процессором
*/
enum CPUerror
{
    CPU_OK = 0,
    CPU_ERROR_INVALID_STRUCUTE,
    CPU_ERROR_INVALID_COMMAND,
    CPU_ERROR_EXCEPTION,
    CPU_ERROR_EPI_OUT_OF_RANE,
    CPU_INVALID_INPUT_DATA
};

#define DUMP_PRINT_MEMORY

class CPU
{
public:
    static CPU& Instance()
    {
        static CPU theInstance;
        return theInstance;
    }
    void dump(Stream outStream);
    void status();
    void init(const InputParams inParam);
    CPUerror run(ui8* bytes, ui32 size, ui32 ptrStart);
    ~CPU();
private:
    CPU() {};
    CPU(const CPU&) = delete;
    CPU& operator=(const CPU) = delete;
    CPUerror evaluate();
public:
    static struct CPUStruct
    {
        bool isValid = 0;
        int  interruptCode = 0;
        bool stepByStep = 0;
        bool isGraphMode = 0;
        bool isVideoMemoryChanged = 0;
        struct
        {
            ui32 x = 0;
            ui32 y = 0;
        }ChangedPixel;
        ui32 ramSize = 8;
        struct
        {
            ui32 eax; ui32 ebx; ui32 ecx; ui32 edx;
            ui32 esi; ui32 edi; ui32 esp; ui32 ebp;
            ui32 eip; ui32 efl; ui32 ecs; ui32 eds;
            ui32 ess;
        }Register;
        ui8* RAM = NULL;
        Stack(ui8) stack;
    }myCPU;
    typedef void(*PtrToFunction)(Assembler::Command*);
    static PtrToFunction runFunction[];
    static const ui32 FUNCTION_TABLE_SIZE;
};


C_string getStringByErrorCode(CPUerror errorCode);

