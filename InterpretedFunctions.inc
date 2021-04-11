INTERPRETATED_FUNCTION(
    OUT,
    {
        OperandUnion* dst = (OperandUnion*)getOperand(marchCode, 0, op1, ex1);
        printf("0x%08X \t(float: %f)\n", dst->ivalue, dst->fvalue);
    }
)


INTERPRETATED_FUNCTION(
    IN,
    {
        OperandUnion* dst = (OperandUnion*)getOperand(marchCode, 0, op1, ex1);
        printf("enter int:");
        scanf("%d", &dst->ivalue);
    }
)


INTERPRETATED_FUNCTION(
    FIN,
    {
        OperandUnion* dst = (OperandUnion*)getOperand(marchCode, 0, op1, ex1);
        printf("enter float:");
        scanf("%f", &dst->fvalue);
    }
)


INTERPRETATED_FUNCTION(
    ABS,
    {
        OperandUnion* dst = (OperandUnion*)getOperand(marchCode, 0, op1, ex1);

        dst->ivalue *= dst->ivalue < 0 ? -1 : 1;
    }
)


INTERPRETATED_FUNCTION(
    ATAN2,
    {
        OperandUnion* dst = (OperandUnion*)getOperand(marchCode, 0, op1, ex1);
		OperandUnion* src = (OperandUnion*)getOperand(marchCode, 1, op2, ex2);

        dst->fvalue = atan2(dst->fvalue, src->fvalue);
    }
)