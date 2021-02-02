#include "Asm.h"
#include "AsmTables.h"
#include <ctype.h>


using namespace Assembler;

/*
\brief Описываем типы лексем
*/
enum LexemaType
{
    LEX_ERROR = -1,
    LEX_COMMAND = 1,
    LEX_REGISTER = 2,
    LEX_NUMBER = 3,
    LEX_LABEL = 4,
    LEX_MEMORY = 5,
    LEX_MEM_BY_REG = 6
};

/*
\brief Структура, для работы с метками
*/
struct Label
{
    char name[32];
    ui32 pos;
};


/**
\brief  Функция переводит все буквы в строке в нижний регистр
\param  [in]  str  Входящая строка
*/
inline void toLowerStr(char* str)
{
    if (!str)
        return;
    while (*str)
    {
        *str = tolower(*str);
        str++;
    }
}

void prepareCompilatorsTable()
{
    for (int i = 0; i < COMMAND_TABLE_SIZE; i++)
    {
        if (!islower(commandTable[i].command[0]))
            toLowerStr((char*)commandTable[i].command);
    }
}

Compilator::Compilator()
{
    prepareCompilatorsTable();
}



/*
\brief Функция проверяет, является ли строка числом
\param [in]  str  Входящая строка
\return true, если строка содержит шестнадцатиричное число или просто число,
        в противном случае возвращается false.
*/
static inline bool isNumber(const char* str)
{
    Assert_c(str);
    if (!str)
        return false;

    if (!isdigit(*str) && *str != '-')
        return false;

    if (strstr(str, "0x"))
        return true;

    if ((strstr(str, ".")))
        return true;

    while (*str)
    {
        if (!isdigit(*str))
            return false;
        str++;
    }
    return true;
}

/*
\brief  Функция по строке определяет тип лексемы
\param  [in]  str  Входящая строка
\return Тип лексемы, записанной в строке
*/
static LexemaType getLexemaType(const char* str)
{
    static char buffer[32] = {};
    Assert_c(str);
    if (!str)
        return LEX_ERROR;

    if (strchr(str, ':'))
        return LEX_LABEL;

    if (isNumber(str))
        return LEX_NUMBER;

    for (int i = 0; i < COMMAND_TABLE_SIZE; i++)
        if (!strcmp(str, commandTable[i].command))
            return LEX_COMMAND;

    LexemaType result = LEX_ERROR;
    if (strchr(str, '[') && strchr(str, ']'))
    {
        sscanf(str, "[%s]", buffer);
        strchr(buffer, ']')[0] = 0;
        str = buffer;
        result = LEX_MEMORY;
    }

    for (int i = 0; i < REGISTER_TABLE_SIZE; i++)
        if (!strcmp(str, registerTable[i].command))
        {
            result = result == LEX_ERROR ? LEX_REGISTER : LEX_MEM_BY_REG;
            break;
        }


    return result;
}

/*
\brief  Функция генерит таблицу меток
\param  [in]  codeLine  Код программы на ассемблере
\param  [in]  lables    Указатель на таблицу с метками
\return Возвращает предполагаемый размер скомпилированного файла в
        байтах. В случае ошибки возвращается константа ASM_ERROR_CODE
*/
static int genLableTable(const char* codeLine, Label* lables)
{
    Assert_c(codeLine);
    Assert_c(lables);
    if (!codeLine || !lables)
        return ASM_ERROR_CODE;

    static char buf[32] = {};
    ui32 nBytes = 0;
    LexemaType lxt;
    while (*codeLine)
    {
        sscanf(codeLine, "%s ", buf);
        codeLine += strlen(buf) + 1;
        lxt = getLexemaType(buf);
        switch (lxt)
        {
        case LEX_COMMAND:
            nBytes += sizeof(Mcode);
            break;
        case LEX_LABEL:
            lables->pos = nBytes;
            memcpy(lables->name, buf, 32);
            *(strchr(lables->name, ':')) = 0;
            lables++;
            break;
        case LEX_NUMBER:
            nBytes += sizeof(ui32);
            break;
        case LEX_REGISTER:
            nBytes++;
            break;
        case LEX_MEMORY:
            nBytes += sizeof(ui32);
            break;
        case LEX_MEM_BY_REG:
            nBytes += sizeof(ui8);
            break;
        default:
            nBytes += sizeof(ui32);
            break;
        }
    }
    return nBytes;
}

