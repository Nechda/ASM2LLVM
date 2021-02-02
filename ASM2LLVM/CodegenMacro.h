#pragma once

#if defined(CODE_GENERATE_MACRO)
#define DECLARE_FUNC_WITH_1_PARAMS(irFunc)\
[&](Value* op1)\
{\
    Value* res = builder.Create##irFunc(op1);\
    ringBufValue.push_back(res);\
    return res;\
}

#define DECLARE_FUNC_WITH_2_PARAMS(irFunc)\
[&](Value* op1, Value* op2)\
{\
    Value* res = builder.Create##irFunc(op1,op2);\
    ringBufValue.push_back(res);\
    return res;\
}

#define DECLARE_FUNC_WITH_2_PARAMS_I(irFunc)\
[&](Value* op1, ui32 op2)\
{\
    Value* res = builder.Create##irFunc(op1,op2);\
    ringBufValue.push_back(res);\
    return res;\
}

#define DECLARE_FUNC_WITH_2_UNION_PARAMS(irFunc)\
[&](Value* op1, Value* op2)\
{\
    Value* res = builder.Create##irFunc({op1,op2});\
    ringBufValue.push_back(res);\
    return res;\
}


#define DECLARE_FUNC_WITH_1_PARAMS_STK(irFunc)\
[&]()\
{\
    Value* arg = ringBufValue.back(); ringBufValue.pop_back();\
    ringBufValue.push_back(builder.Create##irFunc(arg));\
}

#define DECLARE_FUNC_WITH_1_PARAMS_STK_1(irFunc, _type)\
[&](_type* arg2)\
{\
    Value* arg1 = ringBufValue.back(); ringBufValue.pop_back();\
    ringBufValue.push_back(builder.Create##irFunc(arg1, arg2));\
}

#define DECLARE_FUNC_WITH_2_PARAMS_STK(irFunc)\
[&]()\
{\
    Value* arg1 = ringBufValue.back(); ringBufValue.pop_back();\
    Value* arg2 = ringBufValue.back(); ringBufValue.pop_back();\
    ringBufValue.push_back(builder.Create##irFunc(arg1, arg2));\
}

#define DECLARE_FUNC_WITH_2_UNION_PARAMS_STK(irFunc)\
[&]()\
{\
    Value* arg1 = ringBufValue.back(); ringBufValue.pop_back();\
    Value* arg2 = ringBufValue.back(); ringBufValue.pop_back();\
    ringBufValue.push_back(builder.Create##irFunc({arg1, arg2}));\
}
#endif