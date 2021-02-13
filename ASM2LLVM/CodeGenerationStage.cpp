#include "ASM2LLVM.h"
#include "CPU/CPU.h"
#include "Asm/Asm.h"

using namespace Assembler;

const ui32 RING_BUFFER_CAPACITY = 8;

static Value* operand[2] = { nullptr, nullptr };
static Value* operand_ptr[2] = { nullptr, nullptr };
static circular_buffer<Value*> ringBufValue(RING_BUFFER_CAPACITY);

static Value* getAndPop()
{
    if (ringBufValue.empty())
    {
        logger.push("Error", "{Code generation}: trying pop value* from empty buffer");
        return nullptr;
    }
    Value* res = ringBufValue.back();
    ringBufValue.pop_back();
    return res;
}


TranslatorError Translator::codeGenerationStage()
{
    bool isErrorOccur = 0;
    isErrorOccur |= m_bbArray.size() <= 1;


    bool isLastCmdSeparating = 0;
    for (ui32 i = 0; i < m_bbArray.size() - 1 && !isErrorOccur; i++)
    {
        //parse commands for each block
        BlockInfo& bbInfo = m_bbArray[i];
        isLastCmdSeparating = 0;
        m_builder.SetInsertPoint(bbInfo.bb);
        m_currBBIndex = i;
        for (ui32 j = bbInfo.sLine; j <= bbInfo.eLine && !isLastCmdSeparating && !isErrorOccur; j++)
        {
            #ifdef LLVM_PRINT_DISASSEMBLER
                m_disasembler.disasmCommand(m_commandList[j], stdout);
            #endif
            isErrorOccur |=
                LLVMPareseCommand(m_commandList[j], isLastCmdSeparating, bbInfo) != ASM_OK;
        }

        if (
            !isLastCmdSeparating
            && m_bbArray[i].funcIndex != m_bbArray[i + 1].funcIndex
            && i != m_bbArray.size() - 2
        )
        {
            logger.push("Error", "{Code generation}: "
                "Two consecutive blocks "
                "belong to different functions "
                "and are not separated by a "
                "branch operator");
            return TR_ERROR_CODE_GENERATION;
        }

        if (!isLastCmdSeparating && i == m_bbArray.size() - 2)
            m_builder.CreateRetVoid();
        else if (!isLastCmdSeparating)
            m_builder.CreateBr(m_bbArray[i+1].bb);
    }
    return isErrorOccur ? TR_ERROR_CODE_GENERATION : TR_OK;
}


#define IRFuncPtr(f) &IRBuilder<>::Create##f

/*
    \brief макросы, генерирующие команды ir, сгенерированная команда помещается в стек
*/
#define c_GEP(ptr, index)          createOnStack<Value*, Value*, Value*  , const Twine&>(IRFuncPtr(GEP)         , ptr, index, "")
#define c_ConstGEP1_32(ptr, index) createOnStack<Value*, Value*, unsigned, const Twine&>(IRFuncPtr(ConstGEP1_32), ptr, index, "")
#define c_CastPtrInt32(ptr)        createOnStack<Value*, Value*, Type*, const Twine&>   (IRFuncPtr(PointerCast) , ptr, Type::getInt32PtrTy(m_context), "")
#define c_CastPtrFlt32(ptr)        createOnStack<Value*, Value*, Type*, const Twine&>   (IRFuncPtr(PointerCast) , ptr, Type::getFloatPtrTy(m_context), "")

#define c_GEPList(type, ptr, indexList)          createOnStack<Value*, Type*, Value*, ArrayRef<Value*>, const Twine&>(IRFuncPtr(GEP), type, ptr, indexList, "")
#define c_ConstGEP2_32(type, ptr, idx0, idx1)    createOnStack<Value*,Type*, Value*, unsigned,unsigned, const Twine&>(IRFuncPtr(ConstGEP2_32), type, ptr, idx0, idx1, "")


