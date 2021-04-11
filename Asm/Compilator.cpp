#include "Asm.h"
#include "AsmTables.h"
#include <ctype.h>
#include <math.h>


using namespace Assembler;

/*
\brief Описываем типы лексем
*/
enum LexemaType
{
    LEX_ERROR = -1,
    LEX_COMMAND = 1,
    LEX_REGISTER,
    LEX_NUMBER,
    LEX_LABEL,
    LEX_MEMORY,
    LEX_MEM_BY_REG,
    LEX_MEM_BY_REG_LONG,
    LEX_VARIABLE
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
        if (!isdigit(*str) && *str != '-')
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
    if (strchr(str, '@'))
        return LEX_VARIABLE;

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

    char* divider = (char*)strchr(str, '+');
    divider = !divider ? (char*)strchr(str, '-') : divider;
    bool isSum = divider && divider[0] == '+';
    if (divider) divider[0] = 0;
    
    for (int i = 0; i < REGISTER_TABLE_SIZE; i++)
        if (!strcmp(str, registerTable[i].command))
        {
            result = result == LEX_ERROR ? LEX_REGISTER : LEX_MEM_BY_REG;
            if (divider && result == LEX_MEM_BY_REG)
            {
                result = LEX_MEM_BY_REG_LONG;
                divider[0] = isSum ? '+' : '-';
            }
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
    ui32 variableSize = 0;
    ui8 sizeFactor = 0;
    while (*codeLine)
    {
        sscanf(codeLine, "%s ", buf);
        codeLine += strlen(buf) + 1;

        if (!memcmp(buf, ".text", 5))
            continue;
        if (!memcmp(buf, ".data", 5))
            continue;

        lxt = getLexemaType(buf);
        switch (lxt)
        {
        
        case LEX_LABEL:
            lables->pos = nBytes;
            memcpy(lables->name, buf, 32);
            *(strchr(lables->name, ':')) = 0;
            lables++;
            break;
        case LEX_VARIABLE:
            lables->pos = nBytes;
            memcpy(lables->name, buf+1, 32);
            lables++;

            sscanf(codeLine, "%s ", buf);
            if (buf[1] == 'b') sizeFactor = 1;
            if (buf[1] == 'w') sizeFactor = 2;
            if (buf[1] == 'q') sizeFactor = 4;
            if (sizeFactor == 0) break;
            while (codeLine[0]!='a')
            {
                sscanf(codeLine, "%s ", buf);
                codeLine += strlen(buf) + 1;
            }
            if (memcmp("alloc", codeLine, 5))
            {
                logger.push("Syntax error", "Invalid variable declaration, sould be `alloc` after value.");
                return -1;
            }
            codeLine += 6;

            if (1 != sscanf(codeLine, "%d ", &variableSize))
            {
                logger.push("Syntax error", "Invalid number of bytes for variable");
                return -1;
            }
            if (variableSize <= 0)
            {
                logger.push("Syntax error", "Array size couldnt be negative!");
                return -1;
            }
            codeLine += ((int)floor(log10(variableSize)) + 1) + 1;
            variableSize *= sizeFactor;

            nBytes += variableSize;
            break;
        case LEX_COMMAND:         nBytes += sizeof(Mcode);              break;
        case LEX_NUMBER:          nBytes += sizeof(ui32);               break;
        case LEX_REGISTER:        nBytes += sizeof(ui8);                break;
        case LEX_MEMORY:          nBytes += sizeof(ui32);               break;
        case LEX_MEM_BY_REG:      nBytes += sizeof(ui8);                break;
        case LEX_MEM_BY_REG_LONG: nBytes += sizeof(ui8) + sizeof(ui32); break;
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

static inline ui32 getNumberFromString(const char* str)
{
    ui32 buffer = 0;
    if (strchr(str, 'x'))
    {
        sscanf(str, "0x%X", &buffer);
        return buffer;
    }
    if (strchr(str, '.'))
    {
        sscanf(str, "%f", &buffer);
        return buffer;
    }
    else
    {
        sscanf(str, "%d", &buffer);
        return buffer;
    }
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
        bufInt = getNumberFromString(buffer);
        return &bufInt;
    }

    return &buffer;
}

/*
\brief  Функция парсит операнд, если его лексема имела тип LEX_MEMORY или LEX_MEM_BY_REG
\param  [in]  str       Указатель на строку с лексемой
\param  [in]  lables    Указатель на таблицу с метками
\param  [in]  nLables   Количество меток в таблице
\param  [in,out] longMemoryOperand Указатель на буффер, куда будет записываться информация о смещении,
        если тип лексемы был LEX_MEM_BY_REG_LONG
\return В зависимости от типа лексемы будет возвращается следующие величины:
        Если тип лексемы LEX_MEMORY, то возвращается число, равное семещению регистра eip
        Если тип лексемы LEX_MEM_BY_REG, то возвращается код регистра
        В случае ошибки возвращается константа ASM_ERROR_CODE.
*/
static int getMemoryOperand(char* str, Label* lables, ui32 nLables,ui32* longMemoryOperand = nullptr)
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

    //проверяем, стоит ли число в скобках
    if (isNumber(str))
    {
        if (strchr(str, 'x'))
            sscanf(str, "0x%X", &resOperand);
        else
            sscanf(str, "%d", &resOperand);
        return resOperand;
    }

    //или это метка
    resOperand = getLabel(str, lables, nLables);
    if (resOperand != ASM_ERROR_CODE)
        return lables[resOperand].pos;

    //а может быть регистр
    for (int i = 0; i < REGISTER_TABLE_SIZE; i++)
    {
        if (!strcmp(str, registerTable[i].command))
            return registerTable[i].machineCode;
    }

    //и остается проверить только обращение long memory command
    if (strchr(str, '+') || strchr(str, '-'))
    {
        char* divider = strchr(str, '+');
        divider = !divider ? strchr(str, '-') : divider;
        bool isSum = divider[0] == '+';
        *(divider++) = 0;
        if (isNumber(divider))
        {
            for (int i = 0; i < REGISTER_TABLE_SIZE; i++)
                if (!strcmp(str, registerTable[i].command))
                {
                    if (!longMemoryOperand)
                    {
                        logger.push("Compilator error", "getMemoryOperand(...) longMemoryOperand contain NULL\n");
                        return ASM_ERROR_CODE;
                    }
                    
                    *longMemoryOperand = getNumberFromString(divider) * (isSum ? 1 : -1);
                    *(--divider) = isSum ? '+' : '-';
                    
                    return registerTable[i].machineCode;
                }
        }
        *(--divider) = isSum ? '+' : '-';
    }


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
    char* validityStr[3] = { NULL, NULL };
    for (int i = 0; i < COMMAND_TABLE_SIZE; i++)
        if (commandTable[i].machineCode >> 10 == cmd.bits.opCode)
        {
            validityStr[0] = commandTable[i].validFirstOperand;
            validityStr[1] = commandTable[i].validSecondOperand;
            validityStr[2] = commandTable[i].validThirdOperand;
        }

    if (validityStr[0] == NULL)
        return ASM_ERROR_INVALID_MACHINE_CODE;

    const char* opTypeStr = "RNMB";
    OperandType opType;
    for (int i = 0; i < cmd.bits.nOperands; i++)
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
    ui32 textSectionSize = 0;
    LexemaType lxt;
    Command cmd;
    enum {CODE_SECTION, DATA_SECTION} currentSection = CODE_SECTION;
    if (memcmp(".text", codeLine, 5))
    {
        logger.push("Compilator error",
            "First seection should be .text");
        return ASM_ERROR_INVALID_SYNTAX;
    }
    while (*codeLine)
    {
        if (!memcmp(".text", codeLine, 5))
        {
            codeLine += 6;
            continue;
        }
        if (!memcmp(".data", codeLine, 5))
        {
            textSectionSize = bytes - ptrBytes;
            codeLine += 6;
            currentSection = DATA_SECTION;
            continue;
        }
        
        
        ptr = (char*)readNextLexema((char**)&codeLine, &lxt);
        if (lxt == LEX_LABEL)
            continue;
        if (lxt == LEX_VARIABLE)
        {
            if (currentSection != DATA_SECTION)
            {
                logger.push("Compilator error",
                            "You try to create variable in .text section! "
                            "Try to declare it in .data section.");
                return ASM_ERROR_INVALID_SYNTAX;
            }
            const char* modifier = NULL;
            const char* dictionary = "bwq";
            if (codeLine[0] == 'd' && (modifier = strchr("bwq", codeLine[1])))
            {
                codeLine += 3;
                ui8 bytesForVariable = 1 << (modifier - dictionary);
                ui32 nElements = 0;
                do
                {
                    ptr = (char*)readNextLexema((char**)&codeLine, &lxt);
                    if (lxt != LEX_NUMBER && codeLine[0] != 'a')
                    {
                        logger.push("Compilator error", "Expected size of variable...");
                        return ASM_ERROR_INVALID_SYNTAX;
                    }
                    if (lxt == LEX_NUMBER)
                    {
                        if (bytesForVariable == 1) *(( ui8*)bytes) = *((ui32*)ptr);
                        if (bytesForVariable == 2) *((ui16*)bytes) = *((ui32*)ptr);
                        if (bytesForVariable == 4) *((ui32*)bytes) = *((ui32*)ptr);
                        bytes += bytesForVariable;
                        nElements++;
                    }
                    if (!memcmp(codeLine, "alloc", 5)) // codeLine == "alloc..."
                    {
                        codeLine += 6;
                        ui32 allocatedMemory = 0;
                        
                        sscanf(codeLine, "%d ", &allocatedMemory);
                        codeLine += ((int)floor(log10(allocatedMemory)) + 1) + 1;
                        if (allocatedMemory < nElements)
                        {
                            logger.push("Compilator error", "You allocate not enougth memory for your variable.");
                            return ASM_ERROR_INVALID_SYNTAX;
                        }
                        else
                        {
                            allocatedMemory -= nElements;
                            allocatedMemory *= bytesForVariable;
                            while (allocatedMemory--)
                            {
                                *((ui8*)bytes) = 0;
                                bytes += sizeof(ui8);
                            }
                            break;
                        }
                    }
                } while (true);
                continue;
            }
            else
            {
                logger.push("Compilator error", "Variable undefined variable specificator: %c%c", ptr[0],ptr[1]);
                return ASM_ERROR_INVALID_SYNTAX;
            }
        }
        if (lxt != LEX_COMMAND)
        {
            logger.push("Compilator error", "Lexema: \"%s\" is not command, but should be...", ptr);
            return ASM_ERROR_INVALID_SYNTAX;
        }
        if (!ptr)
        {
            logger.push("Compilator error", "We can't read lexema.\n");
            return ASM_ERROR_CANT_READ_LEXEMA;
        }

        if (lxt == LEX_COMMAND && currentSection != CODE_SECTION)
        {
            logger.push("Compilator error",
                "You try to write command in .data section! "
                "Try to declare it in .text section.");
            return ASM_ERROR_INVALID_SYNTAX;
        }

        cmd.bits.marchCode = *((Mcode*)ptr);
        currentPosition += sizeof(Mcode);
        for (int i = 0; i < cmd.bits.nOperands; i++)
        {
            ptr = (char*)readNextLexema((char**)&codeLine, &lxt);
            if (!ptr)
            {
                logger.push("Compilator error", "We can't read lexema.\n");
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
                    logger.push("Compilator error", "Invalid link to memory: \"%s\" ", ptr);
                    return ASM_ERROR_INVALID_OPERAND_SYNTAX;
                }
                setOperandType(cmd, i, lxt == LEX_MEMORY ? OPERAND_MEMORY : OPERAND_MEM_BY_REG);
                currentPosition += lxt == LEX_MEMORY ? sizeof(ui32) : sizeof(ui8);
            }
            if (lxt == LEX_MEM_BY_REG_LONG)
            {
                ui32 offsetOperand = 0;
                cmd.operand[i].ivalue = getMemoryOperand(ptr, lables, nLables, &offsetOperand);
                cmd.extend[i] = offsetOperand;
                cmd.bits.longCommand = 1;
                if (cmd.operand[i].ivalue == ASM_ERROR_CODE)
                {
                    logger.push("Compilator error", "Invalid link to memory: \"%s\" ", ptr);
                    return ASM_ERROR_INVALID_OPERAND_SYNTAX;
                }
                setOperandType(cmd, i, OPERAND_MEM_BY_REG);
                currentPosition += sizeof(ui8);
                currentPosition += sizeof(ui32);
            }
            if (lxt == LEX_LABEL)
            {
                logger.push("Compilator error", "Invalid operand: \"%s\" ", ptr);
                return ASM_ERROR_INVALID_OPERAND_SYNTAX;
            }
            if (lxt == LEX_ERROR)
            {
                int labelIndex = getLabel(ptr, lables, nLables);
                if (labelIndex == -1)
                {
                    logger.push("Compilator error", "Invalid lexema: \"%s\" ", ptr);
                    return ASM_ERROR_INVALID_SYNTAX;
                }
                setOperandType(cmd, i, OPERAND_NUMBER);
                currentPosition += sizeof(ui32);
                cmd.operand[i].ivalue = lables[labelIndex].pos;
            }

        }

        *((Mcode*)bytes) = cmd.bits.marchCode;
        bytes += sizeof(Mcode);


        AsmError errorCode = checkValidityOfOperands(cmd);
        if (errorCode == ASM_ERROR_INVALID_OPERAND_TYPE_FOR_COMMAND)
        {
            logger.push("Compilator error", "Invalid type of operand for current command.");
            Disassembler::disasmCommand(cmd, logger.getStream());
            return ASM_ERROR_INVALID_OPERAND_TYPE_FOR_COMMAND;
        }
        if (errorCode == ASM_ERROR_INVALID_MACHINE_CODE)
        {
            logger.push("Compilator error", "Has been generated invalid machine code: 0x%X", cmd.bits.marchCode);
            return ASM_ERROR_INVALID_MACHINE_CODE;
        }


        for (int i = 0; i < cmd.bits.nOperands; i++)
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
                if (cmd.bits.longCommand && opType == OPERAND_MEM_BY_REG)
                {
                    *((ui32*)bytes) = cmd.extend[i];
                    bytes += sizeof(ui32);
                }
            }
        }
    }
    if (currentSection == CODE_SECTION)
        textSectionSize = bytes - ptrBytes;
    *((ui32*)bytes) = textSectionSize;
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
        logger.push("Access error", "In function %s. There are problems with file stream.", __FUNCSIG__);
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

