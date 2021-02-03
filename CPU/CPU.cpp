#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <chrono>
#define _USE_MATH_DEFINES
#include <math.h>

#include <GL\freeglut.h>

#include "Asm/Asm.h"
#include "CPU/CPU.h"
#include "Stack/Stack_kernel.h"
#define TYPE_ ui8
#include "Stack/Stack.h"
#undef TYPE_


using namespace Assembler;

CPU::CPUStruct CPU::myCPU = {};
CPU::CPUStruct& myCPU = CPU::myCPU;

/*
\brief Константы, определяющие работу процессора
@{
*/
const Mcode ASM_HLT = 0 << 6 | 0 << 4 | 0 << 2 | 0x0; ///< Именно эта команда будет завершать работу процессора

#ifdef CPU_GRAPH_MODE


/**
\brief Константы, задающие режим работы с графикой
*/

const ui32 VIDEO_MEMORY_PTR = 0x400;    ///< именно в сюда нужно будет писать данные в память, чтобы можно было что-то выводить на экран
const ui16 STANDART_FONT_SIZE = 105;    ///< Размер шрифта в юнитах, данная константа получена через glutWidth(...)

void drawFromVideoMemory();///< Функция, которая будет рисовать на экран все, что располагается в видео буффере


/*
\brief Описание структуры консольного окна
*/
struct Window
{
    ui16 nCols = 80;
    ui16 nLines = 25;
    ui16 fontWidth = 8;
    ui16 fontHeight = 8;
    //эти два значения будут исползоваться для вывода шрифтов на экран
    float ratioX;
    float ratioY;
    ui16 winWidth;
    ui16 winHeight;
}window;

#endif




/*
\brief  Функция возвращает указатель на поле структуры CPU.Register,
в качестве аргумента принимается нормер регистра (см таблицу с регистрами).
\param  [in]  number  Номер регистра, адрес которого кужно получить
\return Указатель на поле структуры CPU.Register, в случае ошибки возвращается NULL
*/
ui32* getRegisterPtr(ui8 number)
{
    if (number > sizeof(myCPU.Register) / sizeof(ui32))
        return nullptr;
    return (ui32*)&myCPU.Register + (number - 1);
}

/*
\brief  Функция возвращает поясняющую строку, по коду ошибки процессора
\param  [in]  errorCode  Код ошибки
\return Возвращается строка, поясняющее код ошибки
*/
C_string getStringByErrorCode(CPUerror errorCode)
{
    switch (errorCode)
    {
    case CPU_OK:
        return "Ok";
        break;
    case CPU_ERROR_INVALID_STRUCUTE:
        return "CPU structure has been broken";
        break;
    case CPU_ERROR_INVALID_COMMAND:
        return "CPU find command that he doesn't know";
        break;
    case CPU_ERROR_EXCEPTION:
        return "An exceptional situation has occurred";
        break;
    case CPU_ERROR_EPI_OUT_OF_RANE:
        return "EPI register is too large for CPU's RAM";
        break;
    case CPU_INVALID_INPUT_DATA:
        return "The data passed is not valid";
    default:
        return "Undefined error code";
        break;
    }
}

/*
\brief  Функция производит инициализацию структуры CPU
\param  [in]  ramSize  Размер виртуальной памяти процессора
\note   Если при инициализации структуры возникли ошибки, то полю isValid присваивается значение 0
Причина возникновения ошибки записывается в лог файл.
*/
void CPU::init(const InputParams inParam)
{
    myCPU.isGraphMode = inParam.useGraphMode;
    myCPU.ramSize = inParam.memorySize;

    if (myCPU.ramSize > 1024 * 512)
    {
        myCPU.isValid = 0;
        logger.push("CPU error", "I'm not sure that you really want too much memory: %d.", myCPU.ramSize);
        return;
    }
    myCPU.RAM = (ui8*)calloc(myCPU.ramSize, sizeof(ui8));
    memset(myCPU.RAM, 0, myCPU.ramSize * sizeof(ui8));
    Assert_c(myCPU.RAM);
    if (!myCPU.RAM)
    {
        myCPU.isValid = 0;
        logger.push("CPU error", "We can't alloc memory for virtual RAM.");
        return;
    }
    StackError errorCode = (StackError)stackInit(&myCPU.stack, 0);
    Assert_c(!errorCode);
    if (errorCode)
    {
        myCPU.isValid = 0;
        free(myCPU.RAM);
        myCPU.RAM = nullptr;
        logger.push("CPU error", "There are some problems with init stack.");
        return;
    }
    myCPU.stack.capacity = -1;
    myCPU.isValid = 1;
    myCPU.stepByStep = inParam.useStepByStepMode;

    #ifdef CPU_GRAPH_MODE
    if (myCPU.isGraphMode)
    {
        ///тут важно, что данные структуры имеют одинаковое расположение полей
        memcpy(&window, &inParam.Window, sizeof(inParam.Window));
        window.winWidth = window.nCols * window.fontWidth;
        window.winHeight = window.nLines * window.fontHeight;

        window.ratioX = (float)window.fontWidth  / STANDART_FONT_SIZE;
        window.ratioY = (float)window.fontHeight / STANDART_FONT_SIZE;

        glutInitWindowSize(window.winWidth, window.winHeight);
        glutInitWindowPosition(0, 0);
        glutCreateWindow("Screen");
        glutDisplayFunc(drawFromVideoMemory);

        glViewport(0, 0, window.winWidth, window.winHeight);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, window.winWidth, 0, window.winHeight);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
    #endif
}


