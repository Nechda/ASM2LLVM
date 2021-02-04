#include "ASM2LLVM.h"
#include "CPU/CPU.h"
#include "Asm/Asm.h"

using namespace Assembler;

const ui32 RING_BUFFER_CAPACITY = 8;

static Value* operand[2] = { nullptr, nullptr };
static Value* operand_ptr[2] = { nullptr, nullptr };
static circular_buffer<Value*> ringBufValue = circular_buffer<Value*>(RING_BUFFER_CAPACITY);


TranslatorError ASM2LLVMBuilder::codeGenerationStage()
{
    bool isErrorOccur = 0;
    isErrorOccur |= bbArray.size() <= 1;


    bool isLastCmdBrch = 0;
    for (ui32 i = 0; i < bbArray.size() - 1 && !isErrorOccur; i++)
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
        for (ui32 j = bbInfo.sLine; j <= bbInfo.eLine && !isLastCmdBrch && !isErrorOccur; j++)
            isErrorOccur |=
            LLVMPareseCommand(commandList[j], isLastCmdBrch, bbInfo) != ASM_OK;

        if (
            !isLastCmdBrch
            && bbArray[i].funcIndex != bbArray[i + 1].funcIndex
            && i != bbArray.size() - 2
        )
        {
            logger.push("Error", "{Code generation}: "
                "Two consecutive blocks "
                "belong to different functions "
                "and are not separated by a "
                "branch operator");
            return TR_ERROR_CODE_GENERATION;
        }

        if (!isLastCmdBrch && i == bbArray.size() - 2)
            builder.CreateRetVoid();
        else if (!isLastCmdBrch)
            builder.CreateBr(bbArray[i+1].bb);
    }

    return isErrorOccur ? TR_ERROR_CODE_GENERATION : TR_OK;
}



#define CODE_GENERATE_MACRO
#include "CodegenMacro.h"

/*
    \brief Макросы, генерирующие команды ir, сгенерированная команда помещается в стек
*/
#define c_GEP(ptr, index)          DECLARE_FUNC_WITH_2_PARAMS(GEP)(ptr,index)
#define c_ConstGEP1_32(ptr, index) DECLARE_FUNC_WITH_2_PARAMS_I(ConstGEP1_32)(ptr, index)
//#define c_CastPtrInt32(ptr)      DECLARE_FUNC_WITH_2_PARAMS(PointerCast)(ptr,Type::getInt32PtrTy(context))
//#define c_CastPtrFlt32(ptr)      DECLARE_FUNC_WITH_2_PARAMS(PointerCast)(ptr,Type::getFloatPtrTy(context))
#define c_Load(ptr)                DECLARE_FUNC_WITH_1_PARAMS(Load)(ptr)
#define c_Store(data, ptr)         DECLARE_FUNC_WITH_2_PARAMS(Store)(data, ptr); ringBufValue.pop_back()

#define c_Add(op1, op2)            DECLARE_FUNC_WITH_2_PARAMS(Add) (op1, op2)
#define c_Sub(op1, op2)            DECLARE_FUNC_WITH_2_PARAMS(Sub) (op1, op2) 
#define c_Mul(op1, op2)            DECLARE_FUNC_WITH_2_PARAMS(Mul) (op1, op2)
#define c_Div(op1, op2)            DECLARE_FUNC_WITH_2_PARAMS(FDiv)(op1, op2)

#define c_FAdd(op1, op2)            DECLARE_FUNC_WITH_2_PARAMS(FAdd) (op1, op2)
#define c_FSub(op1, op2)            DECLARE_FUNC_WITH_2_PARAMS(FSub) (op1, op2) 
#define c_FMul(op1, op2)            DECLARE_FUNC_WITH_2_PARAMS(FMul) (op1, op2)
#define c_FDiv(op1, op2)            DECLARE_FUNC_WITH_2_PARAMS(FDiv)(op1, op2)

#define c_Not(op1, op2)            DECLARE_FUNC_WITH_1_PARAMS(Not)(op1)
#define c_Cast_i1(op1)             DECLARE_FUNC_WITH_2_PARAMS(ICmpNE)(op1,builder.getInt32(0))
#define c_And(op1, op2)      DECLARE_FUNC_WITH_2_UNION_PARAMS(And)(op1,op2)
#define c_Or (op1, op2)      DECLARE_FUNC_WITH_2_UNION_PARAMS(Or )(op1,op2)

/*
    \brief Макросы, генерирующие команды ir, аргументы команд берутся из стека
*/
#define s_CastPtrInt32() DECLARE_FUNC_WITH_1_PARAMS_STK_1(PointerCast, Type)(Type::getInt32PtrTy(context))
#define s_CastPtrFlt32() DECLARE_FUNC_WITH_1_PARAMS_STK_1(PointerCast, Type)(Type::getFloatPtrTy(context))
#define s_Load()         DECLARE_FUNC_WITH_1_PARAMS_STK(Load)()
#define s_And()          DECLARE_FUNC_WITH_2_UNION_PARAMS_STK(And)()
#define s_Or()           DECLARE_FUNC_WITH_2_UNION_PARAMS_STK(Or)()
#define s_Not()          DECLARE_FUNC_WITH_1_PARAMS_STK_1(ICmpEQ, Value)(builder.getInt32(0))
#define s_Cast_i1()      DECLARE_FUNC_WITH_1_PARAMS_STK_1(ICmpNE, Value)(builder.getInt32(0))