static char* delSpecialSymbols(const char* code, ui32& nLabels)
{
    int strLen = strlen(code);
    char* codeLine = (C_string)calloc(strLen + 2, sizeof(ui8));
    Assert_c(codeLine);
    if (!codeLine)
        return NULL;
    ui8 c[2] = {};
    bool pingpong = 0;
    int index = 0;
    bool isComment = 0;
    for (int i = 0; i < strLen; i++)
    {
        c[pingpong] = code[i];
        if (c[pingpong] == ':' || c[pingpong] == '@')
            nLabels++;

        if (c[pingpong] == ';')
        {
            while (i < strLen && code[i] != '\n')
                i++;
            if (i == strLen)
                break;
            c[pingpong] = code[i];
        }

        #define isValidChr(x) (isalpha(x) || isdigit(x) || strchr(":_[].-;+-@" , x) && x )

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
    return codeLine;
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
    //которые никак не используюся в ассемблере. Также удаляются дополнительные
    //пробелы, символы перевода строки, табуляции и т.д.
    ui32 nLabels = 0;
    C_string codeLine = delSpecialSymbols(code, nLabels);
    Assert_c(codeLine);
    if (!codeLine)
        return ASM_ERROR_OUT_OF_MEMORY;


    // Генерируем таблицу меток
    Label* lables = (Label*)calloc(nLabels, sizeof(Label));
    Assert_c(lables);
    if (!lables)
    {
        free(codeLine);
        return ASM_ERROR_OUT_OF_MEMORY;
    }

    ui32 nBytes = genLableTable(codeLine, lables);
    //резервируем место для записи в секцию .data размера секции .text
    nBytes += sizeof(ui32);
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