/*
\brief  Функция выполняет поиск метки в таблице по имени
\param  [in]  labelName  Имя метки
\param  [in]  lables     Таблица с метками
\param  [in]  nLables    Количество меток в таблице
\return Индекс метки в таблице, в случае ошибки возвращается ASM_ERROR_CODE
*/
static int getLabel(char* labelName, Label* lables, ui32 nLables)
{
    Assert_c(labelName);
    Assert_c(lables);
    if (!labelName || !lables)
        return ASM_ERROR_CODE;

    for (int i = 0; i < nLables; i++)
        if (!strcmp(lables[i].name, labelName))
            return i;
    return ASM_ERROR_CODE;
}


/*
\brief   Функция парсит лексему из строки
\details Функция вытаскивает лексему из строки, переводя указатель на новую лексему.
         Затем происходит опеределение полученной лексемы.
         В зависимости от типа будет возвращает указатель на буффер, содержащий подробную
         информацию о лексеме.
\param  [in,out]  ptrCodeLine  Указатель на строку с кодом
\param  [in,out]  lxt          Указатель на структуру LexemaType
\return Указатель на буфер, с подробной информацией о лексеме. В случае ошибки возвращается NULL
        Например, если лексема является числом, то возвращается указатель на ui32, в котором лежит именно это число.
        Если лексема была командой или регистром, то возвращается указатель на ui32, в котором лежит машинный код.
\note   Важно отметить, что после выполнения данной функции значение указателя (*ptrCodeLine) изменяется!
*/
static inline void* readNextLexema(char** ptrCodeLine, LexemaType* lxt)
{
    Assert_c(ptrCodeLine);
    Assert_c(lxt);
    if (!ptrCodeLine || !lxt)
        return NULL;
    Assert_c(*ptrCodeLine);
    if (!*ptrCodeLine)
        return NULL;

    static char buffer[32] = {};
    static ui32 bufInt = 0;
    sscanf(*ptrCodeLine, "%s ", buffer);
    *ptrCodeLine = strchr(*ptrCodeLine, ' ') + 1;

    *lxt = getLexemaType(buffer);

    if (*lxt == LEX_COMMAND)
        for (int i = 0; i < COMMAND_TABLE_SIZE; i++)
            if (!strcmp(buffer, commandTable[i].command))
            {
                bufInt = commandTable[i].machineCode;
                return &bufInt;
            }
    if (*lxt == LEX_REGISTER)
        for (int i = 0; i < REGISTER_TABLE_SIZE; i++)
            if (!strcmp(buffer, registerTable[i].command))
            {
                bufInt = registerTable[i].machineCode;
                return &bufInt;
            }
    if (*lxt == LEX_NUMBER)
    {
        if (strchr(buffer, 'x'))
        {
            sscanf(buffer, "0x%X", &bufInt);
            return &bufInt;
        }
        if (strchr(buffer, '.'))
        {
            sscanf(buffer, "%f", &bufInt);
            return &bufInt;
        }
        else
        {
            sscanf(buffer, "%d", &bufInt);
            return &bufInt;
        }
    }

    return &buffer;
}

