#include "LibStr.h"
#include "Logger.h"
#include "CallStack.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>


/**
\brief  Функция полностью сичтывает файл
\param  [in]      filename  Имя считываемого файла
\param  [in,out]  outString Указатель на считанную строку
\param  [in]      readBytesPtr  Указатель на unsigned, в котором будет храниться количество считанных байтов
\return В случае успеха возвращается количество прочитанных байт.
Если произошла ошибка, то возвращается константа -1.
*/
int readFullFile(const char* filename, char** outString)
{$
    assert(filename);
    assert(outString);
    if (!filename || !outString)
    {
        $$$("NULL ptr in filename or outString");
        return STANDART_ERROR_CODE;
    }

    FILE* inputFile = fopen(filename, "rb");
    assert(inputFile);
    if (!inputFile)
    {
        $$$("Can't read file");
        return STANDART_ERROR_CODE;
    }
    if (ferror(inputFile))
    {
        $$$("Stream related to the file has an error");
        return STANDART_ERROR_CODE;
    }

    fseek(inputFile, 0, SEEK_END);
    long fsize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    char* string = (char*)calloc(fsize + 8, sizeof(char));
    assert(string);
    if (!string)
    {
        $$$("Can't allocate memory for out string");
        return STANDART_ERROR_CODE;
    }

    unsigned nReadBytes = fread(string, sizeof(char), fsize, inputFile);
    fclose(inputFile);
    string[fsize] = 0;

    *outString = string;

    $$
    return nReadBytes;
}


/**
\brief  Функция удаляет из строки все спец символы, кроме тех, которые указаны в строке dontDelChar
\param  [in,out]      ptrStr  Указатель на строку в которой будем производить удаление
\param  [in,out]      dontDelChar Строка, задающая набор символов, которые не следует удалять
\return В случае успеха возвращается новая длина строки.
Если произошла ошибка, то возвращается константа ASM_ERROR_CODE.
\note   В новой строке будут только числа,буквы и символы из строки dontDelChar
*/
int removeExtraChar(char** ptrStr, const char* dontDelChar)
{
    $
    Assert_c(ptrStr);
    Assert_c(dontDelChar);
    if (!ptrStr || !dontDelChar)
    {
        $$$("NULL in ptrStr or dontDelChar");
        return STANDART_ERROR_CODE;
    }
    char* str = *ptrStr;
    Assert_c(str);
    if (!str)
    {
        $$$("NULL in *ptrStr");
        return STANDART_ERROR_CODE;
    }

    unsigned i = 0;
    unsigned j = 0;

    bool isStrChar = 0;
#define isValid(x) (isalpha(x) || isdigit(x) || strchr(dontDelChar , x) )
    while (str[j])
    {
        if (str[j] == '\"')
        {
            isStrChar ^= 1;
            j++;
            continue;
        }
        if (isValid(str[j]) || isStrChar)
        {
            str[i] = str[j];
            i++;
        }
        j++;
    }
    str[i++] = 0;
    i++;
#undef isValid
    str = (char*)realloc(str, i);
    Assert_c(str);
    if (!str)
    {
        $$$("Can't allocate memeory for new string");
        return STANDART_ERROR_CODE;
    }

    *ptrStr = str;

    $$
    return i;
}


