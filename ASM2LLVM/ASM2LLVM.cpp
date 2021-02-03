#include "ASM2LLVM.h"
#include "CPU/CPU.h"
#include <algorithm>
#include <iostream>
#include <list>

using namespace Assembler;


TranslatorError ASM2LLVMBuilder::codePrintStage()
{
    string s;
    raw_string_ostream os(s);
    module->print(os, nullptr);
    os.flush();
    printf("#[LLVM IR]:\n");
    printf("%s\n", s.c_str());
    printf("#[LLVM IR] END\n\n\n");
    system("pause");
    return TR_OK;
}

AsmError ASM2LLVMBuilder::ASM2LLVM(const C_string inputFile, const C_string outFile)
{
    AsmError errorCode = ASM_OK;

    #ifndef LLVM_IR_SIMPLEST_PROGRAMM
        //stage 1 generate list of commands from binary file
        parseBinaryStage(inputFile);

        //stage 2 init base block map structure, at the next stage we create llvm:BasicBlock*
        genBBListStage();

        //stage 3 prepare registers
        LLVMPreparation(inputFile);

        //stage 4 generate llvm ir by list of commands for every base block
        codeGenerationStage();

        //stage 5 push IR into output file
        codePrintStage();

        //stage 6 run the LLVM IR
        LLVMRun();
    #else
        //generate simple LLVM IR program and run it
        LLVMGenSimplestProgram(inputFile);
    #endif


    return ASM_OK;
}


static const string INTERP_FUNC_NAMES[] =
{
    #define DEF(name, mCode, vStr1, vStr2, code)\
        std::string("_run_"#name),
        #include "Extend.h"
    #undef DEF
};

void* lazyFunctionCreator(const string& funcName)
{
    for (ui32 i = 0; i < CPU::FUNCTION_TABLE_SIZE; i++)
        if (funcName == INTERP_FUNC_NAMES[i]) 
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
        .setEngineKind(EngineKind::JIT)
        .setErrorStr(&err_str)
        .create();
    if (!ee)
    {
        printf("Could not create ExecutionEngine: %s", err_str.c_str());
        return;
    }
    ee->InstallLazyFunctionCreator(lazyFunctionCreator);


    ui64 adrToRegTable = reinterpret_cast<ui64>(&CPU::myCPU.Register);
    ee->addGlobalMapping("_ptr_reg", reinterpret_cast<ui64>(&adrToRegTable));
    ui64 adrToMemory = reinterpret_cast<ui64>(&CPU::myCPU.RAM[0]);
    ee->addGlobalMapping("_ptr_mem", reinterpret_cast<ui64>(&adrToMemory));

    ee->finalizeObject();
    

    printf("#[LLVM IR EXEC]:\n");
    GenericValue result = ee->runFunction(mainFunc, vector<GenericValue>());
    printf("#[LLVM IR EXEC] END\n");
    printf("%s\n", ee->getErrorMessage().c_str());
    
    printf("CPU dump result:\n");
    CPU::Instance().dump();
}

#ifdef LLVM_IR_SIMPLEST_PROGRAMM
void ASM2LLVMBuilder::LLVMGenSimplestProgram(const C_string sourceFile)
{
    module = new Module("Main_module", context);
    module->setSourceFileName(sourceFile);
    module->getOrInsertGlobal("ptr_reg", Type::getInt32PtrTy(context));

    FunctionType* funcType = FunctionType::get(builder.getInt64Ty(), false);
    mainFunc = Function::Create(funcType, llvm::Function::ExternalLinkage, "main_func", module);
    BasicBlock* entryBB = BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(entryBB);

    Value* tmp = builder.CreateConstGEP1_32(
        Type::getInt32PtrTy(context),
        module->getNamedGlobal("ptr_reg"),
        0,
        "addr"
    );
    builder.CreateRet(tmp);



    InitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    LLVMLinkInMCJIT();

    std::string err_str;
    ExecutionEngine* ee = EngineBuilder(std::unique_ptr<Module>(module))
        .setEngineKind(EngineKind::JIT)
        .setErrorStr(&err_str)
        .create();
    if (!ee)
    {
        printf("Could not create ExecutionEngine: %s", err_str.c_str());
        return;
    }
    ee->InstallLazyFunctionCreator(lazyFunctionCreator);



    ui64 adrToRegTable = reinterpret_cast<ui64>(&CPU::myCPU.Register);
    ee->addGlobalMapping("_ptr_reg", adrToRegTable);
    ee->finalizeObject();


    std::cout << "#[LLVM IR EXEC]:\n";
    GenericValue result = ee->runFunction(mainFunc, vector<GenericValue>());
    std::cout << "#[LLVM IR EXEC] END\n";
    std::cout << ee->getErrorMessage() << std::endl;

    ui64 r = *result.IntVal.getRawData();
    printf("   Returned: %lX\n", r);
    printf(" &Registers: %lX\n", adrToRegTable);

    printf("Result:\n");
    CPU::Instance().dump();
}
#endif