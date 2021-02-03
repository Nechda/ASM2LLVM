#include "Stack_kernel.h"
#include <string.h>
#include <ctime>


/**
\brief Таблица перестановки 256 энементов для алгоритма хеширования Пирсона
*/
static const ui8 T[256] = {
    249, 232, 89, 20, 244, 97, 50, 114, 220, 107, 86, 150, 67, 233, 42, 226,
    209, 3, 206, 74, 207, 180, 85, 216, 21, 191, 246, 82, 137, 186, 128, 40,
    172, 15, 96, 93, 152, 60, 240, 95, 122, 2, 164, 33, 112, 17, 201, 129,
    22, 248, 225, 132, 76, 163, 127, 139, 118, 57, 136, 8, 37, 245, 195, 16,
    43, 87, 69, 0, 39, 188, 254, 130, 251, 213, 243, 222, 78, 223, 6, 228,
    231, 211, 106, 119, 124, 174, 155, 14, 189, 29, 101, 113, 70, 196, 18, 173,
    35, 167, 229, 92, 239, 157, 83, 28, 25, 212, 215, 237, 203, 62, 10, 156,
    160, 63, 59, 9, 79, 44, 141, 47, 34, 252, 158, 90, 64, 68, 27, 170,
    56, 49, 108, 146, 5, 236, 100, 55, 26, 178, 175, 241, 65, 110, 54, 159,
    147, 205, 135, 224, 198, 61, 120, 1, 154, 208, 7, 126, 138, 32, 161, 53,
    165, 71, 148, 73, 13, 94, 11, 84, 38, 104, 77, 45, 81, 131, 193, 255,
    234, 88, 217, 179, 4, 116, 219, 145, 168, 75, 171, 204, 192, 140, 166, 185,
    30, 218, 151, 48, 24, 176, 80, 143, 149, 153, 51, 210, 121, 58, 235, 200,
    125, 103, 197, 177, 184, 221, 181, 52, 19, 230, 242, 134, 109, 123, 31, 187,
    12, 111, 23, 238, 253, 36, 98, 66, 247, 117, 227, 133, 72, 169, 102, 41,
    105, 46, 190, 214, 194, 250, 199, 91, 202, 162, 142, 182, 183, 99, 144, 115
};


#ifdef STK_HASH_PROTECTION
/**
\brief   Генерация 64 битного хеша по алгоритму Пирсона
\param   [in]   data   Указатель на массив, по которому строится хеш
\param   [in]   len    Размер передаваемого массива
\return  Возвращается 64 битных хеш, сгенерированный по алгоритму Пирсона
*/
static Hash getHash(const ui8* data, ui32 len)
{
    Hash hash = 0;
    ui8 h = 0;
    ui8* hPtr = (ui8*)&hash;
    for (int j = 0; j < 8; j++)
    {
        h = T[(data[0] + j) % 256];
        for (int i = 0; i < len; i++)
            h = T[h^data[i]];
        hPtr[j] = h;
    }
    return hash;
}
#endif


/**
\brief   Функция изменения размера стека
\param   [in]   stk       Указатель на структура стека
\param   [in]   newSize   Новый размер выделенной памяти
\return  Возвращается 0 если все ok, в противном случае код ошибки.
*/
static StackError stackResize(void* stk, ui32 newSize = 1)
{
    Assert_c(stk);
    if (!stk)
        return STK_ERROR_NULL_PTR;


    _BaseStack* stack = (_BaseStack*)stk;

    if (newSize == 0)
    {
        stack->capacity = newSize;
        return STK_OK;
    }

    char* ptr = (char*)stack->data;
    if (ptr == NULL)
    {
        ptr = (char*)calloc(newSize*stack->elementSize + 2 * STK_CANARY_SIZE,sizeof(ui8));
        Assert_c(ptr);
        if (!ptr)
            return STK_ERROR_OUT_OF_MEMORY;
    }
    else
    {
        ptr -= STK_CANARY_SIZE;
        ptr = (char*)realloc(ptr, newSize*stack->elementSize + 2 * STK_CANARY_SIZE);
        Assert_c(ptr);
        if (!ptr)
            return STK_ERROR_OUT_OF_MEMORY;
    }

    stack->data = ptr + STK_CANARY_SIZE;

    #ifdef STK_CANARY_PROTECTION
        *((CanaryType*)ptr) = STK_CANARY_VALUE;
        ptr += STK_CANARY_SIZE + newSize*stack->elementSize;
        *((CanaryType*)ptr) = STK_CANARY_VALUE;
    #endif
    stack->capacity = newSize;
    return STK_OK;
}

