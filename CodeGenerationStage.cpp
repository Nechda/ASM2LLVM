#include "ASM2LLVM.h"
#include "CPU/CPU.h"
#include "Asm/Asm.h"
#include <stack>
using std::stack;
using namespace Assembler;

void ASM2LLVMBuilder::codeGenerationStage()
{
    #ifdef LLVM_IR_SIMPLEST_PROGRAMM
        Value* tmp = builder.CreateConstGEP1_32(Type::getInt32PtrTy(context),module->getNamedGlobal("ptr_reg"),0, "addr");
        builder.CreateRet(tmp);
    #else
        bool isLastCmdBrch = 0;
        for (ui32 i = 0; i < bbArray.size() - 1; i++)
        {
            BlockInfo& bbInfo = bbArray[i];
            // это очень странный способ получения ptr_reg_table & ptr_memory ....
            BasicBlock& bbEntry = funcArray[bbInfo.funcIndex].first->getEntryBlock();
            BasicBlock::iterator its = bbEntry.begin();
            ptr_reg_table = &(*its); its++;
            ptr_memory = &(*its);

            //парсим команды из блока
            isLastCmdBrch = 0;
            builder.SetInsertPoint(bbInfo.bb);
            currBBIndex = i;
            for (ui32 j = bbInfo.sLine; j <= bbInfo.eLine && !isLastCmdBrch; j++)
                LLVMPareseCommand(commandList[j], isLastCmdBrch, bbInfo);

            if (
                !isLastCmdBrch
             && bbArray[i].funcIndex != bbArray[i + 1].funcIndex
             && i != bbArray.size() - 2
            )
            {
                printf(
                    "Error: Two consecutive blocks "
                    "belong to different functions "
                    "and are not separated by a "
                    "branch operator");
                return;
            }

            if (!isLastCmdBrch && i == bbArray.size() - 2)
                builder.CreateRetVoid();
            else if (!isLastCmdBrch)
                builder.CreateBr(bbArray[i+1].bb);
        }
    #endif
}


#define AND_(a,b)\
    stackValue.push(builder.CreateAnd({a, b}));
#define OR_(a,b)\
    stackValue.push(builder.CreateOr({a, b}));
#define AND_s()\
{\
    Value* a = stackValue.top();stackValue.pop();\
    Value* b = stackValue.top();stackValue.pop();\
    stackValue.push(builder.CreateAnd({a,b}));\
}
#define OR_s()\
{\
    Value* a = stackValue.top();stackValue.pop();\
    Value* b = stackValue.top();stackValue.pop();\
    stackValue.push(builder.CreateOr({a,b}));\
}
#define NOT_s()\
{\
    Value* a = stackValue.top();\
    stackValue.pop();\
    stackValue.push(builder.CreateICmpEQ(a,builder.getInt32(0)));\
}

#define CAST_I1_s()\
{\
    Value* a = stackValue.top();\
    stackValue.pop();\
    stackValue.push(builder.CreateICmpNE(a,builder.getInt32(0)));\
}

