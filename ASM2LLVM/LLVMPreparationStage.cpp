#include"ASM2LLVM.h"

void ASM2LLVMBuilder::LLVMPreparation(const C_string sourceFile)
{
    module = new Module("Main_module", context);
    module->setSourceFileName(sourceFile);
    module->getOrInsertGlobal("ptr_reg", Type::getInt32PtrTy(context));
    module->getOrInsertGlobal("ptr_mem", Type::getInt8PtrTy(context));


    FunctionType* funcType = FunctionType::get(builder.getVoidTy(), false);
    mainFunc = Function::Create(funcType, Function::ExternalLinkage, "main_func", module);
    BasicBlock* entryBB;
    for (ui32 i = 0; i < funcArray.size(); i++)
    {
        funcArray[i].first = Function::Create(funcType, Function::ExternalLinkage, "func_" + std::to_string(i), module);
        entryBB = BasicBlock::Create(context, "entry", funcArray[i].first);
        builder.SetInsertPoint(entryBB);
        builder.CreateLoad(module->getNamedGlobal("ptr_reg"), "registers_" + std::to_string(i));
        builder.CreateLoad(module->getNamedGlobal("ptr_mem"), "memory_" + std::to_string(i));
        BlockInfo& bbAfterEntry = bbArray[funcArray[i].second];
        bbAfterEntry.bb = BasicBlock::Create(context, "BB_" + std::to_string(funcArray[i].second), funcArray[i].first);
        builder.SetInsertPoint(entryBB);
        builder.CreateBr(bbAfterEntry.bb);
    }

    entryBB = BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(entryBB);
    builder.CreateCall(funcArray[0].first);
    builder.CreateRetVoid();


    for(ui32 i = 0; i < bbArray.size() - 1; i++)
        if(!bbArray[i].bb)
            bbArray[i].bb = BasicBlock::Create(context, "BB_" + std::to_string(i), funcArray[bbArray[i].funcIndex].first);

}