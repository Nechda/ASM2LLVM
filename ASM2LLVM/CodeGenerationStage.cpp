#include "ASM2LLVM.h"
#include "CPU/CPU.h"
#include "CPU/CPUInfo.h"
#include "Asm/Asm.h"

using namespace Assembler;

const ui32 RING_BUFFER_CAPACITY = 8;

static Value* operand[3] = { nullptr, nullptr, nullptr };
static Value* operand_ptr[3] = { nullptr, nullptr, nullptr };
static CircularBuffer<Value*> ringBufValue(RING_BUFFER_CAPACITY);

bool isBranchCommand(const ui8 cmdOpCode);//< Defined in ASM2LLVM.cpp


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
                Disassembler::disasmCommand(m_commandList[j], stdout);
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
    \brief Макросы, генерирующие команды ir, сгенерированная команда помещается в стек
*/
#define c_GEP(ptr, index)          createOnStack<Value*, Value*, Value*  , const Twine&>(IRFuncPtr(GEP)         , ptr, index, "")
#define c_ConstGEP1_32(ptr, index) createOnStack<Value*, Value*, unsigned, const Twine&>(IRFuncPtr(ConstGEP1_32), ptr, index, "")
#define c_CastPtrInt8(ptr)         createOnStack<Value*, Value*, Type*, const Twine&>   (IRFuncPtr(PointerCast) , ptr, Type::getInt8PtrTy(m_context),  "")
#define c_CastPtrInt16(ptr)        createOnStack<Value*, Value*, Type*, const Twine&>   (IRFuncPtr(PointerCast) , ptr, Type::getInt16PtrTy(m_context), "")
#define c_CastPtrInt32(ptr)        createOnStack<Value*, Value*, Type*, const Twine&>   (IRFuncPtr(PointerCast) , ptr, Type::getInt32PtrTy(m_context), "")
#define c_CastPtrFlt32(ptr)        createOnStack<Value*, Value*, Type*, const Twine&>   (IRFuncPtr(PointerCast) , ptr, Type::getFloatPtrTy(m_context), "")
#define c_BitCastToI32(data)       createOnStack<Value*, Value*, Type*, const Twine&>   (IRFuncPtr(BitCast)    , data, Type::getInt32Ty(m_context), "")

#define c_GEPList(type, ptr, indexList)          createOnStack<Value*, Type*, Value*, ArrayRef<Value*>, const Twine&>(IRFuncPtr(GEP), type, ptr, indexList, "")
#define c_ConstGEP2_32(type, ptr, idx0, idx1)    createOnStack<Value*,Type*, Value*, unsigned,unsigned, const Twine&>(IRFuncPtr(ConstGEP2_32), type, ptr, idx0, idx1, "")


#define c_CastI1toI32(op)   createOnStack<Value*, Value*, Type*, bool, const Twine&>(IRFuncPtr(IntCast), op, Type::getInt32Ty(m_context), 1, "") //signed
#define c_CastI32toF(op)    createOnStack<Value*, Instruction::CastOps, Value*, Type*, const Twine&>(IRFuncPtr(Cast), Instruction::SIToFP, op, Type::getFloatTy(m_context), "") //signed



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
#define c_Mod(op1, op2)     createOnStack<Value*, Value*, Value*  , const Twine&>(IRFuncPtr(SRem), op1, op2, "")


#define c_Not(op1)          createOnStack<Value*, Value*, const Twine&, MDNode*     >(IRFuncPtr(Not)   , op1, "")
#define c_And(op1, op2)     createOnStack<Value*, Value*, Value*      , const Twine&>(IRFuncPtr(And)   , op1, op2, "")
#define c_Or(op1, op2)      createOnStack<Value*, Value*, Value*      , const Twine&>(IRFuncPtr(Or)    , op1, op2, "")
#define c_Xor(op1, op2)     createOnStack<Value*, Value*, Value*      , const Twine&>(IRFuncPtr(Xor)    , op1, op2, "")
#define c_And_i64(op1,op2)  createOnStack<Value*, Value*, uint64_t    , const Twine&>(IRFuncPtr(And)   , op1, op2, "")
#define c_Or_i64(op1,op2)   createOnStack<Value*, Value*, uint64_t    , const Twine&>(IRFuncPtr(Or)    , op1, op2, "")
#define c_Cast_i1(op1)      createOnStack<Value*, Value*, Value*      , const Twine&>(IRFuncPtr(ICmpNE), op1, m_builder.getInt32(0), "")
#define c_Not_i1(op1)       createOnStack<Value*, Value*, Value*      , const Twine&>(IRFuncPtr(ICmpEQ), op1, m_builder.getInt32(0), "")


