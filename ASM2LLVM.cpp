#include "ASM2LLVM.h"
#include "CPU/CPU.h"
#include <algorithm>
#include <iostream>
#include <list>

using namespace Assembler;


void ASM2LLVMBuilder::codePrintStage()
{
    std::cout << "#[LLVM IR]:\n";
    std::string s;
    llvm::raw_string_ostream os(s);
    module->print(os, nullptr);
    os.flush();
    std::cout << s;
    std::cout << "#[LLVM IR] END\n\n\n";
    system("pause");
}

AsmError ASM2LLVMBuilder::ASM2LLVM(const C_string inputFile, const C_string outFile)
{
    AsmError errorCode = ASM_OK;

    //stage 1 generate list of commands from binary file
    parseBinaryStage(inputFile);

    //stage 2 init base block map structure, at the next stage we create llvm:BasicBlock*
    #ifndef LLVM_IR_SIMPLEST_PROGRAMM
        genBBListStage();
    #endif

    //stage 3 prepare registers
    LLVMPreparation(inputFile);

    //stage 4 generate llvm ir by list of commands for every base block
    codeGenerationStage();

    //stage 5 push IR into output file
    codePrintStage();

    //stage 6 run the LLVM IR
    LLVMRun();

    return ASM_OK;
}


string INTERP_FUNC_NAMES[] =
{
    #define DEF(name, machineCode, validStrOperand_1, validStrOperand_2, code) std::string("run_"#name),
    #include "Extend.h"
    #undef DEF
};

void* lazyFunctionCreator(const string& funcName)
{
    const std::string PREFIX = "_";
    for (ui32 i = 0; i < CPU::FUNCTION_TABLE_SIZE; i++)
        if (funcName == PREFIX+INTERP_FUNC_NAMES[i]) 
            return reinterpret_cast<void*>(CPU::runFunction[i]);
    return nullptr;
}

void ASM2LLVMBuilder::LLVMRun()
{
    InitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    LLVMLinkInMCJIT();

    std::string err_str;

    ExecutionEngine* ee = EngineBuilder(std::unique_ptr<Module>(module))
        .setEngineKind(EngineKind::JIT).
        setErrorStr(&err_str)
        .create();
    if (!ee)
    {
        printf("Could not create ExecutionEngine: %s", err_str.c_str());
        return;
    }
    ee->InstallLazyFunctionCreator(lazyFunctionCreator);


    #ifndef LLVM_IR_SIMPLEST_PROGRAMM
        ui64 adrToRegTable = reinterpret_cast<ui64>(&CPU::myCPU.Register);
        ee->addGlobalMapping("_ptr_reg", reinterpret_cast<ui64>(&adrToRegTable));
        ui64 adrToMemory = reinterpret_cast<ui64>(&CPU::myCPU.RAM[0]);
        ee->addGlobalMapping("_ptr_mem", reinterpret_cast<ui64>(&adrToMemory));
    #else
        ui64 adrToRegTable = reinterpret_cast<ui64>(&CPU::myCPU.Register);
        ee->addGlobalMapping("_ptr_reg", adrToRegTable);
    #endif

    ee->finalizeObject();
    

    std::cout << "#[LLVM IR EXEC]:\n";
    GenericValue result = ee->runFunction(mainFunc, vector<GenericValue>());
    std::cout << "#[LLVM IR EXEC] END\n";
    std::cout << ee->getErrorMessage() << std::endl;
    

    #ifdef LLVM_IR_SIMPLEST_PROGRAMM
        ui64 r = *result.IntVal.getRawData();
        printf("Returned: %lX (%ld)\n", r, r);
        printf(" &Registers: %lX\n", adrToRegTable);
    #endif

    printf("Result:\n");
    CPU::Instance().dump();
}