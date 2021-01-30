/**
\file    В данном фалй описываются команды языка ассмеблера, а так же их реализации применительно
         к эмуляции на виртуальном процессоре.
\details Для задания новой команды требудется вызвать макрос, прототип которого имеет вид:
         #define DEF(name, machineCode, validStrOperand_1, validStrOperand_2, code)
         name --- имя команды, указывается ЗАГЛАВНЫМИ буквами
         machineCode --- машинный код команды, правило формирования кода указано ниже
         validStrOperand_1 --- строка допустимых типов первого операнда, правило формирование смотри ниже
         validStrOperand_2 --- строка допустимых типов второго операнда, правило формирование смотри ниже
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


         Типы символов в строках validOperand:
         R --- operand is Register
         N --- operand is Number
         M --- operand is Memory, based by number
         B --- operand is memory, Based by register
         Пример:
         Для команды mov строка validOperand будет равна "RMB" для первого операнда и "RNMB" для второго операнда.
         Такое кодирование означает, что команда mov не может записать данные в число.

*/

#define isInterruptOccur() if(myCPU.interruptCode) return;

DEF(
    HLT,
    0 << 8 | 0 << 4 | 0 << 2 | 0x0, "", "",
    {}
)

DEF(
    MOV,
    1 << 8 | 0 << 4 | 0 << 2 | 0x2, "RMB", "RNMB",
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
    2 << 8 | 0 << 4 | 0 << 2 | 0x2, "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();


        if (!myCPU.isFloatPointMath)
            dst->ivalue += src->ivalue;
        else
            dst->fvalue += src->fvalue;
    }
)

DEF(
    SUB,
    3 << 8 | 0 << 4 | 0 << 2 | 0x2, "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        if (!myCPU.isFloatPointMath)
            dst->ivalue -= src->ivalue;
        else
            dst->fvalue -= src->fvalue;
    }
)

DEF(
    DIV,
    4 << 8 | 0 << 4 | 0 << 2 | 0x2, "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();


        if (!myCPU.isFloatPointMath)
        {
            if (src->ivalue == 0)
                myCPU.interruptCode = 1; // при делении на ноль, возникает прерывание
            else
                dst->ivalue /= src->ivalue;
        }
        else
        {
            if (isZero(src->ivalue))
                myCPU.interruptCode = 1; // при делении на ноль, возникает прерывание
            else
                dst->fvalue /= src->fvalue;
        }
    }
)

DEF(
    MUL,
    5 << 8 | 0 << 4 | 0 << 2 | 0x2, "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        if (!myCPU.isFloatPointMath)
            dst->ivalue *= src->ivalue;
        else
            dst->fvalue *= src->fvalue;
    }
)

DEF(
    POP,
    6 << 8 | 0 << 4 | 0 << 2 | 0x1, "RMB", "",
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
    7 << 8 | 0 << 4 | 0 << 2 | 0x1, "RNMB", "",
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
    8 << 8 | 0 << 4 | 0 << 2 | 0x1, "N", "",
    {
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)


DEF(
    CMP,
    9 << 8 | 0 << 4 | 0 << 2 | 0x2, "RNMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        OperandUnion result;

        if (!myCPU.isFloatPointMath)
            result.ivalue = dst->ivalue - src->ivalue;
        else
            result.fvalue = dst->fvalue - src->fvalue;

        setBit(&myCPU.Register.efl, FLAG_CF, result.ivalue >> (sizeof(ui32) * 8 - 1));

        if (!myCPU.isFloatPointMath)
        {
            setBit(&myCPU.Register.efl, FLAG_ZF, result.ivalue == 0 ? 1 : 0);
            setBit(&myCPU.Register.efl, FLAG_SF, result.ivalue >= 0 ? 0 : 1);
        }
        else
        {
            setBit(&myCPU.Register.efl, FLAG_ZF, isZero(result.fvalue));
            setBit(&myCPU.Register.efl, FLAG_SF, result.fvalue >= 0 ? 0 : 1);
        }
    }
)

