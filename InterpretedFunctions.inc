INTERPRETATED_FUNCTION(
    OUT,
    {
        OperandUnion* dst = (OperandUnion*)getOperand(marchCode, 0, op1);
        printf("0x%08X \t(float: %f)\n", dst->ivalue, dst->fvalue);
    }
)



INTERPRETATED_FUNCTION(
    IN,
    {
        OperandUnion* dst = (OperandUnion*)getOperand(marchCode, 0, op1);
        printf("enter int:");
        scanf("%d", &dst->ivalue);
    }
)


INTERPRETATED_FUNCTION(
    CMP,
    {
        OperandUnion* dst = (OperandUnion*)getOperand(marchCode, 0, op1);
        OperandUnion* src = (OperandUnion*)getOperand(marchCode, 1, op2);
        
        OperandUnion result;
        result.ivalue = dst->ivalue - src->ivalue;

        setBit(&regTable[EFL_REG_INDEX], FLAG_CF, result.ivalue >> (sizeof(ui32) * 8 - 1));
        setBit(&regTable[EFL_REG_INDEX], FLAG_ZF, result.ivalue == 0 ? 1 : 0);
        setBit(&regTable[EFL_REG_INDEX], FLAG_SF, result.ivalue >= 0 ? 0 : 1);
    }
)

INTERPRETATED_FUNCTION(
    FIN,
    {
        OperandUnion* dst = (OperandUnion*)getOperand(marchCode, 0, op1);
        printf("enter float:");
        scanf("%f", &dst->fvalue);
    }
)

INTERPRETATED_FUNCTION(
    FCOMP,
    {
        OperandUnion* dst = (OperandUnion*)getOperand(marchCode, 0, op1);
        OperandUnion* src = (OperandUnion*)getOperand(marchCode, 1, op2);
        
        OperandUnion result;
        result.fvalue = dst->fvalue - src->fvalue;

        setBit(&regTable[EFL_REG_INDEX], FLAG_CF, result.ivalue >> (sizeof(ui32) * 8 - 1));
        setBit(&regTable[EFL_REG_INDEX], FLAG_ZF, isZero(result.fvalue));
        setBit(&regTable[EFL_REG_INDEX], FLAG_SF, result.fvalue >= 0 ? 0 : 1);
    }
)

INTERPRETATED_FUNCTION(
    FSQRT,
    {
        OperandUnion* dst = (OperandUnion*)getOperand(marchCode, 0, op1);

        if (dst->fvalue >= 0)
            dst->fvalue = sqrt(dst->fvalue);
    }
)

INTERPRETATED_FUNCTION(
    FPOW,
    {
        OperandUnion* dst = (OperandUnion*)getOperand(marchCode, 0, op1);
        OperandUnion* src = (OperandUnion*)getOperand(marchCode, 1, op2);

        dst->fvalue = powf(dst->fvalue, src->fvalue);
    }
)

INTERPRETATED_FUNCTION(
    FABS,
    {
        OperandUnion* dst = (OperandUnion*)getOperand(marchCode, 0, op1);

        dst->fvalue *= dst->fvalue < 0 ? -1.0f : 1.0f;
    }
)