AsmError ASM2LLVMBuilder::parseExternalFunctions(const Command& cmd)
{
    static FunctionType* CallType = FunctionType::get(
        builder.getVoidTy(),
        ArrayRef<Type*>({ builder.getInt8PtrTy() }),
        false
    );
    Value* instr_p = ConstantInt::get(
        builder.getInt64Ty(),
        reinterpret_cast<ui64>(&cmd)
    );

    switch (cmd.code.bits.opCode)
    {
        #define GEN_SWITCH_CASE(func)\
        case CMD_##func:\
        builder.CreateCall(\
            module->getOrInsertFunction("run_"#func, CallType),\
            ArrayRef<Value*>({ instr_p })\
        );\
        return ASM_OK;
        #include "External_functions.inc"
        #undef GEN_SWITCH_CASE
    default:
        break;
    }
    return ASM_ERROR_GEN_LABLE_TABLE;
}

AsmError ASM2LLVMBuilder::parseOperands(const Command& cmd)
{

    bool isFloatPointOperands = cmd.code.bits.opCode >= FPU_ISA_START_CODE;
    if (cmd.code.bits.nOperands > 2)
    {
        logger.push("Error",
            "{Parsing asm commands}: Invalid number of operands. mCode: 0x%X",
            cmd.code.marchCode & 0xFFFF
        );
        return ASM_ERROR_INVALID_OPERANDS_NUMBER;
    }

    for (ui8 i = 0; i < cmd.code.bits.nOperands; i++)
    {
        OperandType opType = getOperandType(cmd, i);
        switch (opType)
        {
        case OPERAND_REGISTER:
            c_ConstGEP1_32(ptr_reg_table, cmd.operand[i].ivalue - 1);
            if (isFloatPointOperands) s_CastPtrFlt32();
            operand_ptr[i] = ringBufValue.back();
            operand[i] = c_Load(operand_ptr[i]);
            break;
        case OPERAND_NUMBER:
            operand[i] = isFloatPointOperands
                ? ConstantFP::get(builder.getFloatTy(), cmd.operand[i].fvalue)
                : ConstantInt::get(builder.getInt32Ty(), cmd.operand[i].ivalue);
            break;
        case OPERAND_MEMORY:
            c_ConstGEP1_32(ptr_memory, cmd.operand[i].ivalue);
            if (isFloatPointOperands) s_CastPtrFlt32();
            else s_CastPtrInt32();
            operand_ptr[i] = ringBufValue.back();
            operand[i] = c_Load(operand_ptr[i]);
            break;
        case OPERAND_MEM_BY_REG:
            c_ConstGEP1_32(ptr_reg_table, cmd.operand[i].ivalue - 1);
            s_Load();
            c_GEP(ptr_memory, ringBufValue.back());
            if (isFloatPointOperands) s_CastPtrFlt32();
            else s_CastPtrInt32();
            operand_ptr[i] = ringBufValue.back();
            operand[i] = c_Load(operand_ptr[i]);
            break;
        default:
            logger.push("Error",
                "{Parsing asm commands}: Invalid type of operands. mCode: 0x%X",
                cmd.code.marchCode & 0xFFFF
            );
            return ASM_ERROR_INVALID_OPERAND_SYNTAX;
            break;
        }
    }
}

AsmError ASM2LLVMBuilder::parseGeneral(const Command& cmd, bool& isEndBBCmd)
{
    Value* ESPRegisterPtr = nullptr;
    Value* ESPRegister = nullptr;
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
    case CMD_FADD:
        c_FAdd(operand[0], operand[1]);
        c_Store(ringBufValue.back(), operand_ptr[0]);
        break;
    case CMD_FSUB:
        c_FSub(operand[0], operand[1]);
        c_Store(ringBufValue.back(), operand_ptr[0]);
        break;
    case CMD_FMUL:
        c_FMul(operand[0], operand[1]);
        c_Store(ringBufValue.back(), operand_ptr[0]);
        break;
    case CMD_FDIV:
        c_FDiv(operand[0], operand[1]);
        c_Store(ringBufValue.back(), operand_ptr[0]);
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
        isEndBBCmd |= 1;
        ESPRegisterPtr = c_ConstGEP1_32(ptr_reg_table, ESP_REG_INDEX);
        ESPRegister = c_Load(ESPRegisterPtr);
        c_Sub(ESPRegister, builder.getInt32(BYTES_IN_REGISTER));
        c_Store(ringBufValue.back(), ESPRegisterPtr);
        builder.CreateRetVoid();
        break;
    case CMD_HLT:
        isEndBBCmd |= 1;
        builder.CreateRetVoid();
        break;
    default:
        break;
    }
    return ASM_OK;
}

AsmError ASM2LLVMBuilder::parseBranches(const Command& cmd, bool& isEndBBCmd)
{
    Value* ESPRegisterPtr = nullptr;
    Value* ESPRegister = nullptr;
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

    isEndBBCmd |= 1;
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
    builder.CreateCondBr(ringBufValue.back(), bbArray[cmd.operand[0].ivalue].bb, bbArray[currBBIndex + 1].bb);
}



AsmError ASM2LLVMBuilder::LLVMPareseCommand(const Command& cmd, bool& isBrhCommand, const BlockInfo& blockInfo)
{
    if (ASM_OK == parseExternalFunctions(cmd))
        return ASM_OK;
    parseOperands(cmd);
    if(!isBrachComand(cmd.code.bits.opCode))
        parseGeneral(cmd, isBrhCommand);
    else
        parseBranches(cmd, isBrhCommand);
    return ASM_OK;
}
