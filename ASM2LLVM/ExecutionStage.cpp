#include "ASM2LLVM.h"
#include "CPU/CPU.h"


static const string INTERP_FUNC_NAMES[] =
{
    #define INTERPRETATED_FUNCTION(name, code)\
        std::string("run_"#name),
        #include "InterpretedFunctions.inc"    
    #undef INTERPRETATED_FUNCTION
};

/*
    Массив функций, которые будем дергать из IR
*/
#define INTERPRETATED_FUNCTION(name, code)\
        extern "C" void run_##name(ui16, ui32, ui32);
#include "InterpretedFunctions.inc"
#undef INTERPRETATED_FUNCTION

typedef void(*PtrToFunction)(ui16, ui32, ui32);
static const PtrToFunction interpretate[] =
{
    #define INTERPRETATED_FUNCTION(name, code)\
        run_##name,
    #include "InterpretedFunctions.inc"
    #undef INTERPRETATED_FUNCTION
};

void* lazyFunctionCreator(const string& funcName)
{
    for (ui32 i = 0; i < CPU::FUNCTION_TABLE_SIZE; i++)
        if (funcName == INTERP_FUNC_NAMES[i])
            return reinterpret_cast<void*>(interpretate[i]);
    return nullptr;
}



/// эти переменные объявлены в staticLib.cpp
extern "C" ui32* regTable;
extern "C" ui8* memory;



void Translator::runJIT()
{
    InitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    LLVMLinkInMCJIT();

    std::string err_str;

    ExecutionEngine* ee = EngineBuilder(std::unique_ptr<Module>(m_module))
        .setEngineKind(EngineKind::JIT)
        .setErrorStr(&err_str)
        .create();
    if (!ee)
    {
        printf("Could not create ExecutionEngine: %s", err_str.c_str());
        return;
    }
    ee->InstallLazyFunctionCreator(lazyFunctionCreator);

    regTable = &CPU::context.Register.eax;
    memory = &CPU::context.RAM[0];

    ee->addGlobalMapping(m_reg_table, &CPU::context.Register.eax);
    ee->addGlobalMapping(m_memory, &CPU::context.RAM[0]);
    ee->finalizeObject();


    printf("#[LLVM IR EXEC]:\n");
    GenericValue result = ee->runFunction(m_mainFunc, vector<GenericValue>());
    printf("#[LLVM IR EXEC] END\n");

    printf("CPU dump result:\n");
    CPU::Instance().dump(stdout);


    FILE* output = fopen("memory.bin", "wb");
    Assert_c(output);
    if (!output)
    {
        logger.push("Dumping memory", "Cant open file for dumping memory.");
        return;
    }

    fwrite(memory, sizeof(ui8), CPU::context.ramSize, output);

    printf("Memory dump has wroten into memory.bin\n");
    fclose(output);
}