#define c_CastI1toI32(op)   createOnStack<Value*, Value*, Type*, bool, const Twine&>(IRFuncPtr(IntCast), op, Type::getInt32Ty(m_context), 1, "") //signed

#define c_Load(ptr)         createOnStack<LoadInst* , Value*, const Twine&>(IRFuncPtr(Load ), ptr , ""    )
#define c_Store(data, ptr)  createOnStack<StoreInst*,Value*, Value* , bool>(IRFuncPtr(Store), data, ptr, 0); ringBufValue.pop_back()

#define c_FAdd(op1, op2)    createOnStack<Value*, Value*, Value*  , const Twine&, MDNode*>(IRFuncPtr(FAdd), op1, op2, "", nullptr)
#define c_FSub(op1, op2)    createOnStack<Value*, Value*, Value*  , const Twine&, MDNode*>(IRFuncPtr(FSub), op1, op2, "", nullptr)
#define c_FMul(op1, op2)    createOnStack<Value*, Value*, Value*  , const Twine&, MDNode*>(IRFuncPtr(FMul), op1, op2, "", nullptr)
#define c_FDiv(op1, op2)    createOnStack<Value*, Value*, Value*  , const Twine&, MDNode*>(IRFuncPtr(FDiv), op1, op2, "", nullptr)

#define c_Add(op1, op2)     createOnStack<Value*, Value*, Value*  , const Twine&, bool, bool>(IRFuncPtr(Add), op1, op2, "", 0, 0)
#define c_Sub(op1, op2)     createOnStack<Value*, Value*, Value*  , const Twine&, bool, bool>(IRFuncPtr(Sub), op1, op2, "", 0, 0)
#define c_Mul(op1, op2)     createOnStack<Value*, Value*, Value*  , const Twine&, bool, bool>(IRFuncPtr(Mul), op1, op2, "", 0, 0)
#define c_Div(op1, op2)     c_FDiv(op1, op2)

#define c_Not(op1)          createOnStack<Value*, Value*, const Twine&, MDNode*     >(IRFuncPtr(Not)   , op1, "")
#define c_And(op1, op2)     createOnStack<Value*, Value*, Value*      , const Twine&>(IRFuncPtr(And)   , op1, op2, "")
#define c_Or(op1, op2)      createOnStack<Value*, Value*, Value*      , const Twine&>(IRFuncPtr(Or)    , op1, op2, "")
#define c_And_i64(op1,op2)  createOnStack<Value*, Value*, uint64_t    , const Twine&>(IRFuncPtr(And)   , op1, op2, "")
#define c_Or_i64(op1,op2)   createOnStack<Value*, Value*, uint64_t    , const Twine&>(IRFuncPtr(Or)    , op1, op2, "")
#define c_Cast_i1(op1)      createOnStack<Value*, Value*, Value*      , const Twine&>(IRFuncPtr(ICmpNE), op1, m_builder.getInt32(0), "")
#define c_Not_i1(op1)       createOnStack<Value*, Value*, Value*      , const Twine&>(IRFuncPtr(ICmpEQ), op1, m_builder.getInt32(0), "")

/*
    \brief макросы, генерирующие команды ir, аргументы команд берутся из стека
*/
#define s_CastPtrInt32() c_CastPtrInt32(getAndPop())
#define s_CastPtrFlt32() c_CastPtrFlt32(getAndPop())
#define s_CastI1toI32()  c_CastI1toI32(getAndPop())
#define s_Load()         c_Load(getAndPop())
#define s_And()          c_And(getAndPop(),getAndPop())
#define s_Or()           c_Or(getAndPop(),getAndPop())
#define s_Not()          c_Not_i1(getAndPop())
#define s_Cast_i1()      c_Cast_i1(getAndPop())

