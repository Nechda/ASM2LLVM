#include "ASM2LLVM.h"
#include "CPU/CPU.h"
#include "Asm/Asm.h"
#include <stack>
#include <boost/circular_buffer.hpp>

using std::stack;
using boost::circular_buffer;
using namespace Assembler;

void ASM2LLVMBuilder::codeGenerationStage()
{
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
}



#define CODE_GENERATE_MACRO
#include "CodegenMacro.h"

/*
    \brief Макросы, генерирующие команды ir, сгенерированная команда помещается в стек
*/
#define c_GEP(ptr, index)          DECLARE_FUNC_WITH_2_PARAMS(GEP)(ptr,index)
#define c_ConstGEP1_32(ptr, index) DECLARE_FUNC_WITH_2_PARAMS_I(ConstGEP1_32)(ptr, index)
#define c_CastPtrInt32(ptr)        DECLARE_FUNC_WITH_2_PARAMS(PointerCast)(ptr,Type::getInt32PtrTy(context))
#define c_Load(ptr)                DECLARE_FUNC_WITH_1_PARAMS(Load)(ptr)
#define c_Store(data, ptr)         DECLARE_FUNC_WITH_2_PARAMS(Store)(data, ptr); ringBufValue.pop_back()

#define c_Add(op1, op2)            DECLARE_FUNC_WITH_2_PARAMS(Add) (op1, op2)
#define c_Sub(op1, op2)            DECLARE_FUNC_WITH_2_PARAMS(Sub) (op1, op2) 
#define c_Mul(op1, op2)            DECLARE_FUNC_WITH_2_PARAMS(Mul) (op1, op2)
#define c_Div(op1, op2)            DECLARE_FUNC_WITH_2_PARAMS(FDiv)(op1, op2)

#define c_Not(op1, op2)            DECLARE_FUNC_WITH_1_PARAMS(Not)(op1)
#define c_Cast_i1(op1)             DECLARE_FUNC_WITH_2_PARAMS(ICmpNE)(op1,builder.getInt32(0))
#define c_And(op1, op2)      DECLARE_FUNC_WITH_2_UNION_PARAMS(And)(op1,op2)
#define c_Or (op1, op2)      DECLARE_FUNC_WITH_2_UNION_PARAMS(Or )(op1,op2)

/*
    \brief Макросы, генерирующие команды ir, аргументы команд берутся из стека
*/
#define s_CastPtrInt32() DECLARE_FUNC_WITH_1_PARAMS_STK_1(PointerCast, Type)(Type::getInt32PtrTy(context))
#define s_Load()         DECLARE_FUNC_WITH_1_PARAMS_STK(Load)()
#define s_And()          DECLARE_FUNC_WITH_2_UNION_PARAMS_STK(And)()
#define s_Or()           DECLARE_FUNC_WITH_2_UNION_PARAMS_STK(Or)()
#define s_Not()          DECLARE_FUNC_WITH_1_PARAMS_STK_1(ICmpEQ, Value)(builder.getInt32(0))
#define s_Cast_i1()      DECLARE_FUNC_WITH_1_PARAMS_STK_1(ICmpNE, Value)(builder.getInt32(0))