#ifdef STK_HASH_PROTECTION
/**
\brief   Функция вычисления хешей стека
\param   [in]   stk   Указатель на стуктуру стека
*/
static inline void recalcHashes(void* stk)
{
    Assert_c(stk);
    if (!stk)
        return;

    _BaseStack* stack = (_BaseStack*)stk;
    stack->dataHash = getHash((ui8*)stack->data - STK_CANARY_SIZE, stack->capacity*stack->elementSize + STK_CANARY_SIZE);
    stack->structHash = getHash((ui8*)&stack->dataHash, (ui8*)&stack->rightSide - (ui8*)&stack->dataHash + STK_CANARY_SIZE);
}
#endif


/**
\brief   Функция, инициализирующая стек начальными данными
\param   [in]   stk            Указатель на структуру стека
\param   [in]   capacity       Емкость нового стека
\param   [in]   elementsSize   Размер одного элемента стека(в байтах)
\return  Возвращается 0 если все ok, в противном случае код ошибки.
*/
StackError _stackInit(void* stk, const ui32 capacity, const ui32 elementSize)
{
    Assert_c(stk);
    if (!stk)
        return STK_ERROR_NULL_PTR;
    StackError errorCode = STK_OK;

    _BaseStack* stack = (_BaseStack*)stk;
    stack->elementSize = elementSize;
    stack->capacity = capacity;
    stack->size = 0;
    stack->data = NULL;
    errorCode = stackResize(stack, capacity);
    Assert_c(!errorCode);
    if (errorCode)
        return errorCode;
    ON_STK_HASH_PROTECTION(recalcHashes(stack));
}


/**
\brief   Валидатор стека
\param   [in]   stk   Указатель на структуру стека
\return  Возвращается 0 если все ok, в противном случае код ошибки.
*/
StackError _stackValidity(const void* stk, const dbgCallInfo dbgInfo)
{
    Assert_c(stk);
    if (!stk)
        return STK_ERROR_NULL_PTR;

    _BaseStack* stack = (_BaseStack*)stk;
    Assert_c(stack->data);
    if (!stack->data)
        return STK_ERROR_INVALID_PTR;

    if (stack->capacity < stack->size)
    {
        _stackDump(stk, dbgInfo);
        Assert_c(!"Capacity less than size!\n");
        return STK_ERROR_OUT_OF_RANGE;
    }


    #ifdef STK_CANARY_PROTECTION
        if (stack->leftSide != STK_CANARY_VALUE)
        {
            _stackDump(stk, dbgInfo);
            Assert_c(!"Somebody attack us from left side!\n");
            return STK_ERROR_ATTACK;
        }
        if (stack->rightSide != STK_CANARY_VALUE)
        {
            _stackDump(stk, dbgInfo);
            Assert_c(!"Somebody attack us from right side!\n");
            return STK_ERROR_ATTACK;
        }
    #endif

    #ifdef STK_HASH_PROTECTION
        Hash structHash = getHash((ui8*)&stack->dataHash, (ui8*)&stack->rightSide - (ui8*)&stack->dataHash + STK_CANARY_SIZE);
        if (structHash != stack->structHash)
        {
            _stackDump(stk, dbgInfo);
            Assert_c(!"Somebody has changed our stack structure!\n");
            return STK_ERROR_CHANGED_STRUCTURE;
        }

        Hash dataHash = getHash((ui8*)stack->data - STK_CANARY_SIZE, stack->capacity*stack->elementSize + STK_CANARY_SIZE);
        if (dataHash != stack->dataHash)
        {
            _stackDump(stk, dbgInfo);
            Assert_c(!"Somebody has changed our data in stack!\n");
            return STK_ERROR_CHANGED_DATA;
        }
    #endif

    #ifdef STK_CANARY_PROTECTION
        CanaryType* ptr = (CanaryType*)stack->data;
        ptr--;
        if (*ptr != STK_CANARY_VALUE)
        {
            _stackDump(stk, dbgInfo);
            Assert_c(!"Somebody attack us from left side!\n");
            return STK_ERROR_ATTACK;
        }

        ptr = (CanaryType*)((ui8*)stack->data + stack->capacity*stack->elementSize);
        if ( *ptr != STK_CANARY_VALUE)
        {
            _stackDump(stk, dbgInfo);
            Assert_c(!"Somebody attack us from right side!\n");
            return STK_ERROR_ATTACK;
        }

        ptr = (CanaryType*)stack->data;
        ptr--;
    #endif

    return STK_OK;
}