/*
\brief  Функция парсит операнд, если его лексема имела тип LEX_MEMORY или LEX_MEM_BY_REG
\param  [in]  str       Указатель на строку с лексемой
\param  [in]  lables    Указатель на таблицу с метками
\param  [in]  nLables   Количество меток в таблице
\return В зависимости от типа лексемы будет возвращается следующие величины:
        Если тип лексемы LEX_MEMORY, то возвращается число, равное семещению регистра eip
        Если тип лексемы LEX_MEM_BY_REG, то возвращается код регистра
        В случае ошибки возвращается константа ASM_ERROR_CODE.
*/
static int getMemoryOperand(char* str, Label* lables, ui32 nLables)
{
    Assert_c(str);
    if (!str)
        return ASM_ERROR_CODE;
    if (str[0] != '[')
        return ASM_ERROR_CODE;
    str++;

    Assert_c(lables);
    if (!lables)
        return ASM_ERROR_CODE;

    char* endStr = strchr(str, ']');
    Assert_c(endStr);
    if (!endStr)
        return ASM_ERROR_CODE;
    *endStr = 0;

    int resOperand = 0;

    if (isNumber(str))
    {
        if (strchr(str, 'x'))
            sscanf(str, "0x%X", &resOperand);
        else
            sscanf(str, "%d", &resOperand);
        return resOperand;
    }

    resOperand = getLabel(str, lables, nLables);
    if (resOperand != ASM_ERROR_CODE)
        return lables[resOperand].pos;

    for (int i = 0; i < REGISTER_TABLE_SIZE; i++)
        if (!strcmp(str, registerTable[i].command))
            return registerTable[i].machineCode;

    return ASM_ERROR_CODE;
}


/*
\brief  Функция проверяет, что сгенерированный машинный код, допускает указанные типы операндов
\param  [in]  cmd  Структура с командой
\return Код ошибки. Если команда сгенерирована правильно, то
        возвращается ASM_OK
*/
static AsmError checkValidityOfOperands(Command cmd)
{
    char* validityStr[2] = { NULL, NULL };
    for (int i = 0; i < COMMAND_TABLE_SIZE; i++)
        if (commandTable[i].machineCode >> 8 == cmd.code.bits.opCode)
        {
            validityStr[0] = commandTable[i].validFirstOperand;
            validityStr[1] = commandTable[i].validSecondOperand;
        }

    if (validityStr[0] == NULL)
        return ASM_ERROR_INVALID_MACHINE_CODE;

    const char* opTypeStr = "RNMB";
    OperandType opType;
    for (int i = 0; i < cmd.code.bits.nOperands; i++)
    {
        opType = getOperandType(cmd, i);
        if (!strchr(validityStr[i], opTypeStr[(ui8)opType]))
            return ASM_ERROR_INVALID_OPERAND_TYPE_FOR_COMMAND;
    }
    return ASM_OK;
}

