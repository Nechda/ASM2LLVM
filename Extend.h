/**
\file    В данном фалй описываются команды языка ассмеблера, а так же их реализации применительно
         к эмуляции на виртуальном процессоре.
\details Для задания новой команды требудется вызвать макрос, прототип которого имеет вид:
         #define DEF(name, mCode, vStr1, vStr2, bits)
         name --- имя команды, указывается ЗАГЛАВНЫМИ буквами
         mCode --- машинный код команды, правило формирования кода указано ниже
         vStr1 --- строка допустимых типов первого операнда, правило формирование смотри ниже
         vStr2 --- строка допустимых типов второго операнда, правило формирование смотри ниже
         bits --- код функции, реализующий действие команды на процессоре


         Структура кодирования команд в машинном коде:
         bytes:             6                    1               1                      2                       2                       2                    2
         description: command general bits | reserved | long memory command | type of third operand | type of second operand | type of first operand  | nOperands

         Кодирование типов операндов в машинном коде:
         0b00 --- operand is register
         0b01 --- operand is number
         0b10 --- operand is memory, based by number
         0b11 --- operand is memory, based by register

         Бит long memory command:
         0b1  --- if operands are mem based by register, then bits become longer
              first 8 bits it's register number then 32 bits for describing offset.
              Example:
              mov eax, 0x10
              mov ebx, [eax + 0x04]
              ; second operand is coded as 0b00000001 0b00000000 0b00000000 0b00000000 0b00000100

              Note: if long memory bit is 1 then all operands which type is mem by register are coding as described above
              Example:
              mov [eax + 0x3],[ebx + 0x8]
              ; first operand:  0b00000001 0b00000000 0b00000000 0b00000000 0b00000011
              ; second operand: 0b00000002 0b00000000 0b00000000 0b00000000 0b00001000
        0b0 --- in this situation operands coding without addition information

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
    (start + offset) << 10 | 0 << 8 | 0 << 6 | 0 << 4 | 0 << 2 | (nOperands) 
#define isInterruptOccur() if(context.interruptCode) return;
#define isFiniteOperands()\
{\
    if (dst && !context.interruptCode) context.interruptCode |= !std::isfinite(dst->fvalue);\
    if (src && !context.interruptCode) context.interruptCode |= !std::isfinite(src->fvalue);\
 }

DEF(
    HLT,
    make_code(0,0,0), "", "", "",
    {}
)

DEF(
    MOV,
    make_code(0,1,2), "RMB", "RNMB", "",
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
    make_code(0, 2, 3), "RMB", "RNMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        getOperandsPointer(cmd, &dst, &op1, &op2);
        isInterruptOccur();

        dst->ivalue = op1->ivalue + op2->ivalue;
    }
)

DEF(
    SUB,
    make_code(0, 3, 3), "RMB", "RNMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        getOperandsPointer(cmd, &dst, &op1, &op2);
        isInterruptOccur();


        dst->ivalue = op1->ivalue - op2->ivalue;
    }
)

DEF(
    DIV,
    make_code(0, 4, 3), "RMB", "RNMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        getOperandsPointer(cmd, &dst, &op1, &op2);
        isInterruptOccur();

        if (op2->ivalue == 0)
            context.interruptCode = 1;
        else
            dst->ivalue = op1->ivalue / op2->ivalue;
    }
)

DEF(
    MUL,
    make_code(0, 5, 3), "RMB", "RNMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        getOperandsPointer(cmd, &dst, &op1, &op2);
        isInterruptOccur();

        dst->ivalue = op1->ivalue * op2->ivalue;
    }
)

DEF(
    POP,
    make_code(0,6,1), "RMB", "", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        ui8* data = (ui8*)dst;
        for (ui8 i = 0; i < sizeof(ui32); i++)
            stackPop(&context.stack, &data[sizeof(ui32) - 1 - i]);

        //в данной реализации процессора в качестве стека используется immortal stack,
        //соттветственно при push он растет в сторону больших адресов,
        //а при pop адрес вершины уменьшается
        context.Register.esp -= sizeof(ui32);
    }
)

DEF(
    PUSH,
    make_code(0,7,1), "RNMB", "", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        ui8* data = (ui8*)dst;
        for (ui8 i = 0; i < sizeof(ui32); i++)
            stackPush(&context.stack, &data[i]);

        //в данной реализации процессора в качестве стека используется immortal stack,
        //соттветственно при push он растет в сторону больших адресов,
        //а при pop адрес вершины уменьшается
        context.Register.esp += sizeof(ui32);
    }
)

DEF(
    JMP,
    make_code(0,8,1), "N", "", "",
    {
        context.pc = cmd->operand[0].ivalue;
    }
)

DEF(
    JE,
    make_code(0, 9, 3), "RNMB", "RNMB", "N",
    {
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        OperandUnion* addr = NULL;
        getOperandsPointer(cmd, &op1, &op2, &addr);
        isInterruptOccur();
        if (op1->ivalue == op2->ivalue)
        context.pc = addr->ivalue;
    }
)

DEF(
    JNE,
    make_code(0, 10, 3), "RNMB", "RNMB", "N",
    {
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        OperandUnion* addr = NULL;
        getOperandsPointer(cmd, &op1, &op2, &addr);
        isInterruptOccur();
        if (op1->ivalue != op2->ivalue)
            context.pc = addr->ivalue;
    }
)

DEF(
    JA,
    make_code(0, 11, 3), "RNMB", "RNMB", "N",
    {
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        OperandUnion* addr = NULL;
        getOperandsPointer(cmd, &op1, &op2, &addr);
        isInterruptOccur();
        if (op1->ivalue > op2->ivalue)
        context.pc = addr->ivalue;
    }
)

DEF(
    JAE,
    make_code(0, 12, 3), "RNMB", "RNMB", "N",
    {
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        OperandUnion* addr = NULL;
        getOperandsPointer(cmd, &op1, &op2, &addr);
        isInterruptOccur();
        if (op1->ivalue >= op2->ivalue)
        context.pc = addr->ivalue;
    }
)


DEF(
    CALL,
    make_code(0,13,1), "N", "", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();

        ui8* data = (ui8*)&context.pc;
        for (ui8 i = 0; i < sizeof(ui32); i++)
            stackPush(&context.stack, &data[i]);
        context.Register.esp += sizeof(ui32);
        context.pc = cmd->operand[0].ivalue;
    }
)

DEF(
    RET,
    make_code(0,14,0), "", "", "",
    {
        ui32 ptrReturn = 0;

        ui8* data = (ui8*)&ptrReturn;
        for (ui8 i = 0; i < sizeof(ui32); i++)
            stackPop(&context.stack, &data[sizeof(ui32) - 1 - i]);
        context.Register.esp -= sizeof(ui32);
        context.pc = ptrReturn;
    }
)


DEF(
    OR,
    make_code(0,15,3), "RMB", "RNMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        getOperandsPointer(cmd, &dst, &op1, &op2);
        isInterruptOccur();
        dst->ivalue = op1->ivalue | op2->ivalue;
    }
)

DEF(
    AND,
    make_code(0,16,3), "RMB", "RNMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        getOperandsPointer(cmd, &dst, &op1, &op2);
        isInterruptOccur();
        dst->ivalue = op1->ivalue & op2->ivalue;
    }
)

DEF(
    XOR,
    make_code(0,17,3), "RMB", "RNMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        getOperandsPointer(cmd, &dst, &op1, &op2);
        isInterruptOccur();
        dst->ivalue = op1->ivalue & op2->ivalue;
    }
)


DEF(
    MOVB,
    make_code(0,18,2), "RMB", "RNMB", "",
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
    make_code(0,19,2), "RMB", "RNMB", "",
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
    make_code(0,20,1), "RNMB", "", "",
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
    make_code(0,21,1), "RMB", "", "",
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
    make_code(0,22,1), "RMB", "", "",
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
    make_code(0,23,0), "", "", "",
    {
        //glFlush();
        //glClear(GL_COLOR_BUFFER_BIT);
        //CPU::Instance().dump(stdout);
        //system("pause");
    }
)


// ==========================================================

//                 float point operation

// ==========================================================

#define FPU_ISA_START_CODE 24

//integer -> float
DEF(
    FILD,
    make_code(FPU_ISA_START_CODE,0,1), "RMB", "", "",
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
    make_code(FPU_ISA_START_CODE,1,1), "RMB", "", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isFiniteOperands();
        isInterruptOccur();
        //printf("float: %f -> int: %d\n", dst->fvalue, static_cast<int>(dst->fvalue));
        dst->ivalue = static_cast<int>(dst->fvalue);
    }
)

DEF(
    FJE,
    make_code(FPU_ISA_START_CODE, 2, 3), "RNMB", "RNMB", "N",
    {
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        OperandUnion* addr = NULL;
        getOperandsPointer(cmd, &op1, &op2, &addr);
        isInterruptOccur();
        if (isZero(op1->fvalue - op2->fvalue))
        context.pc =  addr->ivalue;
    }
)

DEF(
    FJNE,
    make_code(FPU_ISA_START_CODE, 3, 3), "RNMB", "RNMB", "N",
    {
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        OperandUnion* addr = NULL;
        getOperandsPointer(cmd, &op1, &op2, &addr);
        isInterruptOccur();
        if (!isZero(op1->fvalue - op2->fvalue))
        context.pc =  addr->ivalue;
    }
)

DEF(
    FJA,
    make_code(FPU_ISA_START_CODE, 4, 3), "RNMB", "RNMB", "N",
    {
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        OperandUnion* addr = NULL;
        getOperandsPointer(cmd, &op1, &op2, &addr);
        isInterruptOccur();
        if (op1->fvalue > op2->fvalue)
        context.pc =  addr->ivalue;
    }
)

DEF(
    FJAE,
    make_code(FPU_ISA_START_CODE, 5, 3), "RNMB", "RNMB", "N",
    {
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        OperandUnion* addr = NULL;
        getOperandsPointer(cmd, &op1, &op2, &addr);
        isInterruptOccur();
        if (op1->fvalue >= op2->fvalue)
        context.pc =  addr->ivalue;
    }
)


DEF(
    FADD,
    make_code(FPU_ISA_START_CODE,6,3), "RMB", "RNMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        getOperandsPointer(cmd, &dst, &op1, &op2);
        isInterruptOccur();

        dst->fvalue = op1->fvalue + op2->fvalue;
    }
)

DEF(
    FSUB,
    make_code(FPU_ISA_START_CODE,7,3), "RMB", "RNMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        getOperandsPointer(cmd, &dst, &op1, &op2);
        isInterruptOccur();

        dst->fvalue = op1->fvalue - op2->fvalue;
    }
)

DEF(
    FDIV,
    make_code(FPU_ISA_START_CODE,8,3), "RMB", "RNMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        getOperandsPointer(cmd, &dst, &op1, &op2);
        isInterruptOccur();

        if (isZero(op2->ivalue))
            context.interruptCode = 1;
        else
            dst->fvalue = op1->fvalue / op2->fvalue;
    }
)

DEF(
    FMUL,
    make_code(FPU_ISA_START_CODE,9,3), "RMB", "RNMB", "RNMB",
    {
        OperandUnion* dst = NULL;
        OperandUnion* op1 = NULL;
        OperandUnion* op2 = NULL;
        getOperandsPointer(cmd, &dst, &op1, &op2);
        isInterruptOccur();

        dst->fvalue = op1->fvalue * op2->fvalue;
    }
)

DEF(
    FSQRT,
    make_code(FPU_ISA_START_CODE,10,1), "RMB", "", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isFiniteOperands();
        isInterruptOccur();
        if(dst->fvalue < 0)
            context.interruptCode = 1;
        else
            dst->fvalue = sqrt(dst->fvalue);
    }
)

DEF(
    FSIN,
    make_code(FPU_ISA_START_CODE,11,1), "RMB", "", "",
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
    make_code(FPU_ISA_START_CODE,12,1), "RMB", "", "",
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
    make_code(FPU_ISA_START_CODE,13,1), "RMB", "", "",
    {
        OperandUnion* dst = (OperandUnion*)&context.Register.eax;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &src, &dst);
        isInterruptOccur();
        dst->ivalue *= dst->ivalue < 0 ? -1 : 1;
    }
)

DEF(
    FABS,
    make_code(FPU_ISA_START_CODE,14,1), "RMB", "", "",
    {
        OperandUnion* dst = (OperandUnion*)&context.Register.eax;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &src, &dst);
        isInterruptOccur();
        dst->fvalue = fabs(dst->fvalue);
    }
)


DEF(
    FPOW,
    make_code(FPU_ISA_START_CODE,15,2), "RMB", "RNMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isFiniteOperands();
        isInterruptOccur();
        dst->fvalue = powf(dst->fvalue, src->fvalue);
    }
)

DEF(
    ATAN2,
    make_code(FPU_ISA_START_CODE, 16,2), "RMB", "RNMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isFiniteOperands();
        isInterruptOccur();
        dst->fvalue = atan2(dst->fvalue,src->fvalue);
    }
)

DEF(
    MOD,
    make_code(FPU_ISA_START_CODE, 17, 2), "RMB", "RNMB", "",
    {
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        dst->ivalue = abs((int)dst->ivalue % (int)src->ivalue);
    }
)


// ==========================================================

//                 Improved SIMD operations

// ==========================================================


#define ISIMD_ISA_START_CODE 42

#define PUSH_REGISTER(reg)\
{\
    ui8* data = (ui8*)&context.Register.##reg;\
    for (ui8 i = 0; i < sizeof(ui32); i++)\
        stackPush(&context.stack, &data[i]);\
}

DEF(
    PUSHA,
    make_code(ISIMD_ISA_START_CODE, 0, 0), "","", "",
    {
        isInterruptOccur();
        PUSH_REGISTER(eax)
        PUSH_REGISTER(ebx)
        PUSH_REGISTER(ecx)
        PUSH_REGISTER(edx)
        PUSH_REGISTER(esi)
        PUSH_REGISTER(edi)
        PUSH_REGISTER(ebp)
        

        context.Register.esp += 7 * sizeof(ui32);
    }
)
#undef PUSH_REGISTER

#define POP_REGISTER(reg)\
{\
    ui8* data = (ui8*)&context.Register.##reg;\
    for (ui8 i = 0; i < sizeof(ui32); i++)\
        stackPop(&context.stack, &data[sizeof(ui32) - 1 - i]);\
}

DEF(
    POPA,
    make_code(ISIMD_ISA_START_CODE, 1, 0), "", "", "",
    {
        isInterruptOccur();
        POP_REGISTER(ebp)
        POP_REGISTER(edi)
        POP_REGISTER(esi)
        POP_REGISTER(edx)
        POP_REGISTER(ecx)
        POP_REGISTER(ebx)
        POP_REGISTER(eax)

        context.Register.esp -= 7 * sizeof(ui32);
    }
)
#undef POP_REGISTER

/// vectored operations

#define Assert_long_register(reg, command)\
    do{if (!(10 <= reg && reg <= 17))\
    assert(!#command" has not long register as first operand.");}while(0)
DEF(
    LLZA,
    make_code(ISIMD_ISA_START_CODE, 2, 0), "", "", "",
    {
        OperandUnion* dst = (OperandUnion*)&context.Register.lr0;
        memset(dst, 0, sizeof(ui32) * 4 * 8);
    }
)

DEF(
    LLZ,
    make_code(ISIMD_ISA_START_CODE, 3, 1), "R", "", "",
    {
        Assert_long_register(cmd->operand[0].ivalue, LLZ);
        OperandUnion* dst = 
            (OperandUnion*)(&context.Register.lr0 + 4*sizeof(ui32)*(cmd->operand[0].ivalue - 10));
        memset(dst, 0, sizeof(ui32) * 4);
    }
)

DEF(
    LLOAD,
    make_code(ISIMD_ISA_START_CODE, 4, 3), "R", "MB", "N",
    {
        Assert_long_register(cmd->operand[0].ivalue, LLOAD);
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        OperandUnion* offset = NULL;
        getOperandsPointer(cmd, &dst, &src, &offset);
        isInterruptOccur();
        for (ui8 i = 0; i < 4; i++, src += offset->ivalue, dst++)
            dst->ivalue = src->ivalue;
    }
)

DEF(
    LMOV,
    make_code(ISIMD_ISA_START_CODE, 5, 3), "RMB", "RMB", "N",
    {
        if(cmd->bits.typeFirst == OPERAND_REGISTER)
            Assert_long_register(cmd->operand[0].ivalue, LMOV);
        if (cmd->bits.typeSecond == OPERAND_REGISTER)
            Assert_long_register(cmd->operand[1].ivalue, LMOV);

        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        OperandUnion* number = NULL;
        getOperandsPointer(cmd, &dst, &src, &number);
        isInterruptOccur();

        if (number->ivalue > 4 && number < 0)
            assert(!"LMOV operation should have less than 4 number of moved ints!");

        memcpy(dst, src, sizeof(ui32) * number->ivalue);
    }
)


#define LONGREGISTER_GET_OPERANDS(operation)\
    Assert_long_register(cmd->operand[0].ivalue, operation);\
    Assert_long_register(cmd->operand[1].ivalue, operation);\
    Assert_long_register(cmd->operand[2].ivalue, operation);\
    OperandUnion* dst = NULL;\
    OperandUnion* op1 = NULL;\
    OperandUnion* op2 = NULL;\
    getOperandsPointer(cmd, &dst, &op1, &op2);\
    isInterruptOccur();

#define LONGREGISTER_DO_OPERATION(operation, filed)\
    for (ui8 i = 0; i < 4; i++, dst++, op1++, op2++)\
        dst->filed = op1->filed operation op2->filed;

DEF(
    LADD,
    make_code(ISIMD_ISA_START_CODE, 6, 3), "R", "R", "R",
    {
        LONGREGISTER_GET_OPERANDS(LADD);
        LONGREGISTER_DO_OPERATION(+, ivalue);
    }
)

DEF(
    LSUB,
    make_code(ISIMD_ISA_START_CODE, 7, 3), "R", "R", "R",
    {
        LONGREGISTER_GET_OPERANDS(LSUB);
        LONGREGISTER_DO_OPERATION(-, ivalue);
    }
)

DEF(
    LMUL,
    make_code(ISIMD_ISA_START_CODE, 8, 3), "R", "R", "R",
    {
        LONGREGISTER_GET_OPERANDS(LMUL);
        LONGREGISTER_DO_OPERATION(*, ivalue);
    }
)

DEF(
    LFADD,
    make_code(ISIMD_ISA_START_CODE, 9, 3), "R", "R", "R",
    {
        LONGREGISTER_GET_OPERANDS(LFADD);
        LONGREGISTER_DO_OPERATION(+, fvalue);
    }
)

DEF(
    LFSUB,
    make_code(ISIMD_ISA_START_CODE, 10, 3), "R", "R", "R",
    {
        LONGREGISTER_GET_OPERANDS(LFSUB);
        LONGREGISTER_DO_OPERATION(-, fvalue);
    }
)

DEF(
    LFMUL,
    make_code(ISIMD_ISA_START_CODE, 11, 3), "R", "R", "R",
    {
        LONGREGISTER_GET_OPERANDS(LFMULL);
        LONGREGISTER_DO_OPERATION(*, fvalue);
    }
)

DEF(
    LDIV,
    make_code(ISIMD_ISA_START_CODE, 12, 3), "R", "R", "R",
    {
        LONGREGISTER_GET_OPERANDS(LDIV);
        for (ui8 i = 0; i < 4; i++, dst++, op1++, op2++)
        {
            if (op2->ivalue == 0)
            {
                context.interruptCode = 1;
                return;
            }
            dst->ivalue = op1->ivalue / op2->ivalue;
        }
    }
)

DEF(
    LFDIV,
    make_code(ISIMD_ISA_START_CODE, 13, 3), "R", "R", "R",
    {
        LONGREGISTER_GET_OPERANDS(LFDIV);
        for (ui8 i = 0; i < 4; i++, dst++, op1++, op2++)
        {
            if (isZero(op2->fvalue))
            {
                context.interruptCode = 1;
                return;
            }
            dst->fvalue = op1->fvalue / op2->fvalue;
        }
    }
)

#undef LONGREGISTER_GET_OPERANDS
#undef LONGREGISTER_DO_OPERATION


DEF(
    LSHRINK,
    make_code(ISIMD_ISA_START_CODE, 14, 2), "R", "N", "",
    {
        Assert_long_register(cmd->operand[0].ivalue, LSHRINK);
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
        info.mask = cmd->operand[1].ivalue;

        ui32* lr = (ui32*)&context.Register.lr0 + 4 * (cmd->operand[0].ivalue - 10);
        
        ((ui32*)&context.Register.eax)[info.r0] = lr[0];
        ((ui32*)&context.Register.eax)[info.r1] = lr[1];
        ((ui32*)&context.Register.eax)[info.r2] = lr[2];
        ((ui32*)&context.Register.eax)[info.r3] = lr[3];
    }
)

DEF(
    LEXPAND,
    make_code(ISIMD_ISA_START_CODE, 15, 2), "R", "N", "",
    {
        Assert_long_register(cmd->operand[0].ivalue, LEXPAND);
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
        info.mask = cmd->operand[1].ivalue;

        ui32* lr = (ui32*)&context.Register.lr0 + 4 * (cmd->operand[0].ivalue - 10);
        lr[0] = ((ui32*)&context.Register.eax)[info.r0];
        lr[1] = ((ui32*)&context.Register.eax)[info.r1];
        lr[2] = ((ui32*)&context.Register.eax)[info.r2];
        lr[3] = ((ui32*)&context.Register.eax)[info.r3];
    }
)


DEF(
    LSHRINKA,
    make_code(ISIMD_ISA_START_CODE, 16, 1), "R", "", "",
    {
        Assert_long_register(cmd->operand[0].ivalue, LSHRINKA);
        ui32* lr = (ui32*)&context.Register.lr0 + 4 * (cmd->operand[0].ivalue - 10);
        memcpy(&context.Register.eax, lr, 4 * sizeof(ui32));
    }
)

DEF(
    LEXPANDA,
    make_code(ISIMD_ISA_START_CODE, 17, 1), "R", "", "",
    {
        Assert_long_register(cmd->operand[0].ivalue, LEXPANDA);
        ui32* lr = (ui32*)&context.Register.lr0 + 4 * (cmd->operand[0].ivalue - 10);
        memcpy(lr, &context.Register.eax, 4 * sizeof(ui32));
    }
)

DEF(
    LACCUM,
    make_code(ISIMD_ISA_START_CODE, 18, 2), "RMB", "R", "",
    {
        Assert_long_register(cmd->operand[1].ivalue, LACCUM);
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        dst->ivalue = 0;
        for (ui8 i = 0; i < 4; i++, src++)
            dst->ivalue += src->ivalue;
    }
)

DEF(
    LFACCUM,
    make_code(ISIMD_ISA_START_CODE, 19, 2), "RMB", "R", "",
    {
        Assert_long_register(cmd->operand[1].ivalue, LFACCUM);
        OperandUnion* dst = NULL;
        OperandUnion* src = NULL;
        getOperandsPointer(cmd, &dst, &src);
        isInterruptOccur();
        dst->ivalue = 0;
        for (ui8 i = 0; i < 4; i++, src++)
            dst->fvalue += src->fvalue;
    }
)