/**
\brief   Функция, записывающая на вершину стека данные
\param   [in]   stk   Указатель на структуру стека
\param   [in]   value Указатель на данные, которые будем записывать
\return  Возвращается 0 если все ok, в противном случае код ошибки.
*/

StackError _stackPush(void* stk, void* value, const dbgCallInfo dbgInfo)
{
    StackError errorCode = STK_OK;
    errorCode = _stackValidity(stk);
    if (errorCode)
        return errorCode;
    if (!value)
    {
        Assert_c(!"Haha nice try, but there is checking the null ptr!\n");
        return STK_ERROR_NULL_PTR;
    }

    _BaseStack* stack = (_BaseStack*)stk;
    if (stack->size == stack->capacity)
        stackResize(stack,stack->capacity+STK_BUFFER_ADDITION);

    char* addr = (char*)stack->data;
    addr += stack->elementSize * stack->size;
    memcpy(addr, value, stack->elementSize);
    stack->size++;

    ON_STK_HASH_PROTECTION(recalcHashes(stk));
    errorCode = _stackValidity(stk);
    if (errorCode)
        return errorCode;

    return STK_OK;
}


/**
\brief   Функция, вытаскивающая из стека данные
\param   [in]   stk   Указатель на структуру стека
\param   [in]   dest  Указатель на память, куда будет выгружаться элемент
\return  Возвращается 0 если все ok, в противном случае код ошибки.
\note    Если в стеке нет элементов, то функция возвращает код ошибки STK_ERROR_STK_IS_EMPTY
*/
StackError _stackPop(void* stk, void* dest, const dbgCallInfo dbgInfo)
{
    StackError errorCode = STK_OK;
    errorCode = _stackValidity(stk);
    if (errorCode)
        return errorCode;

    Assert_c(dest);
    if (!dest)
        return STK_ERROR_INVALID_PTR;

    _BaseStack* stack = (_BaseStack*)stk;
    if (!stack->size)
        return STK_ERROR_STK_IS_EMPTY;

    stack->size--;
    char* addr = (char*)stack->data;
    addr += stack->elementSize * stack->size;
    memcpy(dest, addr, stack->elementSize);
    memset(addr, 0, stack->elementSize);

    #ifdef STK_AUTO_REALLOC
        if (stack->capacity - stack->size > 2 * STK_BUFFER_ADDITION)
            errorCode = stackResize(stack, stack->capacity - STK_BUFFER_ADDITION);
    #endif

    ON_STK_HASH_PROTECTION(recalcHashes(stk));

    if (errorCode)
        return errorCode;


    errorCode = _stackValidity(stk);
    if (errorCode)
        return errorCode;

    return STK_OK;
}


/**
\brief   Функция, уничтожающая стек
\param   [in]   stk   Указатель на структуру стека
*/
void _stackDest(void* stk)
{
    Assert_c(stk);
    if (!stk)
        return;
    
    _BaseStack* stack = (_BaseStack*)stk;
    ui8* ptr = (ui8*)stack->data;
    ptr -= STK_CANARY_SIZE;
    if (stack->capacity != -1)
    {
        memset(ptr, 0, 2 * STK_CANARY_SIZE + stack->capacity*stack->elementSize);
        free(ptr);
    }
}

