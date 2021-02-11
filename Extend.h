/**
\file    В данном фалй описываются команды языка ассмеблера, а так же их реализации применительно
         к эмуляции на виртуальном процессоре.
\details Для задания новой команды требудется вызвать макрос, прототип которого имеет вид:
         #define DEF(name, mCode, vStr1, vStr2, code)
         name --- имя команды, указывается ЗАГЛАВНЫМИ буквами
         mCode --- машинный код команды, правило формирования кода указано ниже
         vStr1 --- строка допустимых типов первого операнда, правило формирование смотри ниже
         vStr2 --- строка допустимых типов второго операнда, правило формирование смотри ниже
         code --- код функции, реализующий действие команды на процессоре

         Рассмотрим пример раскрытия макроса как функции:
         DEF(
             JMP,
             8 << 8 | 0 << 6 | 0 << 4 | 0 << 2 | 0x1, "N", "",
             {
                myCPU.Register.eip = myCPU.Register.ecs + (int)cmd.operand[0];
             }
         )
         будет создавать функцию, которая описывается следующим кодом:
         void run_JMP(Command cmd)
         {
            myCPU.Register.eip = myCPU.Register.ecs + (int)cmd.operand[0];
         }


         Структура кодирования команд в машинном коде:
         bytes:            8                    2             2                       2                    2
         description: command general code | reseved |type of second operand | type of first operand  | nOperands

         Кодирование типов операндов в машинном коде:
         0b00 --- operand is register
         0b01 --- operand is number
         0b10 --- operand is memory, based by number
         0b11 --- operand is memory, based by register


         Типы символов в строках vSrt:
         R --- operand is Register
         N --- operand is Number
         M --- operand is Memory, based by number
         B --- operand is memory, Based by register
         Пример:
         Для команды mov строка vSrt будет равна "RMB" для первого операнда и "RNMB" для второго операнда.
         Такое кодирование означает, что команда mov не может записать данные в число.

*/

#define make_code(start, offset, nOperands)\
    (start + offset) << 8 | 0 << 6 | 0 << 4 | 0 << 2 | (nOperands) 
#define isInterruptOccur() if(myCPU.interruptCode) return;
#define isFiniteOperands()\
{\
    if (dst && !myCPU.interruptCode) myCPU.interruptCode |= !std::isfinite(dst->fvalue);\
    if (src && !myCPU.interruptCode) myCPU.interruptCode |= !std::isfinite(src->fvalue);\
 }

DEF(
    HLT,
    make_code(0,0,0), "", "",
    {}
)

DEF(
    MOV,
    make_code(0,1,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        *dst = *src;
    }
)

DEF(
    ADD,
    make_code(0,2,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        dst->ivalue += src->ivalue;
    }
)

DEF(
    SUB,
    make_code(0,3,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        dst->ivalue -= src->ivalue;
    }
)

DEF(
    DIV,
    make_code(0,4,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        if (src->ivalue == 0)
            myCPU.interruptCode = 1;
        else
            dst->ivalue /= src->ivalue;
    }
)

DEF(
    MUL,
    make_code(0,5,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        dst->ivalue *= src->ivalue;
    }
)

DEF(
    POP,
    make_code(0,6,1), "RMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        ui8* data = (ui8*)dst;
        for (ui8 i = 0; i < sizeof(ui32); i++)
            stackPop(&myCPU.stack, &data[sizeof(ui32) - 1 - i]);

        //в данной реализации процессора в качестве стека используется immortal stack,
        //соттветственно при push он растет в сторону больших адресов,
        //а при pop адрес вершины уменьшается
        myCPU.Register.esp -= sizeof(ui32);
    }
)

DEF(
    PUSH,
    make_code(0,7,1), "RNMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        ui8* data = (ui8*)dst;
        for (ui8 i = 0; i < sizeof(ui32); i++)
            stackPush(&myCPU.stack, &data[i]);

        //в данной реализации процессора в качестве стека используется immortal stack,
        //соттветственно при push он растет в сторону больших адресов,
        //а при pop адрес вершины уменьшается
        myCPU.Register.esp += sizeof(ui32);
    }
)

DEF(
    JMP,
    make_code(0,8,1), "N", "",
    {
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)


DEF(
    CMP,
    make_code(0,9,2), "RNMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        OperandUnion result;
        result.ivalue = dst->ivalue - src->ivalue;

        setBit(&myCPU.Register.efl, FLAG_CF, result.ivalue >> (sizeof(ui32) * 8 - 1));
        setBit(&myCPU.Register.efl, FLAG_ZF, result.ivalue == 0 ? 1 : 0);
        setBit(&myCPU.Register.efl, FLAG_SF, result.ivalue >= 0 ? 0 : 1);
    }
)

