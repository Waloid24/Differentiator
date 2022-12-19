#include "cpu.h"

#define JUMP_FORM(sign)                             \
    i++;                                            \
    int n1 = stack_pop (&stk, logfile);             \
    int n2 = stack_pop (&stk, logfile);             \
    if (n1 sign n2)                                 \
    {                                               \
        int ptrToJmp = arrCode[i];                  \
        printf ("in cpu JMP_FORM! tmp = %d, %d %s %d\n", ptrToJmp, n1, #sign, n2); \
        i = ptrToJmp-1;                             \
        stack_push (&stk, n2, logfile);             \
        stack_push (&stk, n1, logfile);             \
    }                                               \
    else                                            \
    {                                               \
        stack_push (&stk, n2, logfile);             \
        stack_push (&stk, n1, logfile);             \
    }                   

const int NUM_REGISTERS = 4;
const int MAX_RAM = 100;

static int getNum (void);

void getCode (char * nameFile, code_t fileInfo, int numTags)
{
    MY_ASSERT (nameFile == nullptr, "Unable to access the file");
    FILE * logfile = open_logfile ("stack_log.txt");
    MY_ASSERT (logfile == nullptr, "Unable to open logfile for stack");
    FILE * logCallStack = open_logfile ("callStack.txt");
    MY_ASSERT (logCallStack == nullptr, "Unable to open logfile for call stack");
    setbuf (logfile, NULL);
    stack_t stk = {};

    stack_t callStack = {};
    stk_ctor (&callStack, numTags, "callStk");

    FILE * binFile = fopen (nameFile, "rb");
    MY_ASSERT (binFile == nullptr, "Unable to open binary file");

    int ram[MAX_RAM] = {};

    int * arrCode = (int *) calloc (fileInfo.nStrs * 3, sizeof (int));
    MY_ASSERT (arrCode == nullptr, "Unable to allocate new memory");

    stk_ctor (&stk, fileInfo.nStrs * 3, "cpu");

    fread (arrCode, sizeof(int), fileInfo.nStrs * 3, binFile);

    int * regs = (int *) calloc (NUM_REGISTERS, sizeof(int));
    MY_ASSERT (regs == nullptr, "Unable to allocate new memory");

    for (int i = 0; i < fileInfo.nStrs * 3; i++)
    {
        printf (">>arrCode[%d] = %d\n", i, arrCode[i]);
        if (arrCode[i] == 17) //hlt
        {
            // stack_dtor (&stk);
            break;
        }
        else if (arrCode[i] == 33) //push 7
        {
            i++;
            stack_push (&stk, arrCode[i], logfile);
            stack_dump (stk, logfile);
            
        }
        else if (arrCode[i] == 34) //pop 
        {
            i++;
            stack_pop (&stk, logfile);
        }
        else if (arrCode[i] == 65) //push rax
        {
            i++;
            int nReg = arrCode[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            stack_push (&stk, regs[nReg], logfile);
        }
        else if (arrCode[i] == 66) //pop rax
        {     
            i++;
            int nReg = arrCode[i];    
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");    
            regs[nReg] = stack_pop (&stk, logfile);
        }
        else if (arrCode[i] == 97) //push 5 + rax
        {
            i++;
            int nReg = arrCode[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            i++;    
            int tmp = regs[nReg] + arrCode[i];
            stack_push (&stk, tmp, logfile);
        }
        else if (arrCode[i] == 161) //push[10]
        {
            i++;
            int ramIndex = arrCode[i];
            MY_ASSERT (ramIndex > MAX_RAM-1, "You are out of RAM");
            stack_push (&stk, ram[ramIndex], logfile);
        }
        else if (arrCode[i] == 162) // pop[10]
        {
            i++;
            int ramIndex = arrCode[i];
            MY_ASSERT (ramIndex > MAX_RAM-1, "You are out of RAM");
            ram[ramIndex] = stack_pop (&stk, logfile);
        }
        else if (arrCode[i] == 193) //push[rcx]
        {
            i++;
            int nReg = arrCode[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            int ramIndex = regs[nReg];
            MY_ASSERT (ramIndex > MAX_RAM-1, "You are out of RAM");
            stack_push (&stk, ram[ramIndex], logfile);
        }
        else if (arrCode[i] == 194) //pop[rcx]
        {
            i++;
            int nReg = arrCode[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            int ramIndex = regs[nReg];
            MY_ASSERT (ramIndex > MAX_RAM-1, "You are out of RAM");
            ram[ramIndex] = stack_pop (&stk, logfile);
        }
        else if (arrCode[i] == 225)  //push [5 + rcx]
        {
            i++;
            int nReg = arrCode[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            i++;
            int value1 = regs[nReg];
            int value2 = arrCode[i];
            int ramIndex = value1 + value2;
            MY_ASSERT (ramIndex > MAX_RAM-1, "You are out of RAM");
            stack_push (&stk, ram[ramIndex], logfile);
        }
        else if (arrCode[i] == 226) //pop [5 + rcx]
        {
            i++;
            int nReg = arrCode[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            i++;
            int value1 = regs[nReg];
            int value2 = arrCode[i];
            int ramIndex = value1 + value2;
            MY_ASSERT (ramIndex > MAX_RAM-1, "You are out of RAM");
            ram[ramIndex] = stack_pop (&stk, logfile);
        }
        
        else if (arrCode[i] == 3) //add
        {
            int n1 = stack_pop (&stk, logfile);
            int n2 = stack_pop (&stk, logfile);
            stack_push (&stk, n1 + n2, logfile);
        }
        else if (arrCode[i] == 4) //sub
        {
            int n1 = stack_pop (&stk, logfile);
            int n2 = stack_pop (&stk, logfile);
            stack_push (&stk, n1 - n2, logfile);
        }
        else if (arrCode[i] == 5) //mul
        {
            int n1 = stack_pop (&stk, logfile);
            int n2 = stack_pop (&stk, logfile);
            stack_push (&stk, n1 * n2, logfile);
        }
        else if (arrCode[i] == 6) //div
        {
            int n1 = stack_pop (&stk, logfile);
            int n2 = stack_pop (&stk, logfile);
            stack_push (&stk, n1 / n2, logfile);
        }
        else if (arrCode[i] == 7) //out
        {
            fprintf (stdout, "OUT: %d\n", stack_pop (&stk, logfile));
        }
        else if (arrCode[i] == 8) // in
        {
            int tmp = getNum ();
            printf ("tmp = %d\n", tmp);
            stack_push (&stk, tmp, logfile);
            stack_dump (stk, logfile);
        }
        else if (arrCode[i] == 9) //jmp
        {
            i++;
            int ptrToJmp = arrCode[i];
            printf ("in cpu jmp! ptrToJmp = %d\n", ptrToJmp);
            i = ptrToJmp-1;
        }
        else if (arrCode[i] == 12) //ja  
        {
            JUMP_FORM (>)
        }
        else if (arrCode[i] == 13) //jb
        {
            JUMP_FORM(<)
        }
        else if (arrCode[i] == 14) //jbe
        {
            JUMP_FORM(<=)
        }
        else if (arrCode[i] == 15) //jge
        {
            JUMP_FORM (>=)
        }
        else if (arrCode[i] == 16) //je
        {
            JUMP_FORM (==)
        }
        else if (arrCode[i] == 10) //call
        {
            i++;
            int ptrToJmp = arrCode[i];
            stack_push (&callStack, i+1, logCallStack);
            i = ptrToJmp-1;
            printf ("in call: call = %d, i = %d\n", ptrToJmp, i);
        }
        else if (arrCode[i] == 11) //RET
        {
            i = stack_pop (&callStack, logCallStack) - 1;
            printf ("in ret: i = %d\n", i);
        }
        else if (arrCode[i] == 18)
        {
            int tmp = stack_pop (&stk, logfile);
            printf ("tmp = %d\n", tmp);
            int result = (int) sqrt ((double) tmp);
            printf ("sqrt = %d\n", result);
            stack_push (&stk, result, logfile);
        }
        else 
        {
            break;
            printf ("You have some problem\n");
        }
    }

    free (regs);
    free (arrCode);
    fclose(logfile);
}

static int getNum (void)
{
    int degreeOfNum = 0;
    int enterSymbols = scanf (" %d", &degreeOfNum);
    while (getchar() != '\n');

    if (enterSymbols == 0)
    {
        printf ("Please, enter a number\n");
        return getNum ();
    }

    return degreeOfNum;
}