AsmError Translator::parseExternalFunctions(const Command& cmd)
{
    static FunctionType* CallType = FunctionType::get(
        m_builder.getVoidTy(),
        ArrayRef<Type*>({ m_builder.getInt64Ty()}),
        false
    );
    Value* instr_p = ConstantInt::get(
        m_builder.getInt64Ty(),
        reinterpret_cast<ui64>(&cmd)
    );
    


    switch (cmd.code.bits.opCode)
    {
        #define GEN_SWITCH_CASE(func)\
        case CMD_##func:\
        m_builder.CreateCall(\
            m_module->getOrInsertFunction("run_"#func, CallType),\
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

AsmError Translator::parseOperands(const Command& cmd)
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
            c_ConstGEP2_32(regTableType, m_reg_table, 0, cmd.operand[i].ivalue - 1);
            if (isFloatPointOperands) s_CastPtrFlt32();
            operand_ptr[i] = ringBufValue.back();
            operand[i] = c_Load(operand_ptr[i]);
            break;
        case OPERAND_NUMBER:
            operand[i] = isFloatPointOperands
                ? ConstantFP::get(m_builder.getFloatTy(), cmd.operand[i].fvalue)
                : ConstantInt::get(m_builder.getInt32Ty(), cmd.operand[i].ivalue);
            break;
        case OPERAND_MEMORY:
            c_ConstGEP2_32(memTableType, m_memory, 0, cmd.operand[i].ivalue);
            if (isFloatPointOperands) s_CastPtrFlt32();
            else s_CastPtrInt32();
            operand_ptr[i] = ringBufValue.back();
            operand[i] = c_Load(operand_ptr[i]);
            break;
        case OPERAND_MEM_BY_REG:
            c_ConstGEP2_32(regTableType, m_reg_table, 0, cmd.operand[i].ivalue - 1);
            s_Load();
            c_GEPList(memTableType, m_memory, ArrayRef<Value*>({ m_builder.getInt32(0),ringBufValue.back()} ) );
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
    return ASM_OK;
}

AsmError Translator::parseGeneral(const Command& cmd, bool& isEndBBCmd)
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
        ESPRegisterPtr = c_ConstGEP2_32(regTableType, m_reg_table, 0, ESP_REG_INDEX);
        ESPRegister = c_Load(ESPRegisterPtr);
        c_GEPList(memTableType, m_memory, ArrayRef<Value*>({ m_builder.getInt32(0), ringBufValue.back() }));
        s_CastPtrInt32();
        c_Store(operand[0], ringBufValue.back());
        c_Add(ESPRegister, m_builder.getInt32(BYTES_IN_REGISTER));
        c_Store(ringBufValue.back(), ESPRegisterPtr);
        break;
    case CMD_POP:
        ESPRegisterPtr = c_ConstGEP2_32(regTableType, m_reg_table, 0, ESP_REG_INDEX);
        ESPRegister = c_Load(ESPRegisterPtr);
        c_Sub(ESPRegister, m_builder.getInt32(BYTES_IN_REGISTER));
        c_Store(ringBufValue.back(), ESPRegisterPtr);
        c_GEPList(memTableType, m_memory, ArrayRef<Value*>({ m_builder.getInt32(0), ringBufValue.back() }));
        s_CastPtrInt32();
        s_Load();
        c_Store(ringBufValue.back(), operand_ptr[0]);
        break;
    case CMD_RET:
        isEndBBCmd |= 1;
        ESPRegisterPtr = c_ConstGEP2_32(regTableType, m_reg_table, 0, ESP_REG_INDEX);
        ESPRegister = c_Load(ESPRegisterPtr);
        c_Sub(ESPRegister, m_builder.getInt32(BYTES_IN_REGISTER));
        c_Store(ringBufValue.back(), ESPRegisterPtr);
        m_builder.CreateRetVoid();
        break;
    case CMD_HLT:
        isEndBBCmd |= 1;
        m_builder.CreateRetVoid();
        break;
    case CMD_AND:
        c_Cast_i1(operand[0]);
        c_Cast_i1(operand[1]);
        s_And();
        s_CastI1toI32();
        c_Store(ringBufValue.back(), operand_ptr[0]);
        break;
    case CMD_OR:
        c_Cast_i1(operand[0]);
        c_Cast_i1(operand[1]);
        s_Or();
        s_CastI1toI32();
        c_Store(ringBufValue.back(), operand_ptr[0]);
        break;
    default:
        logger.push("Error",
            "{Parsing asm commands}: The command doesn't support. mCode: 0x%X",
            cmd.code.marchCode & 0xFFFF
        );
        m_disasembler.disasmCommand(cmd, logger.getStream());
        return ASM_ERROR_INVALID_OPERAND_SYNTAX;
        break;
    }
    return ASM_OK;
}

AsmError Translator::parseBranches(const Command& cmd, bool& isEndBBCmd)
{
    Value* ESPRegisterPtr = nullptr;
    Value* ESPRegister = nullptr;
    if (cmd.code.bits.opCode == CMD_CALL)
    {
        ui32 funcIndex = m_bbArray[cmd.operand[0].ivalue].funcIndex;

        ESPRegisterPtr = c_ConstGEP2_32(regTableType, m_reg_table, 0, ESP_REG_INDEX);
        ESPRegister = c_Load(ESPRegisterPtr);
        c_Add(ESPRegister, m_builder.getInt32(BYTES_IN_REGISTER));
        c_Store(ringBufValue.back(), ESPRegisterPtr);

        m_builder.CreateCall(m_funcArray[funcIndex].first);
        return ASM_OK;
    }

    isEndBBCmd |= 1;
    if (cmd.code.bits.opCode == CMD_JMP)
    {
        m_builder.CreateBr(m_bbArray[cmd.operand[0].ivalue].bb);
        return ASM_OK;
    }

    c_ConstGEP2_32(regTableType, m_reg_table, 0, EFL_REG_INDEX);
    Value* eflRegister = s_Load();
    switch (cmd.code.bits.opCode)
    {
    case CMD_JE:
        c_And(eflRegister, m_builder.getInt32(1 << FLAG_ZF));
        s_Cast_i1();
        break;
    case CMD_JNE:
        c_And(eflRegister, m_builder.getInt32(1 << FLAG_ZF));
        s_Not();
        break;
    case CMD_JA:
        c_And(eflRegister, m_builder.getInt32(1 << FLAG_CF));
        s_Not();
        c_And(eflRegister, m_builder.getInt32(1 << FLAG_ZF));
        s_Not();
        s_And();
        break;
    case CMD_JAE:
        c_And(eflRegister, m_builder.getInt32(1 << FLAG_CF));
        s_Not();
        break;
    case CMD_JB:
        c_And(eflRegister, m_builder.getInt32(1 << FLAG_CF));
        s_Cast_i1();
        break;
    case CMD_JBE:
        c_And(eflRegister, m_builder.getInt32(1 << FLAG_CF));
        c_And(eflRegister, m_builder.getInt32(1 << FLAG_ZF));
        s_Or();
        s_Cast_i1();
        break;
    default:
        logger.push("Error",
            "{Parsing asm commands}: The command doesn't support. mCode: 0x%X",
            cmd.code.marchCode & 0xFFFF
        );
        m_disasembler.disasmCommand(cmd, logger.getStream());
        return ASM_ERROR_INVALID_OPERAND_SYNTAX;
        break;
    }
    m_builder.CreateCondBr(ringBufValue.back(), m_bbArray[cmd.operand[0].ivalue].bb, m_bbArray[m_currBBIndex + 1].bb);
    return ASM_OK;
}

AsmError Translator::LLVMPareseCommand(const Command& cmd, bool& isEndBBCmd, const BlockInfo& blockInfo)
{
    AsmError errorCode = ASM_OK;

    errorCode = parseExternalFunctions(cmd);
    if (ASM_OK == errorCode) return ASM_OK;

    errorCode = parseOperands(cmd);
    if (ASM_OK != errorCode) return errorCode;

    if(!isBranchCommand(cmd.code.bits.opCode))
        errorCode = parseGeneral(cmd, isEndBBCmd);
    else
        errorCode = parseBranches(cmd, isEndBBCmd);
    return errorCode;
}

#undef IRFuncPtr