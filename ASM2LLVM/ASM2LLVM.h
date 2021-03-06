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
#include <llvm/IR/LegacyPassManager.h>

#include "CircularBuffer.h"

#include "llvm/ExecutionEngine/JITSymbol.h"


using std::vector;
using std::pair;
using std::string;
using Assembler::Command;
using Assembler::Disassembler;
using Assembler::AsmError;
using namespace llvm;



enum TranslatorError
{
    TR_OK,
    TR_ERROR_PARSE_BINARY,
    TR_ERROR_CODE_GENERATION,
    TR_ERROR_FUNCTON_VERIFICATION,
    TR_ERROR_WRITING_IN_FILE
};


//This define is for creating simple LLVM IR program and exceuting it
//Usually uses for debug
//#define LLVM_IR_SIMPLEST_PROGRAMM

//This define is for printing asm command while parsing is not complete
//Also uses for debug
//#define LLVM_PRINT_DISASSEMBLER

class Translator
{
    private:
        vector<Command> m_commandList;      //list of disassembled commands
        vector<ui8> m_bytesFromDataSection; //list of bytes from binary's data section
        ui32 m_textSectionSize;             //size of .text section in bytes

        struct BlockInfo
        {
            BasicBlock* bb = nullptr;
            ui32 sLine = 0;    //first line in asm file for the block
            ui32 eLine = 0;    //last line in asm file for the block
            i32 funcIndex = 0; //number of function in LLVM IR
        };
        ui32 m_currBBIndex = 0;
        vector<BlockInfo> m_bbArray;
        vector<pair<Function*,ui32>> m_funcArray;

        ArrayType* m_regTableType;
        ArrayType* m_memTableType;

        GlobalVariable* m_reg_table; //variable for access to cpu's registers
        GlobalVariable* m_memory;    //variable for access to cpu's memory

        Function* m_mainFunc;


        LLVMContext m_context;
        Module* m_module;
        IRBuilder<> m_builder;

    public:
        static Translator& Instance()
        {
            static Translator theInstance;
            return theInstance;
        }
        TranslatorError ASM2LLVM(
            const C_string inputFile,
            const C_string outFile,
            ui32 memorySize = 128,
            bool doOptimization = false
        );
        void runJIT();
    private:
        Translator() :
            m_builder(m_context){};
        Translator(const Translator&) = delete;
        Translator& operator=(const Translator&) = delete;

        TranslatorError parseBinaryStage(const C_string inputFile);
        TranslatorError genBBStructureStage();
        TranslatorError LLVMPreparation(const C_string sourceFile, ui32 memorySize);
        TranslatorError codeGenerationStage();
        TranslatorError verificationStage();
        TranslatorError optimizationStage();
        TranslatorError codePrintStage(const C_string outFile);

        AsmError LLVMPareseCommand(const Command& cmd, bool& isBrhCommand, const BlockInfo& blockInfo);
        AsmError parseExternalFunctions(const Command& cmd);
        
            AsmError parseOperands(const Command& cmd, const ui8 offsetInReg = 0);
            AsmError parseGeneral(const Command& cmd, bool& isEndBBCmd);
            AsmError parseBranches(const Command& cmd, bool& isEndBBCmd);

            AsmError simd_parseCommand(const Command& cmd);
        
        template<typename Ret, typename ... Args>
        using pMethod = Ret (IRBuilder<>::*)(Args ...);
        template<typename Ret, typename ... Args>
        Value* createOnStack(pMethod<Ret, Args ...> exec_func, Args ... args)
        {
            Value* res = (m_builder.*exec_func)(args ...);
            ringBufValue.push_back(res);
            return res;
        }

        #ifdef LLVM_IR_SIMPLEST_PROGRAMM
            void LLVMGenSimplestProgram(const C_string sourceFile);
        #endif
};