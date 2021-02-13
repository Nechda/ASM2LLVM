#include "ASM2LLVM/ASM2LLVM.h"


#ifdef LLVM_IR_SIMPLEST_PROGRAMM

void* lazyFunctionCreator(const string& funcName);

void Translator::LLVMGenSimplestProgram(const C_string sourceFile)
{
    m_module = new Module("Main_module", m_context);
    m_module->setSourceFileName(sourceFile);


    const ui32 REG_FILE_SIZE = 4;
    ui32 CPU_REG_FILE[REG_FILE_SIZE] = {};
    const ui32 PUSING_VALUE = 0xFFFFFFFF;

    IntegerType* t_64 = m_builder.getInt64Ty();
    IntegerType* t_32 = m_builder.getInt32Ty();
    PointerType* t_64_ptr = PointerType::get(t_64, 0);
    PointerType* t_32_ptr = PointerType::get(t_32, 0);

    ArrayType* regFileType = ArrayType::get(t_32, REG_FILE_SIZE);
    m_module->getOrInsertGlobal("regFile", regFileType);
    GlobalVariable* regFile = m_module->getNamedGlobal("regFile");


    FunctionType* funcType = FunctionType::get(m_builder.getInt64Ty(), false);
    m_mainFunc = Function::Create(funcType, llvm::Function::ExternalLinkage, "main_func", m_module);
    BasicBlock* entryBB = BasicBlock::Create(m_context, "entry", m_mainFunc);
    m_builder.SetInsertPoint(entryBB);

    Value* ptr_to_data = m_builder.CreateConstGEP2_32(
        regFileType,
        regFile,
        0,
        2
    );

    m_builder.CreateStore(m_builder.getInt32(PUSING_VALUE), ptr_to_data);
    m_builder.CreateRet(
        m_builder.CreatePtrToInt(
            ptr_to_data,
            m_builder.getInt32Ty()
        )
    );
    codePrintStage(nullptr);

    LLVMInitializeNativeTarget();
    InitializeNativeTargetAsmParser();
    InitializeNativeTargetAsmPrinter();

    std::string err_str;
    llvm::EngineBuilder& engineBuilder = EngineBuilder(std::unique_ptr<Module>(m_module));
    engineBuilder.setErrorStr(&err_str);
    engineBuilder.setEngineKind(llvm::EngineKind::JIT);
    //engineBuilder.setMCPU("skylake");
    //engineBuilder.setMArch("x86-64");
    engineBuilder.setErrorStr(&err_str);

    ExecutionEngine* ee = engineBuilder.create();
    if (!ee)
    {
        printf("Could not create ExecutionEngine: %s", err_str.c_str());
        return;
    }

    ee->InstallLazyFunctionCreator(lazyFunctionCreator);
    ee->addGlobalMapping(regFile, (void*)&CPU_REG_FILE[0]);
    ee->finalizeObject();


    for (ui8 i = 0; i < REG_FILE_SIZE; i++)
        std::cout << "Register[" << (ui16)i << "]: " << std::bitset<32>(CPU_REG_FILE[i]) << std::endl;

    std::cout << "#[LLVM IR EXEC]:\n";
    GenericValue result = ee->runFunction(m_mainFunc, vector<GenericValue>());
    std::cout << "#[LLVM IR EXEC] END\n";

    ui64 r = *result.IntVal.getRawData();
    printf("     Returned: 0x%lX\n", r);
    printf("&Registers[0]: 0x%lX\n", &CPU_REG_FILE[0]);
    printf("&Registers[1]: 0x%lX\n", &CPU_REG_FILE[1]);

    for (ui8 i = 0; i < REG_FILE_SIZE; i++)
        std::cout << "Register[" << (ui16)i << "]: " << std::bitset<32>(CPU_REG_FILE[i]) << std::endl;

}
#endif