DEF(
    JE,
    10 << 8 | 0 << 4 | 0 << 2 | 0x1, "N", "",
    {
        if (getBit(myCPU.Register.efl, FLAG_ZF))
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)

DEF(
    JNE,
    11 << 8 | 0 << 4 | 0 << 2 | 0x1, "N", "",
    {
        if (!getBit(myCPU.Register.efl, FLAG_ZF))
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)

DEF(
    JA,
    12 << 8 | 0 << 4 | 0 << 2 | 0x1, "N", "",
    {
        if (!getBit(myCPU.Register.efl, FLAG_CF) && !getBit(myCPU.Register.efl, FLAG_ZF))
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)

DEF(
    JAE,
    13 << 8 | 0 << 4 | 0 << 2 | 0x1, "N", "",
    {
        if (!getBit(myCPU.Register.efl, FLAG_CF))
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)

DEF(
    JB,
    14 << 8 | 0 << 4 | 0 << 2 | 0x1, "N", "",
    {
        if (getBit(myCPU.Register.efl, FLAG_CF))
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)

DEF(
    JBE,
    15 << 8 | 0 << 4 | 0 << 2 | 0x1, "N", "",
    {
        if (getBit(myCPU.Register.efl, FLAG_CF) || getBit(myCPU.Register.efl, FLAG_ZF))
        myCPU.Register.eip = myCPU.Register.ecs + cmd->operand[0].ivalue;
    }
)

DEF(
    CALL,
    16 << 8 | 0 << 4 | 0 << 2 | 0x1, "N", "",
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
    17 << 8 | 0 << 4 | 0 << 2 | 0x0, "", "",
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
    FPMON,
    18 << 8 | 0 << 4 | 0 << 2 | 0x0, "", "",
    {
        myCPU.isFloatPointMath = 1;
    }
)

DEF(
    FPMOFF,
    19 << 8 | 0 << 4 | 0 << 2 | 0x0, "", "",
    {
        myCPU.isFloatPointMath = 0;
    }
)


DEF(
    SQRT,
    20 << 8 | 0 << 4 | 0 << 2 | 0x1, "RNMB", "",
    {
        OperandUnion* dst = (OperandUnion*)&myCPU.Register.eax;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &src, &dst);
        isInterruptOccur();

        if (src->fvalue < 0 || !myCPU.isFloatPointMath)
        {
            myCPU.interruptCode = 3; // извлечение корня из отрицательного числа
            return;
        }
        dst->fvalue = sqrt(src->fvalue);
    }
)


DEF(
    TRUNC,
    21 << 8 | 0 << 4 | 0 << 2 | 0x1, "RMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        dst->ivalue = static_cast<ui32>(dst->fvalue);
    }
)


DEF(
    SIN,
    22 << 8 | 0 << 4 | 0 << 2 | 0x1, "RNMB", "",
    {
        OperandUnion* dst = (OperandUnion*)&myCPU.Register.eax;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &src, &dst);
        isInterruptOccur();

        if (!myCPU.isFloatPointMath)
        {
            myCPU.interruptCode = 3; // должна быть включена арифметика с плавающей точкой
            return;
        }

        dst->fvalue = sinf(src->fvalue);
    }
)


DEF(
    COS,
    23 << 8 | 0 << 4 | 0 << 2 | 0x1, "RNMB", "",
    {
        OperandUnion* dst = (OperandUnion*)&myCPU.Register.eax;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &src, &dst);
        isInterruptOccur();

        if (!myCPU.isFloatPointMath)
        {
            myCPU.interruptCode = 3; // должна быть включена арифметика с плавающей точкой
            return;
        }

        dst->fvalue = cosf(src->fvalue);
    }
)


DEF(
    MOVB,
    24 << 8 | 0 << 4 | 0 << 2 | 0x2, "RMB", "RNMB",
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
    25 << 8 | 0 << 4 | 0 << 2 | 0x2, "RMB", "RNMB",
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
    DUMP,
    26 << 8 | 0 << 4 | 0 << 2 | 0x0, "", "",
    {
        Disassembler::Instance().disasmCommand(*cmd, stdout);
        CPU::Instance().dump();
        system("pause");
    }
)

DEF(
    FLOAT,
    27 << 8 | 0 << 4 | 0 << 2 | 0x1, "RMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        dst->fvalue = static_cast<float>(dst->ivalue);
    }
)

DEF(
    OUT,
    28 << 8 | 0 << 4 | 0 << 2 | 0x1, "RNMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();



        if (myCPU.isFloatPointMath)
            printf("%f\n", dst->fvalue);
        else
            printf("%d (0x%X)\n", static_cast<i32>(dst->ivalue), dst->ivalue);
    }
)

DEF(
    IN,
    29 << 8 | 0 << 4 | 0 << 2 | 0x1, "RMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();


        if (myCPU.isFloatPointMath)
        {
            printf("enter float:");
            scanf("%f", &dst->fvalue);
        }
        else
        {
            printf("enter int:");
            scanf("%d", &dst->ivalue);
        }
    }
)

DEF(
    OR,
    30 << 8 | 0 << 4 | 0 << 2 | 0x2, "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        if (!myCPU.isFloatPointMath)
            dst->ivalue = dst->ivalue || src->ivalue;
        else
            dst->fvalue = isZero(dst->fvalue) || isZero(src->fvalue) ? 1.0 : 0.0;
    }
)

DEF(
    AND,
    31 << 8 | 0 << 4 | 0 << 2 | 0x2, "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        if (!myCPU.isFloatPointMath)
            dst->ivalue = dst->ivalue && src->ivalue;
        else
            dst->fvalue = !isZero(dst->fvalue) && !isZero(src->fvalue) ? 1.0 : 0.0;
    }
)

DEF(
    ABS,
    32 << 8 | 0 << 4 | 0 << 2 | 0x1, "RNMB", "",
    {
        OperandUnion* dst = (OperandUnion*)&myCPU.Register.eax;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &src, &dst);
        isInterruptOccur();

        if (!myCPU.isFloatPointMath)
            dst->ivalue = static_cast<ui32>(abs(static_cast<i32>(src->ivalue)));
        else
            dst->fvalue = abs(src->fvalue);
    }
)

DEF(
    POW,
    33 << 8 | 0 << 4 | 0 << 2 | 0x2, "RMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        if (!myCPU.isFloatPointMath)
        {
            myCPU.interruptCode = 3; // floating-point math should be on
            return;
        }

        dst->fvalue = powf(dst->fvalue, src->fvalue);
    }
)