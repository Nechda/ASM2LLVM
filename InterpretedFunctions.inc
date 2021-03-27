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
    FIN,
    {
        OperandUnion* dst = (OperandUnion*)getOperand(marchCode, 0, op1);
        printf("enter float:");
        scanf("%f", &dst->fvalue);
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