/*
\brief  Функция генерит машинный код
\param  [in]     codeLine  Строка
\param  [in]     lables    Таблица с метками
\param  [in]     nLables   Количество меток в таблице
\param  [in,out] ptrBytes  Указатель на массив байтов, куда будем записывать результат
\return Код ошибки. В случае успеха возвращается ASM_OK.
*/
static AsmError genBytes(const char* codeLine, Label* lables, ui32 nLables, ui8* ptrBytes)
{
    Assert_c(codeLine);
    Assert_c(lables);
    Assert_c(ptrBytes);
    if (!codeLine || !lables || !ptrBytes)
        return ASM_ERROR_INVALID_INPUT_DATA;


    char* ptr = NULL;
    ui8* bytes = ptrBytes;
    int currentPosition = 0;
    LexemaType lxt;
    Command cmd;
    while (*codeLine)
    {
        ptr = (char*)readNextLexema((char**)&codeLine, &lxt);
        if (lxt == LEX_LABEL)
            continue;
        if (lxt != LEX_COMMAND)
        {
            logger("Compilator error", "Lexema: \"%s\" is not command, but should be...", ptr);
            return ASM_ERROR_INVALID_SYNTAX;
        }
        if (!ptr)
        {
            logger("Compilator error", "We can't read lexema.\n");
            return ASM_ERROR_CANT_READ_LEXEMA;
        }

        cmd.code.marchCode = *((Mcode*)ptr);
        currentPosition += sizeof(Mcode);
        for (int i = 0; i < cmd.code.bits.nOperands; i++)
        {
            ptr = (char*)readNextLexema((char**)&codeLine, &lxt);
            if (!ptr)
            {
                logger("Compilator error", "We can't read lexema.\n");
                return ASM_ERROR_CANT_READ_LEXEMA;
            }
            if (lxt == LEX_NUMBER)
            {
                cmd.operand[i].ivalue = *((ui32*)ptr);
                setOperandType(cmd, i, OPERAND_NUMBER);
                currentPosition += sizeof(ui32);
            }
            if (lxt == LEX_REGISTER)
            {
                cmd.operand[i].ivalue = *((ui8*)ptr);
                setOperandType(cmd, i, OPERAND_REGISTER);
                currentPosition += sizeof(ui8);
            }
            if (lxt == LEX_MEMORY || lxt == LEX_MEM_BY_REG)
            {
                cmd.operand[i].ivalue = getMemoryOperand(ptr, lables, nLables);
                if (cmd.operand[i].ivalue == ASM_ERROR_CODE)
                {
                    logger("Compilator error", "Invalid link to memory: \"%s\" ", ptr);
                    return ASM_ERROR_INVALID_OPERAND_SYNTAX;
                }
                setOperandType(cmd, i, lxt == LEX_MEMORY ? OPERAND_MEMORY : OPERAND_MEM_BY_REG);
                currentPosition += lxt == LEX_MEMORY ? sizeof(ui32) : sizeof(ui8);
            }
            if (lxt == LEX_LABEL)
            {
                logger("Compilator error", "Invalid operand: \"%s\" ", ptr);
                return ASM_ERROR_INVALID_OPERAND_SYNTAX;
            }
            if (lxt == LEX_ERROR)
            {
                int labelIndex = getLabel(ptr, lables, nLables);
                if (labelIndex == -1)
                {
                    logger("Compilator error", "Invalid lexema: \"%s\" ", ptr);
                    return ASM_ERROR_INVALID_SYNTAX;
                }
                setOperandType(cmd, i, OPERAND_NUMBER);
                currentPosition += sizeof(ui32);
                cmd.operand[i].ivalue = lables[labelIndex].pos;
            }

        }

        *((Mcode*)bytes) = cmd.code.marchCode;
        bytes += sizeof(Mcode);


        AsmError errorCode = checkValidityOfOperands(cmd);
        if (errorCode == ASM_ERROR_INVALID_OPERAND_TYPE_FOR_COMMAND)
        {
            logger("Compilator error", "Invalid type of operand for current command.");
            Disassembler::Instance().disasmCommand(cmd, getLoggerStream());
            return ASM_ERROR_INVALID_OPERAND_TYPE_FOR_COMMAND;
        }
        if (errorCode == ASM_ERROR_INVALID_MACHINE_CODE)
        {
            logger("Compilator error", "Has been generated invalid machine code: 0x%X", cmd.code.marchCode);
            return ASM_ERROR_INVALID_MACHINE_CODE;
        }


        for (int i = 0; i < cmd.code.bits.nOperands; i++)
        {
            OperandType opType = getOperandType(cmd, i);
            if (opType == OPERAND_NUMBER || opType == OPERAND_MEMORY)
            {
                *((ui32*)bytes) = cmd.operand[i].ivalue;
                bytes += sizeof(ui32);
            }
            if (opType == OPERAND_REGISTER || opType == OPERAND_MEM_BY_REG)
            {
                *((ui8*)bytes) = cmd.operand[i].ivalue;
                bytes += sizeof(ui8);
            }
        }
    }
    return ASM_OK;
}

/*
\brief  Функция записывает сгенерированный машинный код в поток вывода
\param  [in]  bytes      Указатель на массив байтов
\param  [in]  nBytes     Количество байтов в массиве
\param  [in]  outStream  Поток вывода
*/
static AsmError throwMachineCodeIntoStream(ui8* bytes, ui32 nBytes, FILE* outStream = stdout)
{
    Assert_c(bytes);
    Assert_c(outStream);
    if (!bytes || !outStream)
        return ASM_ERROR_INVALID_INPUT_DATA;
    if (ferror(outStream))
    {
        logger("Access error", "In function %s. There are problems with file stream.", __FUNCSIG__);
        return ASM_ERROR_CANT_WRITE_INTO_FILE;
    }

    if (outStream == stdout)
    {
        for (int i = 0; i < nBytes; i++)
        {
            printf("0x%02X ", bytes[i] & 0xFF);
            if ((i + 1) % 16 == 0)
                printf("\n");
        }
    }
    else
        fwrite(bytes, sizeof(ui8), nBytes, outStream);

    return ASM_OK;
}



