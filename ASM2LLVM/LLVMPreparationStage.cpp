#include "ASM2LLVM.h"
#include "CPU/CPU.h"

TranslatorError Translator::LLVMPreparation(const C_string sourceFile, ui32 memorySize)
{       
    m_module = new Module("Main_module", m_context);
    m_module->setSourceFileName(sourceFile);

    //declaring external global variable,that allows us work with CPU's structure, described in CPU.h
    m_regTableType = ArrayType::get(m_builder.getInt32Ty(), COUNT_REGISTERS);
    m_memTableType = ArrayType::get(m_builder.getInt8Ty(), memorySize);

    m_module->getOrInsertGlobal("register", m_regTableType);
    m_module->getOrInsertGlobal("memory", m_memTableType);

    m_reg_table = m_module->getNamedGlobal("register");
    m_memory = m_module->getNamedGlobal("memory");

    FunctionType* funcType = FunctionType::get(m_builder.getVoidTy(), false);
    m_mainFunc = Function::Create(funcType, Function::ExternalLinkage, "main_func", m_module);
    BasicBlock* entryBB;
    
    //declare functions and entry bloks in it
    for (ui32 i = 0; i < m_funcArray.size(); i++)
    {
        m_funcArray[i].first = Function::Create(funcType, Function::ExternalLinkage, "func_" + std::to_string(i), m_module);
        entryBB = BasicBlock::Create(m_context, "entry", m_funcArray[i].first);
        m_builder.SetInsertPoint(entryBB);
    }
    
    //call @func_0() from @main_func()
    entryBB = BasicBlock::Create(m_context, "entry", m_mainFunc);
    m_builder.SetInsertPoint(entryBB);
    m_builder.CreateCall(m_funcArray[0].first);
    m_builder.CreateRetVoid();

    //create basic bloks
    for(i32 i = 0; i < m_bbArray.size() - 1; i++)
        m_bbArray[i].bb = BasicBlock::Create(m_context, "BB_" + std::to_string(i), m_funcArray[m_bbArray[i].funcIndex].first);

    //for each function in its entry block write jump operation to the basick block in function
    //that will contain llvm ir is generated from assembler
    for (ui32 i = 0; i < m_funcArray.size(); i++)
    {
        m_builder.SetInsertPoint(&m_funcArray[i].first->getEntryBlock());
        m_builder.CreateBr(m_bbArray[m_funcArray[i].second].bb);
    }


    return TR_OK;
}