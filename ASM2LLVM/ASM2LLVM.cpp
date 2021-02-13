#include "ASM2LLVM.h"
#include "CPU/CPU.h"
#include <algorithm>
#include <iostream>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>


using namespace Assembler;


TranslatorError Translator::codePrintStage(const C_string outFile)
{
    TranslatorError errorCode = TR_OK;
    Stream stream = nullptr;
    stream = !outFile ? stdout : fopen(outFile, "w");
    if (!stream)
    {
        logger.push("Warning", "{Print IR}: "
            "There are problems related to open file: %s",
            outFile);
        errorCode = TR_ERROR_WRITING_IN_FILE;
    }

    string s;
    raw_string_ostream os(s);
    m_module->print(os, nullptr);
    os.flush();
    if(stream == stdout) fprintf(stream,"#[LLVM IR]:\n");
    fprintf(stream,"%s\n", s.c_str());
    if (stream == stdout) fprintf(stream,"#[LLVM IR] END\n\n\n");

    if (stream != stdout)
        printf("LLVM IR code has wrote in %s file.\n", outFile);

    system("pause");
    return errorCode;
}


TranslatorError Translator::verificationStage()
{
    bool isErrorOccur = 0;
    string s;
    raw_string_ostream os(s);
    for (ui32 i = 0; i < m_funcArray.size() && !isErrorOccur; i++)
        isErrorOccur |= verifyFunction(*m_funcArray[i].first, &os);
    os.flush();

    if (isErrorOccur)
    {
        printf("[Error]: {Function verification}: %s\n", s.c_str());
        logger.push("Error", "{Function verification}: %s", s.c_str());
        return TR_ERROR_FUNCTON_VERIFICATION;
    }

    return TR_OK;
}

TranslatorError Translator::optimizationStage()
{
    // create a new pass manager
    legacy::FunctionPassManager* TheFPM = new legacy::FunctionPassManager(m_module);

    // combine redundant instructions
    TheFPM->add(createInstructionCombiningPass());

    // deleting unreachable code & merging consecutive blocks
    TheFPM->add(createCFGSimplificationPass());

    // multiple use lexically identical expressions
    TheFPM->add(createNewGVNPass());

    //TheFPM->add(createEarlyCSEPass());

    TheFPM->doInitialization();

    for (const auto& it : m_funcArray)
        TheFPM->run(*it.first);

    delete TheFPM;
    return TR_OK;
}

TranslatorError Translator::ASM2LLVM(const C_string inputFile, const C_string outFile, bool doOptimization)
{
    TranslatorError errorCode = TR_OK;
    #define errorCheck() if(errorCode != TR_OK) return errorCode;

    #ifndef LLVM_IR_SIMPLEST_PROGRAMM
        //stage 1 generate list of commands from binary file
        errorCode = parseBinaryStage(inputFile);
        errorCheck();

        //stage 2 init base block array structure & separate code by fuctions
        errorCode = genBBStructureStage();
        errorCheck();

        //stage 3 allocate memory for BasicBlock* and Function* 
        errorCode = LLVMPreparation(inputFile);
        errorCheck();

        //stage 4 generate llvm ir by list of commands for every base block
        errorCode = codeGenerationStage();
        errorCheck();

        //stage 5 verify IR code
        errorCode = verificationStage();
        errorCheck();


        //stage 6 do some optimizations
        if (doOptimization)
        {
            errorCode = optimizationStage();
            errorCheck();
        }

        //stage 7 push IR into output file
        codePrintStage(outFile);
        errorCheck();
    #else
        //generate simple LLVM IR program and run it
        LLVMGenSimplestProgram(inputFile);
    #endif

    #undef errorCheck

    return TR_OK;
}


static const string INTERP_FUNC_NAMES[] =
{
    #define DEF(name, mCode, vStr1, vStr2, code)\
        std::string("run_"#name),
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

    ee->addGlobalMapping(m_reg_table, &CPU::myCPU.Register.eax);
    ee->addGlobalMapping(m_memory, &CPU::myCPU.RAM[0]);
    ee->finalizeObject();


    printf("#[LLVM IR EXEC]:\n");
    GenericValue result = ee->runFunction(m_mainFunc, vector<GenericValue>());
    printf("#[LLVM IR EXEC] END\n");
    
    printf("CPU dump result:\n");
    CPU::Instance().dump(stdout);
}