AsmError ASM2LLVMBuilder::LLVMPareseCommand(const Command& cmd, bool& isBrhCommand, const BlockInfo& blockInfo)
{
    Value* operand[2] = {};
    Value* operand_ptr[2] = {};
    stack<Value*> stackValue;

    for (ui8 i = 0; i < cmd.code.bits.nOperands; i++)
    {
        OperandType opType = getOperandType(cmd, i);
        switch (opType)
        {
            case OPERAND_REGISTER:
                operand_ptr[i] = builder.CreateConstGEP1_32(ptr_reg_table, cmd.operand[i].ivalue - 1);
                operand[i] = builder.CreateLoad(operand_ptr[i]);
                break;
            case OPERAND_NUMBER:
                operand[i] = ConstantInt::get(builder.getInt32Ty(), cmd.operand[i].ivalue);
                break;
                
            case OPERAND_MEMORY:
                stackValue.push(builder.CreateConstGEP1_32(ptr_memory, cmd.operand[i].ivalue));
                stackValue.push(builder.CreatePointerCast(stackValue.top(), Type::getInt32PtrTy(context)));
                operand_ptr[i] = stackValue.top();
                operand[i] = builder.CreateLoad(operand_ptr[i]);
                break;
            case OPERAND_MEM_BY_REG:
                stackValue.push(builder.CreateConstGEP1_32(ptr_reg_table, cmd.operand[i].ivalue - 1));
                stackValue.push(builder.CreateLoad(stackValue.top()));
                stackValue.push(builder.CreateGEP(ptr_memory, stackValue.top()));
                stackValue.push(builder.CreatePointerCast(stackValue.top(), Type::getInt32PtrTy(context)));
                operand_ptr[i] = stackValue.top();
                operand[i] = builder.CreateLoad(operand_ptr[i]);
                break;
                
            default:
                break;
        }
    }

    static FunctionType* CallType = FunctionType::get(builder.getVoidTy(), ArrayRef<Type*>({ builder.getInt8PtrTy() }), false);
    Value* instr_p = nullptr;
    ArrayRef<Value*> argc;
    Value* ESPRegisterPtr = nullptr;
    Value* ESPRegister = nullptr;


    if(!isBrachComand(cmd.code.bits.opCode))
    switch (cmd.code.bits.opCode)
    {
        case Disassembler::CMD_TYPE_MOV:
            builder.CreateStore(operand[1], operand_ptr[0]);
            break;
        case Disassembler::CMD_TYPE_ADD:
            stackValue.push(builder.CreateAdd(operand[0], operand[1]));
            builder.CreateStore(stackValue.top(), operand_ptr[0]);
            break;
        case Disassembler::CMD_TYPE_SUB:
            stackValue.push(builder.CreateSub(operand[0], operand[1]));
            builder.CreateStore(stackValue.top(), operand_ptr[0]);
            break;
        case Disassembler::CMD_TYPE_MUL:
            stackValue.push(builder.CreateMul(operand[0], operand[1]));
            builder.CreateStore(stackValue.top(), operand_ptr[0]);
            break;
        case Disassembler::CMD_TYPE_DIV:
            stackValue.push(builder.CreateFDiv(operand[0], operand[1]));
            builder.CreateStore(stackValue.top(), operand_ptr[0]);
            break;
        case Disassembler::CMD_TYPE_IN:
            instr_p = ConstantInt::get(builder.getInt64Ty(), reinterpret_cast<ui64>(&cmd));
            builder.CreateCall(module->getOrInsertFunction("run_IN", CallType), ArrayRef<Value*>({ instr_p }));
            break;
        case Disassembler::CMD_TYPE_OUT:
            instr_p = ConstantInt::get(builder.getInt64Ty(), reinterpret_cast<ui64>(&cmd));
            builder.CreateCall(module->getOrInsertFunction("run_OUT", CallType), ArrayRef<Value*>({ instr_p }));
            break;
        case Disassembler::CMD_TYPE_CMP:
            instr_p = ConstantInt::get(builder.getInt64Ty(), reinterpret_cast<ui64>(&cmd));
            builder.CreateCall(module->getOrInsertFunction("run_CMP", CallType), ArrayRef<Value*>({ instr_p }));
            break;
        case Disassembler::CMD_TYPE_DUMP:
            instr_p = ConstantInt::get(builder.getInt64Ty(), reinterpret_cast<ui64>(&cmd));
            builder.CreateCall(module->getOrInsertFunction("run_DUMP", CallType), ArrayRef<Value*>({ instr_p }));
            break;
        case Disassembler::CMD_TYPE_PUSH:
            ESPRegisterPtr = builder.CreateConstGEP1_32(ptr_reg_table, ESP_REG_INDEX);
            ESPRegister = builder.CreateLoad(ESPRegisterPtr);
            stackValue.push(builder.CreateGEP(ptr_memory, ESPRegister));
            stackValue.push(builder.CreatePointerCast(stackValue.top(), Type::getInt32PtrTy(context)));
            builder.CreateStore(operand[0], stackValue.top());

            stackValue.push(builder.CreateAdd(ESPRegister, builder.getInt32(BYTES_IN_REGISTER)));
            builder.CreateStore(stackValue.top(), ESPRegisterPtr);

            break;
        case Disassembler::CMD_TYPE_POP:
            ESPRegisterPtr = builder.CreateConstGEP1_32(ptr_reg_table, ESP_REG_INDEX);
            ESPRegister = builder.CreateLoad(ESPRegisterPtr);
            stackValue.push(builder.CreateSub(ESPRegister, builder.getInt32(BYTES_IN_REGISTER)));
            builder.CreateStore(stackValue.top(), ESPRegisterPtr);
            stackValue.push(builder.CreateGEP(ptr_memory, stackValue.top()));
            stackValue.push(builder.CreatePointerCast(stackValue.top(), Type::getInt32PtrTy(context)));
            stackValue.push(builder.CreateLoad(stackValue.top()));
            builder.CreateStore(stackValue.top(), operand_ptr[0]);
            break;
        case Disassembler::CMD_TYPE_RET:
            isBrhCommand |= 1;
            ESPRegisterPtr = builder.CreateConstGEP1_32(ptr_reg_table, ESP_REG_INDEX);
            ESPRegister = builder.CreateLoad(ESPRegisterPtr);
            stackValue.push(builder.CreateSub(ESPRegister, builder.getInt32(BYTES_IN_REGISTER)));
            builder.CreateStore(stackValue.top(), ESPRegisterPtr);
            builder.CreateRetVoid();
            break;
        case Disassembler::CMD_TYPE_HLT:
            isBrhCommand |= 1;
            builder.CreateRetVoid();
            break;
        default:
            break;
    }
    else
    {
        if (cmd.code.bits.opCode == Disassembler::CMD_TYPE_CALL)
        {
            ui32 funcIndex = bbArray[cmd.operand[0].ivalue].funcIndex;

            ESPRegisterPtr = builder.CreateConstGEP1_32(ptr_reg_table, ESP_REG_INDEX);
            ESPRegister = builder.CreateLoad(ESPRegisterPtr);
            stackValue.push(builder.CreateAdd(ESPRegister, builder.getInt32(BYTES_IN_REGISTER)));
            builder.CreateStore(stackValue.top(), ESPRegisterPtr);

            builder.CreateCall(funcArray[funcIndex].first);
            return ASM_OK;
        }

        isBrhCommand |= 1;
        if (cmd.code.bits.opCode == Disassembler::CMD_TYPE_JMP)
        {
            builder.CreateBr(bbArray[cmd.operand[0].ivalue].bb);
            return ASM_OK;
        }

        Value* eflRegister = builder.CreateLoad(builder.CreateConstGEP1_32(ptr_reg_table, EFL_REG_INDEX));
        switch (cmd.code.bits.opCode)
        {
            case Disassembler::CMD_TYPE_JE:
                AND_(eflRegister, builder.getInt32(1 << FLAG_ZF));
                CAST_I1_s();
                break;
            case Disassembler::CMD_TYPE_JNE:
                AND_(eflRegister, builder.getInt32(1 << FLAG_ZF));
                NOT_s();
                break;
            case Disassembler::CMD_TYPE_JA:
                AND_(eflRegister, builder.getInt32(1 << FLAG_CF));
                NOT_s();
                AND_(eflRegister, builder.getInt32(1 << FLAG_ZF));
                NOT_s();
                AND_s();
                break;
            case Disassembler::CMD_TYPE_JAE:
                AND_(eflRegister, builder.getInt32(1 << FLAG_CF));
                NOT_s();
                break;
            case Disassembler::CMD_TYPE_JB:
                AND_(eflRegister, builder.getInt32(1 << FLAG_CF));
                CAST_I1_s();
                break;
            case Disassembler::CMD_TYPE_JBE:
                AND_(eflRegister, builder.getInt32(1 << FLAG_CF));
                AND_(eflRegister, builder.getInt32(1 << FLAG_ZF));
                OR_s();
                CAST_I1_s();
                break;
            default:
                break;
        }
        builder.CreateCondBr(stackValue.top(), bbArray[cmd.operand[0].ivalue].bb, bbArray[currBBIndex+1].bb);
    }
    return ASM_OK;
}
