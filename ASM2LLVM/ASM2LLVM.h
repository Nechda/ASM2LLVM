#pragma once
#include <vector>
#include <map>
#include "Types.h"
#include "Asm/Asm.h"


#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>

#include <boost/circular_buffer.hpp>


#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>



using std::vector;
using std::pair;
using std::string;
using namespace llvm;
using boost::circular_buffer;
using Assembler::Command;
using Assembler::Disassembler;
using Assembler::AsmError;

extern const ui8 COUNT_REGISTERS;


enum TranslatorError
{
    TR_OK,
    TR_ERROR_PARSE_BINARY,
    TR_ERROR_CODE_GENERATION
};

//#define LLVM_IR_SIMPLEST_PROGRAMM
#define LLVM_IR_PRINT_DISASSEMBLER

class ASM2LLVMBuilder
{
    private:
        vector<Command> commandList;

        struct BlockInfo
        {
            BasicBlock* bb = nullptr;
            ui32 sLine = 0;
            ui32 eLine = 0;
            i32 funcIndex = 0;
        };
        ui32 currBBIndex = 0;
        vector<BlockInfo> bbArray;
        vector<pair<Function*,ui32>> funcArray;

        Value* ptr_reg_table;
        Value* ptr_memory;
        Function* mainFunc;


        LLVMContext context;
        Module* module;
        IRBuilder<> builder;


    public:
        Disassembler& disasembler;
        static ASM2LLVMBuilder& Instance()
        {
            static ASM2LLVMBuilder theInstance;
            return theInstance;
        }
        AsmError ASM2LLVM(const C_string inputFile, const C_string outFile);
    private:
        ASM2LLVMBuilder() :
            builder(context),
            disasembler(Disassembler::Instance()) {};
        ASM2LLVMBuilder(const ASM2LLVMBuilder&) = delete;
        ASM2LLVMBuilder& operator=(const ASM2LLVMBuilder&) = delete;

        TranslatorError parseBinaryStage(const C_string inputFile);
        TranslatorError genBBListStage();
        TranslatorError LLVMPreparation(const C_string sourceFile);
        TranslatorError codeGenerationStage();
        TranslatorError codePrintStage();
        void LLVMRun();

        AsmError LLVMPareseCommand(const Command& cmd, bool& isBrhCommand, const BlockInfo& blockInfo);
        AsmError parseExternalFunctions(const Command& cmd);
        AsmError parseOperands(const Command& cmd);
        AsmError parseGeneral(const Command& cmd, bool& isEndBBCmd);
        AsmError parseBranches(const Command& cmd, bool& isEndBBCmd);

        
        //need c++17
        template<typename Rx_, typename ... Args>
        using pMethod = Rx_ (IRBuilder<>::*)(Args ...);
        template<typename Rx_, typename ... Args>
        Value* createOnStack(pMethod<Rx_, Args ...> exec_func, Args ... args)
        {
            Value* res = (builder.*exec_func)(args ...);
            ringBufValue.push_back(res);
            return res;
        }

        #ifdef LLVM_IR_SIMPLEST_PROGRAMM
            void LLVMGenSimplestProgram(const C_string sourceFile);
        #endif

};


inline bool isBrachComand(const ui8 cmdOpCode)
{
    return Assembler::CMD_JE <= cmdOpCode
        && cmdOpCode <= Assembler::CMD_JBE
        || cmdOpCode == Assembler::CMD_JMP
        || cmdOpCode == Assembler::CMD_CALL;
}