/**
\brief   Функция дампа, полностью печатает иформацию о стеке.
\detail  Функция дампа, выводит полную информацию о стеке, включая адреса полей стека и их значения.
         Также вывовдит информацию откуда данный дамп был вызван и какое именно имя переменной
         было переданно в функцию dump(...).
\param   [in]   stk            Указатель на структуру стека
\param   [in]   file           Строка, содержащая имя файла, из которого был вызван дамп (в макросе подставляется __FILE__)
\param   [in]   func           Строка, содержащая имя функции, из которой был вызван дамп (в макросе подставляется __FUNCSIG__ vs2017)
\param   [in]   line           Номер строки, из которой был вызван дамп (в макросе подставляется __LINE__)
\param   [in]   variableName   Имя переменной, которая передается в дамп
*/
void _stackDump(const void* stk, const dbgCallInfo dbgInfo,FILE* outStream)
{
    if (!stk)
    {
        Assert_c(!"stk pointer contain NULL!\n");
        return;
    }

    if(!outStream)
        outStream = logger.getStream();
    if (!outStream)
        return;

    _BaseStack* stack = (_BaseStack*)stk;
    ui32 elementSize = stack->elementSize;
    ui32 capacity = stack->capacity;
    ui32 size = stack->size;
    char* ptrData = (char*)stack->data;
    

    fprintf(outStream,"Dump has called from\nfile:%s\nfunction:%s\nline: %d\n", dbgInfo.file, dbgInfo.func, dbgInfo.line);
    fprintf(outStream,"Stack[0x%X] (variable name: %s) {\n", (char*)stk, dbgInfo.varName);
    #ifdef STK_CANARY_PROTECTION
        fprintf(outStream,"   canary{\n");
        fprintf(outStream,"       leftSide [0x%X] = 0x%llX\n", &stack->leftSide, stack->leftSide);
        fprintf(outStream,"       rightSide[0x%X] = 0x%llX\n", &stack->rightSide, stack->rightSide);
        fprintf(outStream,"   }\n");
    #endif
    #ifdef STK_HASH_PROTECTION
        fprintf(outStream,"   structHash   [0x%X] = 0x%llX\n", &stack->structHash, stack->structHash);
        fprintf(outStream,"   dataHash     [0x%X] = 0x%llX\n", &stack->dataHash, stack->dataHash);
    #endif
    fprintf(outStream,"   elementSize  [0x%X] = %d\n", &stack->elementSize, stack->elementSize);
    fprintf(outStream,"   size         [0x%X] = %d\n", &stack->size, stack->size);
    fprintf(outStream,"   capacity     [0x%X] = %d\n", &stack->capacity, stack->capacity);
    fprintf(outStream,"   data         [0x%X] = 0x%X\n", &stack->data, stack->data);
    fprintf(outStream,"   {\n");
    #ifdef STK_CANARY_PROTECTION
        fprintf(outStream,"       canary{\n");
        fprintf(outStream,"           leftSide[0x%X] = 0x%llX\n", ptrData - sizeof(CanaryType),   *((CanaryType*)(ptrData - sizeof(CanaryType))) );
        fprintf(outStream,"           leftSide[0x%X] = 0x%llX\n", ptrData + capacity*elementSize, *((CanaryType*)(ptrData + capacity*elementSize)));
        fprintf(outStream,"       }\n");
    #endif
       
    #ifdef STK_LIST_FORMAT_PRINT_DATA
    for (int i = 0; i < size; i++)
    {
        fprintf(outStream,"        ");
        if (i < size)
            fprintf(outStream,"*");
        else
            fprintf(outStream," ");
        fprintf(outStream,"data[%d] = 0x",i);
        for (int j = 0; j < elementSize; j++)
            fprintf(outStream,"%X", *((char*)stack->data + i*elementSize + j) & 0xFF);
        fprintf(outStream,"\n");
    }
    #endif


    #ifdef STK_TABLE_FORMAT_PRINT_DATA
    for (int i = 0; i < size / 16 + 1; i++)
    {
        fprintf(outStream, "        ");
        for (int j = 0; j < 16; j++)
        {
            if(i * 16 + j < size)
                fprintf(outStream, "0x%02X ", *((char*)stack->data + i*16 + j) & 0xFF);
        }
        fprintf(outStream, "\n");
    }
    #endif

    union
    {
        float fvalue;
        ui32 ivalue;
        ui8 arr[4];
    }number;


    #define STK_FLOAT_FORMAT_PRINT_DATA



    #ifdef STK_FLOAT_FORMAT_PRINT_DATA
        for (int i = 0; i < 4*16 ; i+=4)
        {
            for (ui8 j = 0; j < 4; j++)
                number.arr[j] = *((char*)stack->data + size - 4 - i + j) & 0xFF;
            fprintf(outStream, "        [0x%X]:0x%08X (%f)\n", size - 4 - i,number.ivalue, number.fvalue);
        }
    #endif

    
    fprintf(outStream,"   }\n");
    fprintf(outStream,"}\n");

}