#define c_ICmpEQ(op1, op2)   createOnStack<Value*, Value*, Value*      , const Twine&>(IRFuncPtr(ICmpEQ)   , op1, op2, "") //equal
#define c_ICmpNE(op1, op2)   createOnStack<Value*, Value*, Value*      , const Twine&>(IRFuncPtr(ICmpNE)   , op1, op2, "") //not equal
#define c_ICmpSGT(op1, op2)  createOnStack<Value*, Value*, Value*      , const Twine&>(IRFuncPtr(ICmpSGT)  , op1, op2, "") //greater than
#define c_ICmpSGE(op1, op2)  createOnStack<Value*, Value*, Value*      , const Twine&>(IRFuncPtr(ICmpSGE)  , op1, op2, "") //greater or equal

#define c_FCmpEQ(op1, op2)   createOnStack<Value*, Value*, Value*      , const Twine&, MDNode*>(IRFuncPtr(FCmpOEQ)  , op1, op2, "", nullptr) //equal
#define c_FCmpNE(op1, op2)   createOnStack<Value*, Value*, Value*      , const Twine&, MDNode*>(IRFuncPtr(FCmpONE)  , op1, op2, "", nullptr) //not equal
#define c_FCmpGT(op1, op2)   createOnStack<Value*, Value*, Value*      , const Twine&, MDNode*>(IRFuncPtr(FCmpOGT)  , op1, op2, "", nullptr) //greater than
#define c_FCmpGE(op1, op2)   createOnStack<Value*, Value*, Value*      , const Twine&, MDNode*>(IRFuncPtr(FCmpOGE)  , op1, op2, "", nullptr) //greater or equal
/*
    \brief Макросы, генерирующие команды ir, аргументы команд берутся из стека
*/
#define PARAM getAndPop()

#define s_CastPtrInt8()  c_CastPtrInt8(getAndPop())
#define s_CastPtrInt32() c_CastPtrInt32(getAndPop())
#define s_CastPtrFlt32() c_CastPtrFlt32(getAndPop())
#define s_CastI1toI32()  c_CastI1toI32(getAndPop())
#define s_BitCastToI32() c_BitCastToI32(getAndPop())
#define s_Load()         c_Load(getAndPop())
#define s_Add()          c_Add(getAndPop(), getAndPop())
#define s_FAdd()         c_FAdd(getAndPop(), getAndPop())
#define s_And()          c_And(getAndPop(), getAndPop())
#define s_Or()           c_Or(getAndPop(), getAndPop())
#define s_Xor()          c_Xor(getAndPop(), getAndPop())
#define s_Not()          c_Not_i1(getAndPop())
#define s_Cast_i1()      c_Cast_i1(getAndPop())


#define c_Intrinsic(intr, op)\
    do {\
        Value* intrinsicValue = m_builder.CreateIntrinsic(\
            intr,\
            { Type::getFloatTy(m_context) },\
            { op }\
        );\
        ringBufValue.push_back(intrinsicValue);\
    } while (0)