AsmError Compilator::compile(const char* code, FILE* outStream)
{
    Assert_c(code);
    Assert_c(outStream);
    if (!code || !outStream)
        return ASM_ERROR_INVALID_INPUT_DATA;
    if (ferror(outStream))
        return ASM_ERROR_CANT_WRITE_INTO_FILE;


    //Из строки с исходным кодом программы удаляем все лишние символы,
    //оставляя только лексемы языка. В качестве разделителя используем пробел.
    C_string codeLine = NULL;
    ui32 nLabels = 0;
    {
        int strLen = strlen(code);
        codeLine = (C_string)calloc(strLen + 2, sizeof(ui8));
        Assert_c(codeLine);
        if (!codeLine)
            return ASM_ERROR_OUT_OF_MEMORY;
        ui8 c[2] = {};
        bool pingpong = 0;
        int index = 0;
        bool isComment = 0;
        for (int i = 0; i < strLen; i++)
        {
            c[pingpong] = code[i];
            if (c[pingpong] == ':')
                nLabels++;

            if (c[pingpong] == ';')
            {
                while (i < strLen && code[i] != '\n')
                    i++;
                if (i == strLen)
                    break;
                c[pingpong] = code[i];
            }

            #define isValidChr(x) (isalpha(x) || isdigit(x) || strchr(":_[].-;" , x) && x )

            if (!isValidChr(c[pingpong]) && !isValidChr(c[pingpong ^ 1]))
                continue;
            else if (isValidChr(code[i]))
                codeLine[index] = code[i];
            else
                codeLine[index] = ' ';
            index++;
            #undef isValidChr(x)

            pingpong ^= 1;
        }
        if (codeLine[index - 1] != ' ')
            codeLine[index++] = ' ';
        codeLine[index++] = 0;
        codeLine = (C_string)realloc(codeLine, index * sizeof(ui8));
        Assert_c(codeLine);
        if (!codeLine)
            return ASM_ERROR_OUT_OF_MEMORY;
    }



    // Генерируем таблицу меток
    Label* lables = (Label*)calloc(nLabels, sizeof(Label));
    Assert_c(lables);
    if (!lables)
    {
        free(codeLine);
        return ASM_ERROR_OUT_OF_MEMORY;
    }
    ui32 nBytes = genLableTable(codeLine, lables);
    Assert_c(nBytes != ASM_ERROR_CODE);
    if (nBytes == ASM_ERROR_CODE)
    {
        free(codeLine);
        free(lables);
        return ASM_ERROR_GEN_LABLE_TABLE;
    }
    //выделяем память для проги
    ui8* bytes = (ui8*)calloc(nBytes + 1, sizeof(ui8));
    Assert_c(bytes);
    if (!bytes)
    {
        free(codeLine);
        free(lables);
        return ASM_ERROR_OUT_OF_MEMORY;
    }

    AsmError errorCode = ASM_OK;

    //производим компиляцию
    errorCode = genBytes(codeLine, lables, nLabels, bytes);
    if (errorCode != ASM_OK)
    {
        free(codeLine);
        free(lables);
        free(bytes);
        return ASM_ERROR_GEN_MACHINE_CODE;
    }

    //и выводим результат компиляции в поток вывода
    errorCode = throwMachineCodeIntoStream(bytes, nBytes, outStream);

    free(codeLine);
    free(lables);
    free(bytes);

    if (errorCode != ASM_OK)
        return ASM_ERROR_CANT_WRITE_INTO_FILE;

    return ASM_OK;
}