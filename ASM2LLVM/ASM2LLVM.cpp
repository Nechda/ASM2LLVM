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


    ui64 adrToRegTable = reinterpret_cast<ui64>(&CPU::myCPU.Register);
    ee->addGlobalMapping("_ptr_reg", reinterpret_cast<ui64>(&adrToRegTable));
    ui64 adrToMemory = reinterpret_cast<ui64>(&CPU::myCPU.RAM[0]);
    ee->addGlobalMapping("_ptr_mem", reinterpret_cast<ui64>(&adrToMemory));

    ee->finalizeObject();


    printf("#[LLVM IR EXEC]:\n");
    GenericValue result = ee->runFunction(m_mainFunc, vector<GenericValue>());
    printf("#[LLVM IR EXEC] END\n");
    //printf("%s\n", ee->getErrorMessage().c_str());
    
    printf("CPU dump result:\n");
    CPU::Instance().dump(stdout);
}

#ifdef LLVM_IR_SIMPLEST_PROGRAMM

void Translator::LLVMGenSimplestProgram(const C_string sourceFile)
{
    
    m_module = new Module("Main_module", m_context);
    m_module->setSourceFileName(sourceFile);

    const ui32 REG_FILE_SIZE = 4;
    i32 CPU_REG_FILE[REG_FILE_SIZE] = { 4,7,1,2 };

    IntegerType* t_64 = m_builder.getInt64Ty();
    IntegerType* t_32 = m_builder.getInt32Ty();
    PointerType* t_64_ptr = PointerType::get(t_64, 0);
    PointerType* t_32_ptr = PointerType::get(t_32, 0);

    ArrayType* regFileType = ArrayType::get(t_32, REG_FILE_SIZE);
    m_module->getOrInsertGlobal("regFile", regFileType);
    GlobalVariable* regFile = m_module->getNamedGlobal("regFile");

    m_module->getOrInsertGlobal("ptr_reg", Type::getInt32Ty(m_context));
    GlobalVariable* ptr_reg_ = m_module->getNamedGlobal("ptr_reg");

   
    FunctionType* funcType = FunctionType::get(m_builder.getInt64Ty(), false);
    m_mainFunc = Function::Create(funcType, llvm::Function::ExternalLinkage, "main_func", m_module);
    BasicBlock* entryBB = BasicBlock::Create(m_context, "entry", m_mainFunc);
    m_builder.SetInsertPoint(entryBB);

    Value* ptr_to_data = m_builder.CreateConstGEP1_32(
        ptr_reg_,
        1
    );

    m_builder.CreateStore(m_builder.getInt32(0xAABBCCDD), ptr_to_data);
    m_builder.CreateRet(m_builder.CreatePtrToInt(ptr_to_data, m_builder.getInt64Ty()));

    
    string s;
    raw_string_ostream os(s);
    m_module->print(os, nullptr);
    os.flush();
    printf("#[LLVM IR]:\n");
    printf("%s\n", s.c_str());
    printf("#[LLVM IR] END\n\n\n");
    system("pause");


    InitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();

    std::string err_str;
    ExecutionEngine* ee = EngineBuilder(std::unique_ptr<Module>(m_module))
        .setErrorStr(&err_str)
        .create();
    if (!ee)
    {
        printf("Could not create ExecutionEngine: %s", err_str.c_str());
        return;
    }
    ee->InstallLazyFunctionCreator(lazyFunctionCreator);
    ee->addGlobalMapping(regFile, CPU_REG_FILE);
    ee->addGlobalMapping(ptr_reg_, CPU_REG_FILE);
    ee->finalizeObject();


    for (ui8 i = 0; i < REG_FILE_SIZE; i++)
        printf("reg[%d]:%d \t", i, CPU_REG_FILE[i]);
    printf("\n");

    std::cout << "#[LLVM IR EXEC]:\n";
    GenericValue result = ee->runFunction(m_mainFunc, vector<GenericValue>());
    std::cout << "#[LLVM IR EXEC] END\n";

    ui64 r = *result.IntVal.getRawData();
    printf("     Returned: 0x%lX\n", r);
    printf("&Registers[0]: 0x%lX\n", &CPU_REG_FILE[0]);

    for (ui8 i = 0; i < REG_FILE_SIZE; i++)
        printf("reg[%d]:%d \t", i, CPU_REG_FILE[i]);
    printf("\n");

}
#endif