AsmError Translator::parseExternalFunctions(const Command& cmd)
{
    static FunctionType* CallType = FunctionType::get(
        m_builder.getVoidTy(),
        ArrayRef<Type*>({
            m_builder.getInt16Ty(),
            m_builder.getInt32Ty(),
            m_builder.getInt32Ty(),
            m_builder.getInt32Ty(),
            m_builder.getInt32Ty() 
        }),
        false
    );
    
    Value* instr_code = ConstantInt::get(
        m_builder.getInt16Ty(),
        cmd.bits.marchCode
    );

    Value* instr_op1 = ConstantInt::get(
        m_builder.getInt32Ty(),
        cmd.operand[0].ivalue
    );

    Value* instr_op2 = ConstantInt::get(
        m_builder.getInt32Ty(),
        cmd.operand[1].ivalue
    );

    Value* instr_ex1 = ConstantInt::get(
        m_builder.getInt32Ty(),
        cmd.extend[0]
    );

    Value* instr_ex2 = ConstantInt::get(
        m_builder.getInt32Ty(),
        cmd.extend[1]
    );

    switch (cmd.bits.opCode)
    {
        #define INTERPRETATED_FUNCTION(name,code)\
            case CMD_##name:\
            m_builder.CreateCall(\
                m_module->getOrInsertFunction("run_"#name, CallType),\
                ArrayRef<Value*>({ instr_code, instr_op1, instr_op2, instr_ex1, instr_ex2 })\
            );\
            return ASM_OK;
            #include "InterpretedFunctions.inc"
        #undef INTERPRETATED_FUNCTION
    default:
        break;
    }
    return ASM_ERROR_GEN_LABLE_TABLE;
}


static inline bool isFPOperands(const Command& cmd)
{
    #define IN(value, a, b) a <= value && value <= b

    bool result = IN(cmd.bits.opCode, FPU_ISA_START_CODE, ISIMD_ISA_START_CODE);
    result &= !(
        cmd.bits.opCode == CMD_ABS
        || cmd.bits.opCode == CMD_MOD
        || cmd.bits.opCode == CMD_FILD
        );

    result |= IN(cmd.bits.opCode, CMD_LFADD, CMD_LFMUL)
           || cmd.bits.opCode == CMD_LFDIV || cmd.bits.opCode == CMD_LFACCUM;

    #undef IN
    return result;
}

AsmError Translator::parseOperands(const Command& cmd, const ui8 offsetInReg)
{
    bool isFloatPointOperands = isFPOperands(cmd);

    #define LONG_REGISTER_OFFSET(index) (index - LR0_REG_INDEX - 1) * 4 + LR0_REG_INDEX
    for (ui8 i = 0; i < cmd.bits.nOperands; i++)
    {
        OperandType opType = getOperandType(cmd, i);
        switch (opType)
        {
        case OPERAND_REGISTER:
            if(cmd.operand[i].ivalue - 1 >= LR0_REG_INDEX)
                c_ConstGEP2_32(m_regTableType, m_reg_table, 0, LONG_REGISTER_OFFSET(cmd.operand[i].ivalue) + offsetInReg);
            else
                c_ConstGEP2_32(m_regTableType, m_reg_table, 0, cmd.operand[i].ivalue - 1);
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
            c_ConstGEP2_32(m_memTableType, m_memory, 0, cmd.operand[i].ivalue);
            if (isFloatPointOperands) s_CastPtrFlt32();
            else s_CastPtrInt32();
            operand_ptr[i] = ringBufValue.back();
            operand[i] = c_Load(operand_ptr[i]);
            break;
        case OPERAND_MEM_BY_REG:
            c_ConstGEP2_32(m_regTableType, m_reg_table, 0, cmd.operand[i].ivalue - 1);
            s_Load();
            if (cmd.bits.longCommand)
                c_Add(getAndPop(), m_builder.getInt32(cmd.extend[i]));
            c_GEPList(m_memTableType, m_memory, ArrayRef<Value*>({ m_builder.getInt32(0),ringBufValue.back()} ) );
            if (isFloatPointOperands) s_CastPtrFlt32();
            else s_CastPtrInt32();
            operand_ptr[i] = ringBufValue.back();
            operand[i] = c_Load(operand_ptr[i]);
            break;
        default:
            logger.push("Error",
                "{Parsing asm commands}: Invalid type of operands. mCode: 0x%X",
                cmd.bits.marchCode & 0xFFFF
            );
            return ASM_ERROR_INVALID_OPERAND_SYNTAX;
            break;
        }
    }
    #undef LONG_REGISTER_OFFSET
    return ASM_OK;
}

AsmError Translator::parseGeneral(const Command& cmd, bool& isEndBBCmd)
{
    Value* ESPRegisterPtr = nullptr;
    Value* ESPRegister = nullptr;
    switch (cmd.bits.opCode)
    {
        case CMD_DUMP:
        break;
        case CMD_MOV:
            c_Store(operand[1], operand_ptr[0]);
            break;
        case CMD_MOVB:
            operand[1] = m_builder.CreateCast(Instruction::CastOps::Trunc, operand[1], m_builder.getInt8Ty());
            operand_ptr[0] = c_CastPtrInt8(operand_ptr[0]);
            c_Store(operand[1], operand_ptr[0]);
            break;
        case CMD_MOVW:
            operand[1] = m_builder.CreateCast(Instruction::CastOps::Trunc, operand[1], m_builder.getInt16Ty());
            operand_ptr[0] = c_CastPtrInt16(operand_ptr[0]);
            c_Store(operand[1], operand_ptr[0]);
            break;
        case CMD_ADD:
            c_Add(operand[1], operand[2]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_SUB:
            c_Sub(operand[1], operand[2]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_MUL:
            c_Mul(operand[1], operand[2]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_DIV:
            c_Div(operand[1], operand[2]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_FADD:
            c_FAdd(operand[1], operand[2]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_FSUB:
            c_FSub(operand[1], operand[2]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_FMUL:
            c_FMul(operand[1], operand[2]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_FDIV:
            c_FDiv(operand[1], operand[2]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_PUSH:
            ESPRegisterPtr = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, ESP_REG_INDEX);
            ESPRegister = c_Load(ESPRegisterPtr);
            c_GEPList(m_memTableType, m_memory, ArrayRef<Value*>({ m_builder.getInt32(0), ringBufValue.back() }));
            s_CastPtrInt32();
            c_Store(operand[0], ringBufValue.back());
            c_Add(ESPRegister, m_builder.getInt32(BYTES_IN_REGISTER));
            c_Store(ringBufValue.back(), ESPRegisterPtr);
            break;
        case CMD_POP:
            ESPRegisterPtr = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, ESP_REG_INDEX);
            ESPRegister = c_Load(ESPRegisterPtr);
            c_Sub(ESPRegister, m_builder.getInt32(BYTES_IN_REGISTER));
            c_Store(ringBufValue.back(), ESPRegisterPtr);
            c_GEPList(m_memTableType, m_memory, ArrayRef<Value*>({ m_builder.getInt32(0), ringBufValue.back() }));
            s_CastPtrInt32();
            s_Load();
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_RET:
            isEndBBCmd |= 1;
            ESPRegisterPtr = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, ESP_REG_INDEX);
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
            c_And(operand[1], operand[2]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_OR:
            c_Or(operand[1], operand[2]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_XOR:
            c_Xor(operand[1], operand[2]);
            c_Store(ringBufValue.back(), operand_ptr[0]);
            break;
        case CMD_FSQRT:
            c_Intrinsic(Intrinsic::sqrt, operand[0]);
            c_Store(getAndPop(), operand_ptr[0]);
            break;
        case CMD_FSIN:
            c_Intrinsic(Intrinsic::sin, operand[0]);
            c_Store(getAndPop(), operand_ptr[0]);
            break;
        case CMD_FCOS:
            c_Intrinsic(Intrinsic::cos, operand[0]);
            c_Store(getAndPop(), operand_ptr[0]);
            break;
        case CMD_FABS:
            c_Intrinsic(Intrinsic::fabs, operand[0]);
            c_Store(getAndPop(), operand_ptr[0]);
            break;
        case CMD_FPOW:
            {
                Value* intrinsicValue = m_builder.CreateIntrinsic(
                    Intrinsic::pow,
                    { Type::getFloatTy(m_context)}, 
                    { operand[0], operand[1] }
                ); 
                c_Store(intrinsicValue, operand_ptr[0]);
            }
            break;
        case CMD_FISTP:
            {
                Value* intrinsicValue = m_builder.CreateIntrinsic(
                    Intrinsic::lrint,
                    { Type::getInt32Ty(m_context), Type::getFloatTy(m_context) },
                    { operand[0] }
                );
                c_CastPtrInt32(operand_ptr[0]);
                c_Store(intrinsicValue, getAndPop());
            }
            break;
        case CMD_FILD:
            operand_ptr[0] = c_CastPtrFlt32(operand_ptr[0]);
            c_CastI32toF(operand[0]);
            c_Store(getAndPop(), operand_ptr[0]);
            break;
        case CMD_MOD:
            c_Mod(operand[0], operand[1]);
            c_Store(getAndPop(), operand_ptr[0]);
            break;
        case CMD_PUSHA:
            for (ui8 i = 0; i < 4; i++)
            {
                operand_ptr[0] = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, i);
                operand[0] = c_Load(operand_ptr[0]);
                ESPRegisterPtr = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, ESP_REG_INDEX);
                ESPRegister = c_Load(ESPRegisterPtr);
                c_GEPList(m_memTableType, m_memory, ArrayRef<Value*>({ m_builder.getInt32(0), ringBufValue.back() }));
                s_CastPtrInt32();
                c_Store(operand[0], ringBufValue.back());
                c_Add(ESPRegister, m_builder.getInt32(BYTES_IN_REGISTER));
                c_Store(ringBufValue.back(), ESPRegisterPtr);
            }
            break;
        case CMD_POPA:
            for (i8 i = 3; i > 0; i--)
            {
                operand_ptr[0] = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, i);
                operand[0] = c_Load(operand_ptr[0]);
                ESPRegisterPtr = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, ESP_REG_INDEX);
                ESPRegister = c_Load(ESPRegisterPtr);
                c_Sub(ESPRegister, m_builder.getInt32(BYTES_IN_REGISTER));
                c_Store(ringBufValue.back(), ESPRegisterPtr);
                c_GEPList(m_memTableType, m_memory, ArrayRef<Value*>({ m_builder.getInt32(0), ringBufValue.back() }));
                s_CastPtrInt32();
                s_Load();
                c_Store(ringBufValue.back(), operand_ptr[0]);
            }
            break;
        case CMD_LLZA:
            for (ui16 i = LR0_REG_INDEX; i < 8 * 4; i ++)
            {
                operand_ptr[0] = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, i);
                c_Store(m_builder.getInt32(0), operand_ptr[0]);
            }

        default:
            logger.push("Error",
                "{Parsing asm commands}: The command doesn't support. mCode: 0x%X",
                cmd.bits.marchCode & 0xFFFF
            );
            Disassembler::disasmCommand(cmd, logger.getStream());
            return ASM_ERROR_INVALID_OPERAND_SYNTAX;
            break;
    }
    return ASM_OK;
}

AsmError Translator::parseBranches(const Command& cmd, bool& isEndBBCmd)
{
    Value* ESPRegisterPtr = nullptr;
    Value* ESPRegister = nullptr;
    if (cmd.bits.opCode == CMD_CALL)
    {
        ui32 funcIndex = m_bbArray[cmd.operand[0].ivalue].funcIndex;

        ESPRegisterPtr = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, ESP_REG_INDEX);
        ESPRegister = c_Load(ESPRegisterPtr);
        c_Add(ESPRegister, m_builder.getInt32(BYTES_IN_REGISTER));
        c_Store(ringBufValue.back(), ESPRegisterPtr);

        m_builder.CreateCall(m_funcArray[funcIndex].first);
        return ASM_OK;
    }

    isEndBBCmd |= 1;
    if (cmd.bits.opCode == CMD_JMP)
    {
        m_builder.CreateBr(m_bbArray[cmd.operand[0].ivalue].bb);
        return ASM_OK;
    }


    switch (cmd.bits.opCode)
    {
        case CMD_JE:   c_ICmpEQ( operand[0], operand[1]);  break;
        case CMD_JNE:  c_ICmpNE( operand[0], operand[1]);  break;
        case CMD_JA:   c_ICmpSGT(operand[0], operand[1]);  break;
        case CMD_JAE:  c_ICmpSGE(operand[0], operand[1]);  break;
        case CMD_FJE:  c_FCmpEQ( operand[0], operand[1]);  break;
        case CMD_FJNE: c_FCmpNE( operand[0], operand[1]);  break;
        case CMD_FJA:  c_FCmpGT( operand[0], operand[1]);  break;
        case CMD_FJAE: c_FCmpGE( operand[0], operand[1]);  break;
        default:
            logger.push("Error",
                "{Parsing asm commands}: The command doesn't support. mCode: 0x%X",
                cmd.bits.marchCode & 0xFFFF
            );
            Disassembler::disasmCommand(cmd, logger.getStream());
            return ASM_ERROR_INVALID_OPERAND_SYNTAX;
            break;
    }


    m_builder.CreateCondBr(
        ringBufValue.back(),
        m_bbArray[cmd.operand[2].ivalue].bb,
        m_bbArray[m_currBBIndex + 1].bb
    );
    return ASM_OK;
}


//=======================================================================


//                          SIMD command translator


//=======================================================================


AsmError Translator::simd_parseCommand(const Command& cmd)
{
#define LLVM_INT8_PTR m_builder.getInt8PtrTy()
#define LLVM_INT1 m_builder.getInt1Ty()
#define LLVM_INT8 m_builder.getInt8Ty()
#define LLVM_INT32 m_builder.getInt32Ty()

    //arithmetic operations
    if (CMD_LADD <= cmd.bits.opCode && cmd.bits.opCode <= CMD_LFDIV)
    {
        for (ui8 i = 0; i < 4; i++)
        {
            parseOperands(cmd, i);
            switch (cmd.bits.opCode)
            {
                case CMD_LADD:  c_Add( operand[1], operand[2]); break;
                case CMD_LSUB:  c_Sub( operand[1], operand[2]); break;
                case CMD_LMUL:  c_Mul( operand[1], operand[2]); break;
                case CMD_LDIV:  c_Div( operand[1], operand[2]); break;
                case CMD_LFADD: c_FAdd(operand[1], operand[2]); break;
                case CMD_LFSUB: c_FSub(operand[1], operand[2]); break;
                case CMD_LFMUL: c_FMul(operand[1], operand[2]); break;
                case CMD_LFDIV: c_FDiv(operand[1], operand[2]); break;
                default: return ASM_ERROR_INVALID_MACHINE_CODE; break;
            }
            c_Store(getAndPop(), operand_ptr[0]);
        }
        return ASM_OK;
    }


    union
    {
        struct
        {
            ui8 r3 : 2;
            ui8 r2 : 2;
            ui8 r1 : 2;
            ui8 r0 : 2;
        };
        ui32 mask;
    }info;

    #define LONG_REGISTER_OFFSET(index) (index - LR0_REG_INDEX - 1) * 4 + LR0_REG_INDEX

    Command tcmd = cmd;
    switch (cmd.bits.opCode)
    {
        case CMD_LLZA:
            // memset(&Register.lr0, 0, 8 * 4 * sizeof(ui32)); (not volatile)
            c_ConstGEP2_32(m_regTableType, m_reg_table, 0, LR0_REG_INDEX);
            operand_ptr[0] = s_CastPtrInt8();
            m_builder.CreateIntrinsic(
                Intrinsic::memset, { LLVM_INT8_PTR, LLVM_INT32 },
                { operand_ptr[0] , m_builder.getInt8(0), m_builder.getInt32(8 * 4 * BYTES_IN_REGISTER), m_builder.getInt1(0) }
            );
        break;
        case CMD_LLZ:
            // memset(&Register.lr_i, 0, 1 * 4 * sizeof(ui32)); (not volatile)
            c_ConstGEP2_32(m_regTableType, m_reg_table, 0, LONG_REGISTER_OFFSET(cmd.operand[0].ivalue));
            operand_ptr[0] = s_CastPtrInt8();
            m_builder.CreateIntrinsic(
                Intrinsic::memset, { LLVM_INT8_PTR, LLVM_INT32 },
                { operand_ptr[0] , m_builder.getInt8(0), m_builder.getInt32(1 * 4 * BYTES_IN_REGISTER), m_builder.getInt1(0) }
            );
        break;
        case CMD_LLOAD:
            // storing data into long register from memory
            tcmd = cmd;
            for (ui8 i = 0; i < 4; i++)
            {
                parseOperands(tcmd, i); //parse operands from temp command structure
                c_Store(operand[1], operand_ptr[0]);
                tcmd.operand[0].ivalue++; //increasing ptr on register
                if (getOperandType(cmd, 1) == OPERAND_MEMORY) // and offset in memory
                    tcmd.operand[1].ivalue += 4 * cmd.operand[2].ivalue;
                else
                    tcmd.extend[1] += 4 * cmd.operand[2].ivalue;
            }
        break;
        case CMD_LMOV:
            tcmd = cmd;
            tcmd.bits.longCommand = 1;
            for (ui8 i = 0; i < cmd.operand[2].ivalue; i++)
            {
                parseOperands(tcmd, i); //parse operands from temp command structure
                c_Store(operand[1], operand_ptr[0]);
                for(ui8 j = 0; j < 2; j++)
                switch (getOperandType(tcmd, j))
                {
                    case OPERAND_MEMORY:     tcmd.operand[j].ivalue += 4; break;
                    case OPERAND_MEM_BY_REG: tcmd.extend[j] += 4;         break;
                    default:break;
                }
            }
        break;
        case CMD_LSHRINK:
            info.mask = cmd.operand[1].ivalue;
            #define SHRINK_DEFINE(index)\
                operand_ptr[1] = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, info.r##index);\
                operand_ptr[0] = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, LONG_REGISTER_OFFSET(cmd.operand[0].ivalue) + index);\
                s_Load();\
                c_Store(getAndPop(), operand_ptr[1]);
            SHRINK_DEFINE(0);
            SHRINK_DEFINE(1);
            SHRINK_DEFINE(2);
            SHRINK_DEFINE(3);
            #undef SHRINK_DEFINE
        break;
        case CMD_LEXPAND:
            info.mask = cmd.operand[1].ivalue;
            #define EXPAND_DEFINE(index)\
                operand_ptr[0] = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, LONG_REGISTER_OFFSET(cmd.operand[0].ivalue) + index);\
                operand_ptr[1] = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, info.r##index);\
                s_Load();\
                c_Store(getAndPop(), operand_ptr[0]);
            EXPAND_DEFINE(0);
            EXPAND_DEFINE(1);
            EXPAND_DEFINE(2);
            EXPAND_DEFINE(3);
            #undef EXPAND_DEFINE
        break;
        case CMD_LSHRINKA:
            for (ui8 i = 0; i < 4; i++)
            {
                operand_ptr[0] = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, LONG_REGISTER_OFFSET(cmd.operand[0].ivalue) + i);
                operand_ptr[1] = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, i);
                c_Load(operand_ptr[0]);
                c_Store(getAndPop(), operand_ptr[1]);
            }
        break;
        case CMD_LEXPANDA:
            for (ui8 i = 0; i < 4; i++)
            {
                operand_ptr[0] = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, LONG_REGISTER_OFFSET(cmd.operand[0].ivalue) + i);
                operand_ptr[1] = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, i);
                c_Load(operand_ptr[1]);
                c_Store(getAndPop(), operand_ptr[0]);
            }
        break;
        case CMD_LACCUM:
            parseOperands(cmd);
            for (ui8 i = 0; i < 4; i++)
            {
                operand_ptr[1] = c_ConstGEP2_32(m_regTableType, m_reg_table, 0, LONG_REGISTER_OFFSET(cmd.operand[1].ivalue) + i);
                s_Load();
            }
            s_Add(); s_Add(); s_Add();
            c_Store(getAndPop(), operand_ptr[0]);
        break;
        case CMD_LFACCUM:
            parseOperands(cmd);
            for (ui8 i = 0; i < 4; i++)
            {
                c_ConstGEP2_32(m_regTableType, m_reg_table, 0, LONG_REGISTER_OFFSET(cmd.operand[1].ivalue) + i);
                s_CastPtrFlt32();
                s_Load();
            }
            s_FAdd(); s_FAdd(); s_FAdd();
            c_Store(getAndPop(), operand_ptr[0]);
        break;
        default:
            logger.push("Error", "{Parsing asm commands}: undefined module for command 0x%X",
                cmd.bits.marchCode & 0xFFFF);
            return ASM_ERROR_INVALID_MACHINE_CODE;
        break;
    }
    #undef LONG_REGISTER_OFFSET

    return ASM_OK;
}