AsmError ASM2LLVMBuilder::LLVMPareseCommand(const Command& cmd, bool& isBrhCommand, const BlockInfo& blockInfo)
{
    Value* operand[2] = {};
    Value* operand_ptr[2] = {};
    static circular_buffer<Value*> ringBufValue(RING_BUFFER_CAPACITY);

    for (ui8 i = 0; i < cmd.code.bits.nOperands; i++)
    {
        OperandType opType = getOperandType(cmd, i);
        switch (opType)
        {
            case OPERAND_REGISTER:
                operand_ptr[i] = c_ConstGEP1_32(ptr_reg_table, cmd.operand[i].ivalue - 1);
                operand[i] = c_Load(operand_ptr[i]);
                break;
            case OPERAND_NUMBER:
                operand[i] = builder.getInt32(cmd.operand[i].ivalue);
                break;
            case OPERAND_MEMORY:
                c_ConstGEP1_32(ptr_memory, cmd.operand[i].ivalue);
                s_CastPtrInt32();
                operand_ptr[i] = ringBufValue.back();
                operand[i] = c_Load(operand_ptr[i]);
                break;
            case OPERAND_MEM_BY_REG:
                c_ConstGEP1_32(ptr_reg_table, cmd.operand[i].ivalue - 1);
                s_Load();
                c_GEP(ptr_memory, ringBufValue.back());
                s_CastPtrInt32();
                operand_ptr[i] = ringBufValue.back();
                operand[i] = c_Load(operand_ptr[i]);
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
        case CMD_MOV:
            c_Store(operand[1], operand_ptr[0]);
            break;
        case CMD_ADD:
            c_Add(operand[0], operand[1]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_SUB:
            c_Sub(operand[0], operand[1]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_MUL:
            c_Mul(operand[0], operand[1]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_DIV:
            c_Div(operand[0], operand[1]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_IN:
            instr_p = ConstantInt::get(builder.getInt64Ty(), reinterpret_cast<ui64>(&cmd));
            builder.CreateCall(module->getOrInsertFunction("run_IN", CallType), ArrayRef<Value*>({ instr_p }));
            break;
        case CMD_OUT:
            instr_p = ConstantInt::get(builder.getInt64Ty(), reinterpret_cast<ui64>(&cmd));
            builder.CreateCall(module->getOrInsertFunction("run_OUT", CallType), ArrayRef<Value*>({ instr_p }));
            break;
        case CMD_CMP:
            instr_p = ConstantInt::get(builder.getInt64Ty(), reinterpret_cast<ui64>(&cmd));
            builder.CreateCall(module->getOrInsertFunction("run_CMP", CallType), ArrayRef<Value*>({ instr_p }));
            break;
        case CMD_DUMP:
            instr_p = ConstantInt::get(builder.getInt64Ty(), reinterpret_cast<ui64>(&cmd));
            builder.CreateCall(module->getOrInsertFunction("run_DUMP", CallType), ArrayRef<Value*>({ instr_p }));
            break;
        case CMD_PUSH:
            ESPRegisterPtr = c_ConstGEP1_32(ptr_reg_table, ESP_REG_INDEX);
            ESPRegister = c_Load(ESPRegisterPtr);
            c_GEP(ptr_memory, ESPRegister);
            s_CastPtrInt32();
            c_Store(operand[0], ringBufValue.back());
            c_Add(ESPRegister, builder.getInt32(BYTES_IN_REGISTER));
            c_Store(ringBufValue.back(), ESPRegisterPtr);
            break;
        case CMD_POP:
            ESPRegisterPtr = c_ConstGEP1_32(ptr_reg_table, ESP_REG_INDEX);
            ESPRegister = c_Load(ESPRegisterPtr);
            c_Sub(ESPRegister, builder.getInt32(BYTES_IN_REGISTER));
            c_Store(ringBufValue.back(), ESPRegisterPtr);
            c_GEP(ptr_memory, ringBufValue.back());
            s_CastPtrInt32();
            s_Load();
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_RET:
            isBrhCommand |= 1;
            ESPRegisterPtr = c_ConstGEP1_32(ptr_reg_table, ESP_REG_INDEX);
            ESPRegister = c_Load(ESPRegisterPtr);
            c_Sub(ESPRegister, builder.getInt32(BYTES_IN_REGISTER));
            c_Store(ringBufValue.back(), ESPRegisterPtr);
            builder.CreateRetVoid();
            break;
        case CMD_HLT:
            isBrhCommand |= 1;
            builder.CreateRetVoid();
            break;
        default:
            break;
    }
    else
    {
        if (cmd.code.bits.opCode == CMD_CALL)
        {
            ui32 funcIndex = bbArray[cmd.operand[0].ivalue].funcIndex;

            ESPRegisterPtr = c_ConstGEP1_32(ptr_reg_table, ESP_REG_INDEX);
            ESPRegister = c_Load(ESPRegisterPtr);
            c_Add(ESPRegister, builder.getInt32(BYTES_IN_REGISTER));
            c_Store(ringBufValue.back(), ESPRegisterPtr);

            builder.CreateCall(funcArray[funcIndex].first);
            return ASM_OK;
        }

        isBrhCommand |= 1;
        if (cmd.code.bits.opCode == CMD_JMP)
        {
            builder.CreateBr(bbArray[cmd.operand[0].ivalue].bb);
            return ASM_OK;
        }

        Value* eflRegister = builder.CreateLoad(builder.CreateConstGEP1_32(ptr_reg_table, EFL_REG_INDEX));
        switch (cmd.code.bits.opCode)
        {
            case CMD_JE:
                c_And(eflRegister, builder.getInt32(1 << FLAG_ZF));
                s_Cast_i1();
                break;
            case CMD_JNE:
                c_And(eflRegister, builder.getInt32(1 << FLAG_ZF));
                s_Not();
                break;
            case CMD_JA:
                c_And(eflRegister, builder.getInt32(1 << FLAG_CF));
                s_Not();
                c_And(eflRegister, builder.getInt32(1 << FLAG_ZF));
                s_Not();
                s_And();
                break;
            case CMD_JAE:
                c_And(eflRegister, builder.getInt32(1 << FLAG_CF));
                s_Not();
                break;
            case CMD_JB:
                c_And(eflRegister, builder.getInt32(1 << FLAG_CF));
                s_Cast_i1();
                break;
            case CMD_JBE:
                c_And(eflRegister, builder.getInt32(1 << FLAG_CF));
                c_And(eflRegister, builder.getInt32(1 << FLAG_ZF));
                s_Or();
                s_Cast_i1();
                break;
            default:
                break;
        }
        builder.CreateCondBr(ringBufValue.back(), bbArray[cmd.operand[0].ivalue].bb, bbArray[currBBIndex+1].bb);
    }
    return ASM_OK;
}