/*
\brief  Функция делает cleanUp структуры CPU
*/
CPU::~CPU()
{
    if (myCPU.RAM)
        free(myCPU.RAM);
    stackDest(&myCPU.stack);
    myCPU.isValid = 0;
}

/*
\brief Функция, производящая дамп процессора, результат закидывается в outStream
\param [in] outStream указатель на поток вывода
*/
void CPU::dump()
{
    FILE* outStream = stdout;
    Assert_c(outStream);
    if (!outStream)
        return;
    fprintf(outStream, "CPU{\n");
    fprintf(outStream, "    Registers{\n");
    #define printRegInfo(regName)\
    fprintf(outStream, "        " #regName ":0x%04X  (int: %d) \t(float: %f)\n", myCPU.Register.##regName,myCPU.Register.##regName,*((float*)&myCPU.Register.##regName))
    printRegInfo(eax);printRegInfo(ebx);printRegInfo(ecx);printRegInfo(edx);
    printRegInfo(esi);printRegInfo(edi);printRegInfo(ebp);printRegInfo(eip);
    printRegInfo(efl);printRegInfo(ecs);printRegInfo(eds);printRegInfo(esp);
    #undef printRegInfo
    fprintf(outStream, "    }\n");
   
    #ifdef DUMP_PRINT_MEMORY
    fprintf(outStream, "    RAM:\n");
    fprintf(outStream, "    Segment offset |  0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F\n");
    fprintf(outStream, "    ----------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < myCPU.ramSize ; i += 16)
    {
        fprintf(outStream, "    Segment:0x%05X|", i);
        for (int j = 0; j < 16; j++)
        {
            ui8 data = i + j < myCPU.ramSize ? myCPU.RAM[i + j] : 0;
            fprintf(outStream, "  0x%02X", data & 0xFF);
        }
        fprintf(outStream, "\n");
    }
    #endif
    fprintf(outStream, "}\n");
}




static inline bool getBit(ui32 marchCode, ui8 n)
{
    return marchCode & (1 << n);
}

static inline void setBit(ui32* marchCode, ui8 n, bool value)
{
    *marchCode &= ~(1 << n);
    *marchCode |= (value << n);
}


/*
\brief  Функция вычисления указателей на память, с которой будет работать команда
\param  [in]  cmd   команда
\param  [in,out]  dst  указатель на область памяти, отвечающее первому операнду в команде
\param  [in,out]  src  указатель на область памяти, отвечающее второму операнду в команде
\note   В случае ошибки будет брошен Assert_c.
Пример работы: если команда имела вид mov eax,ebx, то в dst и src будут равны:
dst = &CPU.Register.eax, src = &CPU.Register.ebx
*/
void getOperandsPointer(Command* cmd, OperandUnion** dst, OperandUnion** src)
{
    ui32** ptrOperands[2] = { (ui32**)dst, (ui32**)src };
    OperandType opType;
    ui32 offset = 0;
    for (ui8 i = 0; i < cmd->code.bits.nOperands; i++)
    {
        opType = getOperandType(*cmd, i);
        switch (opType)
        {
        case OPERAND_REGISTER:
            *ptrOperands[i] = getRegisterPtr(cmd->operand[i].ivalue);
            break;
        case OPERAND_NUMBER:
            *ptrOperands[i] = &cmd->operand[i].ivalue;
            break;
        case OPERAND_MEMORY:
            offset = myCPU.Register.eds;
            offset += myCPU.isFloatPointMath ? static_cast<ui32>(cmd->operand[i].fvalue) : cmd->operand[i].ivalue;
            if (offset+sizeof(ui32) >= myCPU.ramSize)
            {
                Assert_c(!"The command tries to access a nonexistent memory area!");
                myCPU.interruptCode = 3;
                break;
            }
            #ifdef CPU_GRAPH_MODE
            if (myCPU.isGraphMode
                &&  offset >= VIDEO_MEMORY_PTR
                && offset <= VIDEO_MEMORY_PTR + sizeof(ui8) * window.nCols * window.nLines
                )
            {
                myCPU.isVideoMemoryChanged = 1;
                myCPU.ChangedPixel.x = offset - VIDEO_MEMORY_PTR;
                myCPU.ChangedPixel.y = myCPU.ChangedPixel.x;
                myCPU.ChangedPixel.x %= window.nCols;
                myCPU.ChangedPixel.y /= window.nCols;
            }
            #endif
            *ptrOperands[i] = (ui32*)&myCPU.RAM[offset];
            break;
        case OPERAND_MEM_BY_REG:
            offset = myCPU.Register.eds + static_cast<ui32>(*getRegisterPtr(cmd->operand[i].ivalue));
            if (offset+sizeof(ui32)>= myCPU.ramSize)
            {
                Assert_c(!"The command tries to access a nonexistent memory area!");
                myCPU.interruptCode = 3;
                break;
            }
            #ifdef CPU_GRAPH_MODE
            if (CPU.isGraphMode && offset >= VIDEO_MEMORY_PTR && offset <= VIDEO_MEMORY_PTR + sizeof(ui8) * window.nCols * window.nLines)
            {
                CPU.isVideoMemoryChanged = 1;
                CPU.ChangedPixel.x = offset - VIDEO_MEMORY_PTR;
                CPU.ChangedPixel.y = CPU.ChangedPixel.x;
                CPU.ChangedPixel.x %= window.nCols;
                CPU.ChangedPixel.y /= window.nCols;
            }
            #endif
            *ptrOperands[i] = (ui32*)&myCPU.RAM[offset];
            break;
        default:
            Assert_c(!"Invalid type of operand.");
            break;
        }
    }
}

/*
\brief  Функция проверяет является ли число нулем с заданной точностью
\param  [in]  num       Исследуемое число
\param  [in]  accuracy  Точность сравнения
\return true, если число близко к нулю и false в противном случае.
*/
static inline bool isZero(float num, float accuracy = 1E-7)
{
    return fabs(num) < accuracy ? 1 : 0;
}


/*
\brief  Объвление дополнительных функций
*/
#define DEF(name, mCode, vStr1, vStr2, code)\
void run_##name(Command* cmd) code
#include "Extend.h"
#undef DEF


/*
\breif Массив функций, реализующих поведение процессора
*/
CPU::PtrToFunction CPU::runFunction[] =
{
    #define DEF(name, machineCode, validStrOperand_1, validStrOperand_2, code) run_##name,
    #include "Extend.h"
    #undef DEF
};
const ui32 CPU::FUNCTION_TABLE_SIZE = sizeof(CPU::runFunction) / sizeof(CPU::PtrToFunction);



/*
\brief  Функция, запускает выполнение программы, начиная с текущего значение CPU.Register.eip
\param  [in]  writeResultInLog  Флаг, отвечающий за то, хотим ли мы увидеть результат работы программы в логе
\return Возвращается код ошибки или CPU_OK
*/
CPUerror CPU::evaluate()
{
    ui8* ptr = &myCPU.RAM[myCPU.Register.eip];
    Command cmd;

    while (*((Mcode*)ptr) != ASM_HLT)
    {
        ptr = &myCPU.RAM[myCPU.Register.eip];
        cmd.code.marchCode = *((Mcode*)ptr);
        ptr += sizeof(Mcode);
        myCPU.Register.eip += sizeof(Mcode);

        for (int index = 0; index < cmd.code.bits.nOperands; index++)
        {
            OperandType opType = getOperandType(cmd, index);
            if (opType == OPERAND_REGISTER || opType == OPERAND_MEM_BY_REG)
            {
                cmd.operand[index].ivalue = *((ui8*)ptr);
                myCPU.Register.eip += sizeof(ui8);
                ptr += sizeof(ui8);
            }
            if (opType == OPERAND_NUMBER || opType == OPERAND_MEMORY)
            {
                cmd.operand[index].ivalue = *((ui32*)ptr);
                myCPU.Register.eip += sizeof(ui32);
                ptr += sizeof(ui32);
            }
        }

        if (myCPU.stepByStep)
        {
            Disassembler::Instance().disasmCommand(cmd, stdout);
            dump();
            system("pause");
            system("cls");
        }


        ui32 indexCalledFunc = cmd.code.bits.opCode;
        if (indexCalledFunc >= FUNCTION_TABLE_SIZE)
        {
            logger.push("CPU error", "Invalid machine code of command.");
            dump();
            return CPU_ERROR_INVALID_COMMAND;
        }
        runFunction[indexCalledFunc](&cmd);
        ptr = &myCPU.RAM[myCPU.Register.eip];

        if (myCPU.interruptCode)
        {
            logger.push("CPU error", "Catch exception after execution command:");
            Disassembler::Instance().disasmCommand(cmd, logger.getStream());
            dump();
            return CPU_ERROR_EXCEPTION;
        }
        if (myCPU.Register.eip >= myCPU.ramSize)
        {
            logger.push("CPU error", "Register epi quite big for RAM.");
            dump();
            return CPU_ERROR_EPI_OUT_OF_RANE;
        }
        myCPU.stack.data = &myCPU.RAM[myCPU.Register.ess];
        myCPU.stack.size = myCPU.Register.esp;

        #ifdef CPU_GRAPH_MODE
        if (myCPU.isGraphMode && myCPU.isVideoMemoryChanged)
        {
            drawFromVideoMemory();
            myCPU.isVideoMemoryChanged = 0;
        }
        #endif
    }
    return CPU_OK;
}


CPUerror CPU::run(ui8* bytes, ui32 size, ui32 ptrStart)
{
    if (!myCPU.isValid)
    {
        logger.push("CPU error", "You try to evaluate program on broken CPU.");
        return CPU_ERROR_INVALID_STRUCUTE;
    }
    Assert_c(bytes);
    if (!bytes)
    {
        logger.push("CPU error", "You try execute program, located by NULL pointer.");
        return CPU_INVALID_INPUT_DATA;
    }
    Assert_c(size > 0);
    if (size <= 0)
    {
        logger.push("CPU error", "You try execute program, that have incorrect size:%d", size);
        return CPU_INVALID_INPUT_DATA;
    }
    Assert_c(ptrStart + size + 1 < myCPU.ramSize);
    if (ptrStart + size + 1 >= myCPU.ramSize)
    {
        logger.push("CPU error", "Your program doesn't fit in RAM. Try to change ptrStart or write small program");
        return CPU_INVALID_INPUT_DATA;
    }

    memcpy(&myCPU.RAM[ptrStart], bytes, size);
    *((Mcode*)&myCPU.RAM[ptrStart + size]) = ASM_HLT; ///на всякий случай поставим код остановки, после всей программы
    myCPU.Register.eip = ptrStart;
    myCPU.Register.ecs = ptrStart;
    myCPU.Register.eds = ptrStart;
    myCPU.Register.ess = ptrStart;
    myCPU.Register.esp = size + 1; /// стек будет лежать за кодом

    #ifdef CPU_GRAPH_MODE
    /// Перед видеопамятью расположим информацию о количестве строк и столбиков
    if (myCPU.isGraphMode && myCPU.ramSize >= VIDEO_MEMORY_PTR)
    {
        *((ui16*)&myCPU.RAM[VIDEO_MEMORY_PTR - 2 * sizeof(ui16)]) = window.nLines;
        *((ui16*)&myCPU.RAM[VIDEO_MEMORY_PTR - 1 * sizeof(ui16)]) = window.nCols;
    }
    #endif

    myCPU.stack.data = &myCPU.RAM[myCPU.Register.ess];
    myCPU.stack.size = myCPU.Register.esp;
    CPUerror errorCode = evaluate();
    return errorCode;
}



#ifdef CPU_GRAPH_MODE
static inline void renderChar(float x, float y, const char c)
{
    glColor3f(0,0,0);
    glRectf(x, y, x + window.fontWidth, y + window.fontHeight);

    glColor3f(1, 1, 1);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(window.ratioX, window.ratioY, 1.0);
    glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, c);
    glPopMatrix();
}


void drawFromVideoMemory()
{
    ///если раскомментировать этот кусок, то получится красивый эффект
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.01);
    glRectf(0, 0, window.winWidth, window.winHeight);
    

    
    static const char* string = (char*)&myCPU.RAM[VIDEO_MEMORY_PTR];
    int col = myCPU.ChangedPixel.x;
    int line = myCPU.ChangedPixel.y;
    renderChar(col * window.fontWidth, window.winHeight - (line+1) * window.fontWidth, string[col + line * window.nCols]);

    glFlush();
}

#endif