enum CommandModule
{
    STANDART_MODULE,
    SIMD_MODULE,
    ERROR_MODULE
};


static inline CommandModule getModuleName(const Command& cmd)
{
    #define IN(value, a, b) a <= value && value <= b
    if (IN(cmd.bits.opCode, 0, 43)) // from `mov` to `popa`
        return STANDART_MODULE;
    if (IN(cmd.bits.opCode, 44, 61)) // from `llza` to `lfaccum`
        return SIMD_MODULE;
    #undef IN
    return ERROR_MODULE;
}

AsmError Translator::LLVMPareseCommand(const Command& cmd, bool& isEndBBCmd, const BlockInfo& blockInfo)
{
    AsmError errorCode = ASM_OK;

    errorCode = parseExternalFunctions(cmd);
    if (ASM_OK == errorCode) return ASM_OK;


    CommandModule moduleType = getModuleName(cmd);
    switch (moduleType)
    {
        case STANDART_MODULE:
            errorCode = parseOperands(cmd);
            if (ASM_OK != errorCode) return errorCode;

            if (!isBranchCommand(cmd.bits.opCode))
                errorCode = parseGeneral(cmd, isEndBBCmd);
            else
                errorCode = parseBranches(cmd, isEndBBCmd);
            return errorCode;
        break;
        case SIMD_MODULE:
            errorCode = simd_parseCommand(cmd);
        break;
        default:
            logger.push("Error", "{Parsing asm commands}: undefined module for command 0x%X",
                cmd.bits.marchCode & 0xFFFF);
    }
}

#undef IRFuncPtr