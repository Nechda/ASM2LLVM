#include "ASM2LLVM.h"
#include <iostream>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/AggressiveInstCombine/AggressiveInstCombine.h>
#include <llvm/Transforms/Scalar/DeadStoreElimination.h>


using namespace Assembler;

bool isBranchCommand(const ui8 cmdOpCode);              
ui32& getOperandConsistAddrInBrchCommand(Assembler::Command& cmd);


/**
    Добавим вспомогательную функцию, которая
    будет определять задает ли команда операцию
    ветвления.
*/
bool isBranchCommand(const ui8 cmdOpCode)
{
    return CMD_JMP <= cmdOpCode && cmdOpCode <= CMD_JAE
        || CMD_FJE <= cmdOpCode && cmdOpCode <= CMD_FJAE
        || CMD_CALL == cmdOpCode;
}

/**
    Также потребуется функция, которая будет возвращать
    адрес перехода операторов ветвления.
*/
ui32& getOperandConsistAddrInBrchCommand(Command& cmd)
{
    return cmd.bits.opCode == CMD_JMP
        || cmd.bits.opCode == CMD_CALL
        ? cmd.operand[0].ivalue : cmd.operand[2].ivalue;
}


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
    else
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

    // transform sets of stores into memsets
    TheFPM->add(createMemCpyOptPass());

    // deleting unreachable code & merging consecutive blocks
    TheFPM->add(createCFGSimplificationPass());

    // combine redundant instructions
    TheFPM->add(createInstructionCombiningPass());

    TheFPM->add(createAggressiveInstCombinerPass());

    // multiple use lexically identical expressions
    TheFPM->add(createNewGVNPass());

    // deleting a lot of stores
    TheFPM->add(createDeadStoreEliminationPass());

    TheFPM->add(createEarlyCSEPass());
    TheFPM->add(createCFGSimplificationPass());

    TheFPM->doInitialization();

    for (const auto& it : m_funcArray)
        TheFPM->run(*it.first);

    delete TheFPM;
    return TR_OK;
}

TranslatorError
Translator::ASM2LLVM(const C_string inputFile, const C_string outFile, ui32 memorySize, bool doOptimization)
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
        errorCode = LLVMPreparation(inputFile, memorySize);
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