DEF(
    JE,
    make_code(0,10,1), "N", "",
    {
        if (getBit(myCPU.Register.efl, FLAG_ZF))
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)

DEF(
    JNE,
    make_code(0,11,1), "N", "",
    {
        if (!getBit(myCPU.Register.efl, FLAG_ZF))
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)

DEF(
    JA,
    make_code(0,12,1), "N", "",
    {
        if (!getBit(myCPU.Register.efl, FLAG_CF) && !getBit(myCPU.Register.efl, FLAG_ZF))
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)

DEF(
    JAE,
    make_code(0,13,1), "N", "",
    {
        if (!getBit(myCPU.Register.efl, FLAG_CF))
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)

DEF(
    JB,
    make_code(0,14,1), "N", "",
    {
        if (getBit(myCPU.Register.efl, FLAG_CF))
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)

DEF(
    JBE,
    make_code(0,15,1), "N", "",
    {
        if (getBit(myCPU.Register.efl, FLAG_CF) || getBit(myCPU.Register.efl, FLAG_ZF))
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)

DEF(
    CALL,
    make_code(0,16,1), "N", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        ui8* data = (ui8*)&myCPU.Register.eip;
        for (ui8 i = 0; i < sizeof(ui32); i++)
            stackPush(&myCPU.stack, &data[i]);
        myCPU.Register.esp += sizeof(ui32);
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)

DEF(
    RET,
    make_code(0,17,0), "", "",
    {
        ui32 ptrReturn = 0;

        ui8* data = (ui8*)&ptrReturn;
        for (ui8 i = 0; i < sizeof(ui32); i++)
            stackPop(&myCPU.stack, &data[sizeof(ui32) - 1 - i]);
        myCPU.Register.esp -= sizeof(ui32);
        myCPU.Register.eip = ptrReturn;
    }
)


DEF(
    OR,
    make_code(0,18,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        dst->ivalue = dst->ivalue || src->ivalue;
    }
)

DEF(
    AND,
    make_code(0,19,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        dst->ivalue = dst->ivalue && src->ivalue;
    }
)

DEF(
    XOR,
    make_code(0,20,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        dst->ivalue = dst->ivalue ^ src->ivalue;
    }
)


DEF(
    MOVB,
    make_code(0,21,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        ui8 srcB = src->ivalue;
        memcpy(dst, &srcB, sizeof(ui8));
    }
)

DEF(
    MOVW,
    make_code(0,22,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        ui16 srcW = src->ivalue;
        memcpy(dst, &srcW, sizeof(ui16));
    }
)


DEF(
    OUT,
    make_code(0,23,1), "RNMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        printf("0x%04X \t(float: %f)\n", dst->ivalue & 0xFFFF, dst->fvalue);
    }
)


DEF(
    IN,
    make_code(0,24,1), "RMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        printf("enter int:");
        scanf("%d", &dst->ivalue);
    }
)

DEF(
    FIN,
    make_code(0,25,1), "RMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        printf("enter float:");
        scanf("%f", &dst->fvalue);
    }
)


DEF(
    DUMP,
    make_code(0,26,0), "", "",
    {
        CPU::Instance().dump(stdout);
        system("pause");
    }
)


// ==========================================================

//                 float point operation

// ==========================================================

#define FPU_ISA_START_CODE 27

//integer -> float
DEF(
    FILD,
    make_code(FPU_ISA_START_CODE,0,1), "RMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        
        dst->fvalue = static_cast<float>(dst->ivalue);
    }
)

//float -> integer
DEF(
    FISTP,
    make_code(FPU_ISA_START_CODE,1,1), "RMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isFiniteOperands();
        isInterruptOccur();
        dst->ivalue = static_cast<ui32>(dst->fvalue);
    }
)


DEF(
    FCOMP,
    make_code(FPU_ISA_START_CODE,2,2), "RNMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isFiniteOperands();
        isInterruptOccur();

        OperandUnion result;
        result.fvalue = dst->fvalue - src->fvalue;

        setBit(&myCPU.Register.efl, FLAG_CF, result.ivalue >> (sizeof(ui32) * 8 - 1));
        setBit(&myCPU.Register.efl, FLAG_ZF, isZero(result.fvalue));
        setBit(&myCPU.Register.efl, FLAG_SF, result.fvalue >= 0 ? 0 : 1);
    }
)

DEF(
    FADD,
    make_code(FPU_ISA_START_CODE,3,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isFiniteOperands();
        isInterruptOccur();
        dst->fvalue += src->fvalue;
    }
)

DEF(
    FSUB,
    make_code(FPU_ISA_START_CODE,4,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isFiniteOperands();
        isInterruptOccur();
        dst->fvalue -= src->fvalue;
    }
)

DEF(
    FDIV,
    make_code(FPU_ISA_START_CODE,5,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isFiniteOperands();
        isInterruptOccur();

        if (isZero(src->ivalue))
            myCPU.interruptCode = 1;
        else
            dst->fvalue /= src->fvalue;
    }
)

DEF(
    FMUL,
    make_code(FPU_ISA_START_CODE,6,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isFiniteOperands();
        isInterruptOccur();

        dst->fvalue *= src->fvalue;
    }
)

DEF(
    FSQRT,
    make_code(FPU_ISA_START_CODE,7,1), "RMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isFiniteOperands();
        isInterruptOccur();
        if(dst->fvalue < 0)
            myCPU.interruptCode = 1;
        else
            dst->fvalue = sqrt(dst->fvalue);
    }
)

DEF(
    FSIN,
    make_code(FPU_ISA_START_CODE,8,1), "RMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isFiniteOperands();
        isInterruptOccur();
        dst->fvalue = sinf(dst->fvalue);
    }
)

DEF(
    FCOS,
    make_code(FPU_ISA_START_CODE,9,1), "RMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isFiniteOperands();
        isInterruptOccur();
        dst->fvalue = cosf(dst->fvalue);
    }
)

DEF(
    ABS,
    make_code(FPU_ISA_START_CODE,10,1), "RMB", "",
    {
        OperandUnion* dst = (OperandUnion*)&myCPU.Register.eax;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &src, &dst);
        isInterruptOccur();
        dst->ivalue *= dst->ivalue < 0 ? -1 : 1;
    }
)

DEF(
    FABS,
    make_code(FPU_ISA_START_CODE,11,1), "RMB", "",
    {
        OperandUnion* dst = (OperandUnion*)&myCPU.Register.eax;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &src, &dst);
        isInterruptOccur();
        dst->fvalue *= dst->fvalue < 0 ? -1.0f : 1.0f;
    }
)


DEF(
    FPOW,
    make_code(FPU_ISA_START_CODE,12,2), "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isFiniteOperands();
        isInterruptOccur();
        dst->fvalue = powf(dst->fvalue, src->fvalue);
    }
)