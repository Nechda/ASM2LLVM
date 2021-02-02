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

struct Params
{
    circular_buffer<Value*>& stackValue;
    IRBuilder<>& builder;
    LLVMContext& context;
};


static inline Value* crt_GEP(Value* ptr, Value* index, Params params)
{
    Value* res = params.builder.CreateGEP(ptr, index);
    params.stackValue.push_back(res);
    return res;
}

static inline Value* ctr_ConstGEP1_32(Value* ptr, ui32 index, Params params)
{
    Value* res = params.builder.CreateConstGEP1_32(ptr, index);
    params.stackValue.push_back(res);
    return res;
}

static inline Value* ctr_CastPtrInt32(Value* ptr, Params params)
{
    Value* res = params.builder.CreatePointerCast(ptr, Type::getInt32PtrTy(params.context));
    params.stackValue.push_back(res);
    return res;
}

static inline Value* crt_Load(Value* ptr, Params params)
{
    Value* res = params.builder.CreateLoad(ptr);
    params.stackValue.push_back(res);
    return res;
}

static inline void   crt_Store(Value* data, Value* ptr, Params params)
{
    params.builder.CreateStore(data, ptr);
}

static inline Value* crt_Add(Value* op1, Value* op2, Params params)
{
    Value* res = params.builder.CreateAdd(op1, op2);
    params.stackValue.push_back(res);
    return res;
}

static inline Value* crt_Sub(Value* op1, Value* op2, Params params)
{
    Value* res = params.builder.CreateSub(op1, op2);
    params.stackValue.push_back(res);
    return res;
}

static inline Value* crt_Mul(Value* op1, Value* op2, Params params)
{
    Value* res = params.builder.CreateMul(op1, op2);
    params.stackValue.push_back(res);
    return res;
}

static inline Value* crt_Div(Value* op1, Value* op2, Params params)
{
    Value* res = params.builder.CreateFDiv(op1, op2);
    params.stackValue.push_back(res);
    return res;
}

static inline Value* crt_AND(Value* a, Value* b, Params params)
{
    Value* res = params.builder.CreateAnd({ a, b });
    params.stackValue.push_back(res);
    return res;
}

static inline Value* crt_OR(Value* a, Value* b, Params params)
{
    Value* res = params.builder.CreateOr({ a, b });
    params.stackValue.push_back(res);
    return res;
}

static inline Value* ctr_NOT(Value* a, Params params)
{
    Value* res = params.builder.CreateNot(a);
    params.stackValue.push_back(res);
    return res;
}

static inline Value* crt_CAST_I1(Value* a, Params params)
{
    Value* res = params.builder.CreateICmpNE(a, params.builder.getInt32(0));
    params.stackValue.push_back(res);
    return res;
}


static inline void _CastPtrInt32(Params params)
{
    Value* a = params.stackValue.back(); params.stackValue.pop_back();
    params.stackValue.push_back(params.builder.CreatePointerCast(a, Type::getInt32PtrTy(params.context)));
}

static inline void _Load(Params params)
{
    Value* a = params.stackValue.back(); params.stackValue.pop_back();
    params.stackValue.push_back(params.builder.CreateLoad(a));
}

static inline void _AND(Params params)
{
    Value* a = params.stackValue.back(); params.stackValue.pop_back();
    Value* b = params.stackValue.back(); params.stackValue.pop_back();
    params.stackValue.push_back(params.builder.CreateAnd({ a, b }));
}

static inline void _OR(Params params)
{
    Value* a = params.stackValue.back(); params.stackValue.pop_back();
    Value* b = params.stackValue.back(); params.stackValue.pop_back();
    params.stackValue.push_back(params.builder.CreateOr({ a, b }));
}

static inline void _NOT(Params params)
{
    Value* a = params.stackValue.back(); params.stackValue.pop_back();
    params.stackValue.push_back(params.builder.CreateICmpEQ(a, params.builder.getInt32(0)));
}

static inline void _CAST_I1(Params params)
{
    Value* a = params.stackValue.back(); params.stackValue.pop_back();
    params.stackValue.push_back(params.builder.CreateICmpNE(a, params.builder.getInt32(0)));
}

#define PARAMS_STRCT {ringBufValue, builder, context}

#define c_GEP(ptr, index)          crt_GEP(ptr, index, PARAMS_STRCT)
#define c_ConstGEP1_32(ptr, index) ctr_ConstGEP1_32(ptr, index, PARAMS_STRCT)
#define c_CastPtrInt32(ptr)        ctr_CastPtrInt32(ptr, PARAMS_STRCT)
#define c_Load(ptr)                crt_Load(ptr, PARAMS_STRCT)
#define c_Store(data, ptr)         crt_Store(data, ptr, PARAMS_STRCT)

#define c_Add(op1, op2) crt_Add(op1, op2, PARAMS_STRCT)
#define c_Sub(op1, op2) crt_Sub(op1, op2, PARAMS_STRCT)
#define c_Mul(op1, op2) crt_Mul(op1, op2, PARAMS_STRCT)
#define c_Div(op1, op2) crt_Div(op1, op2, PARAMS_STRCT)

#define c_AND(a,b)   crt_AND(a, b, PARAMS_STRCT)
#define c_OR(a,b)    crt_OR(a, b, PARAMS_STRCT)
#define c_NOT(a)     crt_NOT(a, PARAMS_STRCT)
#define c_CAST_I1(a) crt_CAST_I1(a, PARAMS_STRCT)


#define s_CastPtrInt32() _CastPtrInt32(PARAMS_STRCT)
#define s_Load() _Load(PARAMS_STRCT)

#define s_AND()     _AND(PARAMS_STRCT)
#define s_OR()      _OR(PARAMS_STRCT)
#define s_NOT()     _NOT(PARAMS_STRCT)
#define s_CAST_I1() _CAST_I1(PARAMS_STRCT)

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
                c_AND(eflRegister, builder.getInt32(1 << FLAG_ZF));
                s_CAST_I1();
                break;
            case CMD_JNE:
                c_AND(eflRegister, builder.getInt32(1 << FLAG_ZF));
                s_NOT();
                break;
            case CMD_JA:
                c_AND(eflRegister, builder.getInt32(1 << FLAG_CF));
                s_NOT();
                c_AND(eflRegister, builder.getInt32(1 << FLAG_ZF));
                s_NOT();
                s_AND();
                break;
            case CMD_JAE:
                c_AND(eflRegister, builder.getInt32(1 << FLAG_CF));
                s_NOT();
                break;
            case CMD_JB:
                c_AND(eflRegister, builder.getInt32(1 << FLAG_CF));
                s_CAST_I1();
                break;
            case CMD_JBE:
                c_AND(eflRegister, builder.getInt32(1 << FLAG_CF));
                c_AND(eflRegister, builder.getInt32(1 << FLAG_ZF));
                s_OR();
                s_CAST_I1();
                break;
            default:
                break;
        }
        builder.CreateCondBr(ringBufValue.back(), bbArray[cmd.operand[0].ivalue].bb, bbArray[currBBIndex+1].bb);
    }
    return ASM_OK;
}
