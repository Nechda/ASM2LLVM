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

using std::vector;
using std::map;
using std::pair;
using std::string;
using namespace llvm;
using Assembler::Command;
using Assembler::Disassembler;
using Assembler::AsmError;

extern const ui8 COUNT_REGISTERS;

class ASM2LLVMBuilder
{
    private:
        vector<Command> commandList;

        ui32 currentBB = 0;

        struct BlockInfo
        {
            BasicBlock* bb = nullptr;
            ui32 sLine;
            ui32 eLine;
            i32 funcIndex;
            BlockInfo* nextInfoBlock;
        };

        map<ui32, BlockInfo> bbArray;
        vector<pair<Function*,ui32>> funcArray; // хочу, чтоб второй аргумент в паре давал номер первого блока в функции

        Value* ptr_reg_table;
        Value* ptr_memory;
        Function* mainFunc;


        LLVMContext context;
        Module* module;
        IRBuilder<> builder;
        map<std::string, Value*> NamedValues;
    public:
        Disassembler& disasembler;
        static ASM2LLVMBuilder& Instance()
        {
            static ASM2LLVMBuilder theInstance;
            return theInstance;
        }
        AsmError ASM2LLVM(const C_string inputFile, const C_string outFile);
    private:
        ASM2LLVMBuilder() : builder(context),
            disasembler(Disassembler::Instance()) {};
        ASM2LLVMBuilder(const ASM2LLVMBuilder&) = delete;
        ASM2LLVMBuilder& operator=(const ASM2LLVMBuilder&) = delete;


        AsmError LLVMPareseCommand(const Command& cmd, bool& isBrhCommand, const BlockInfo& blockInfo);


        void parseBinaryStage(const C_string inputFile);
        void genBBListStage();
        void LLVMPreparation(const C_string sourceFile);
        void codeGenerationStage();
        void codePrintStage();

        void LLVMRun();
};

/*
    TODO:
    [ ] переделать алгоритм преобразования адресов в номера команд (использовать unordered map)
    [x] добавить работу со стеком
    [x] поддержка операндов, обращающихся к памяти
    [x] поддержка call, ret
    [x] блок инициализации в начале каждой функции
*/

#define LLVM_IR_DEBUG_CODE 0
//#define LLVM_IR_SIMPLEST_PROGRAMM
#define LLVM_IR_PRINT_DISASSEMBLER

inline bool isBrachComand(const ui8 cmdOpCode)
{
    return Disassembler::CMD_TYPE_JE <= cmdOpCode
        && cmdOpCode <= Disassembler::CMD_TYPE_JBE
        || cmdOpCode == Disassembler::CMD_TYPE_JMP
        || cmdOpCode == Disassembler::CMD_TYPE_CALL;
}