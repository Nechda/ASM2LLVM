#include"ASM2LLVM.h"

TranslatorError Translator::LLVMPreparation(const C_string sourceFile)
{
    m_module = new Module("Main_module", m_context);
    m_module->setSourceFileName(sourceFile);
    m_module->getOrInsertGlobal("ptr_reg", Type::getInt32PtrTy(m_context));
    m_module->getOrInsertGlobal("ptr_mem", Type::getInt8PtrTy(m_context));


    FunctionType* funcType = FunctionType::get(m_builder.getVoidTy(), false);
    m_mainFunc = Function::Create(funcType, Function::ExternalLinkage, "main_func", m_module);
    BasicBlock* entryBB;
    for (ui32 i = 0; i < m_funcArray.size(); i++)
    {
        m_funcArray[i].first = Function::Create(funcType, Function::ExternalLinkage, "func_" + std::to_string(i), m_module);
        entryBB = BasicBlock::Create(m_context, "entry", m_funcArray[i].first);
        m_builder.SetInsertPoint(entryBB);
        m_builder.CreateLoad(m_module->getNamedGlobal("ptr_reg"), "registers_" + std::to_string(i));
        m_builder.CreateLoad(m_module->getNamedGlobal("ptr_mem"), "memory_" + std::to_string(i));
        BlockInfo& bbAfterEntry = m_bbArray[m_funcArray[i].second];
        bbAfterEntry.bb = BasicBlock::Create(m_context, "BB_" + std::to_string(m_funcArray[i].second), m_funcArray[i].first);
        m_builder.SetInsertPoint(entryBB);
        m_builder.CreateBr(bbAfterEntry.bb);
    }

    entryBB = BasicBlock::Create(m_context, "entry", m_mainFunc);
    m_builder.SetInsertPoint(entryBB);
    m_builder.CreateCall(m_funcArray[0].first);
    m_builder.CreateRetVoid();


    for(i32 i = 0; i < m_bbArray.size() - 1; i++)
        if(!m_bbArray[i].bb)
            m_bbArray[i].bb = BasicBlock::Create(m_context, "BB_" + std::to_string(i), m_funcArray[m_bbArray[i].funcIndex].first);

    return TR_OK;
}