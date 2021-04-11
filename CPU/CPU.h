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


//#define CPU_GRAPH_MODE
//#define CPU_PROFILER
//#define DUMP_PRINT_MEMORY
//#define CPU_SMART_PRINT_MEMORY


#ifndef ASM2LLVM_PROJECT
    #include "Profiler/Profiler.h"
    #define ASM2LLVM_CODE_FILTER( code ) code
#else
    #define ASM2LLVM_CODE_FILTER( code )
    #define CPU_PROFILER 0
#endif


class CPU
{
    public:
        static CPU& Instance()
        {
            static CPU theInstance;
            return theInstance;
        }
        void dump(Stream outStream);
        void init(const InputParams inParam);
        CPUerror run(ui8* bytes, ui32 size, ui32 ptrStart);
        ~CPU();
    private:
        CPU() {};
        CPU(const CPU&) = delete;
        CPU& operator=(const CPU) = delete;
        CPUerror evaluate();
        ASM2LLVM_CODE_FILTER(Profiler profiler);
        void* convertVirtualAddrToPhysical(ui32 addr);
        using ui128 = struct { ui32 b[4]; };
    public:
        static struct Context
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
                ui32 _;
                ui128 lr0; ui128 lr1; ui128 lr2; ui128 lr3;
                ui128 lr4; ui128 lr5; ui128 lr6; ui128 lr7;
            }Register;
            ui32 pc;
            struct
            {
                union
                {
                    struct
                    {
                        ui32 permissionLvl   : 2;
                        ui32 reserved        : 10;
                        ui32 pureMappingAddr : 20;
                    };
                    ui32 bits;
                }CR;
                ui32 GPT;
            }ControlRegister;
            ui8* RAM = NULL;
            Stack(ui8) stack;
        }context;
        typedef void(*PtrToFunction)(Assembler::Command*);
        static PtrToFunction runFunction[];
        static const ui32 FUNCTION_TABLE_SIZE;
};


C_string getStringByErrorCode(CPUerror errorCode);

