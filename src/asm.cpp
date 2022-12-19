#include "asm.h"

const int SIGNATURE = 3;
const int VERSION = 1;

int N_FUNC_WITHOUT_ARG = 0;

const int NO_TAGS = -2;

enum {
    NO = 0,
    YES = 1
};

static void scanTag (char * src, char * dst, int lengthSrc);
static int findFreePlace (tag_t * tags, int sizeArrTags);
static long int findTag (tag_t * tags, char * argument, int * startArrWithCode, int numTags);
static void ram (int ** code, char * firstBracket, int numCmd);
static void no_ram (int ** code, char * strCode, int countLetters, int numCmd, tag_t * tags);

void createBinFile (char ** arrStrs, code_t * prog, char * nameBinFile, int numTags)
{
    MY_ASSERT (arrStrs == nullptr, "There is no access to array of strings");
    MY_ASSERT (prog == nullptr, "There is no access to struct with information about code file");

    for (int i = 0; i < prog->nStrs; i++)
    {
        printf ("arrStrs[%d] = %s\n", i, arrStrs[i]);
    }

    FILE * binFile = fopen (nameBinFile, "wb");
    MY_ASSERT (binFile == nullptr, "There is no access to bin file");
    setbuf (binFile, NULL);
    // pushSignature (binFile, *prog);

    tag_t * tags = (tag_t *) calloc (numTags, sizeof(tag_t));
    MY_ASSERT (tags == nullptr, "Unable to allocate new memory");

    int * code = (int *) calloc (prog->nStrs * 3, sizeof(int)); //выделяем для int, а указаетль как char
    int * tmp = code;

    int isTag = 0;
    long int tagCallOrJmps = 0;
    int tagJmp = 0;

    freeCall_t * freeFunc = (freeCall_t *) calloc (numTags, sizeof(freeCall_t));
    MY_ASSERT (freeFunc == nullptr, "Unable to allocate new memory");
    freeCall_t * saveTagCallsOrJumps = freeFunc;

    #define DEF_CMD(nameCmd, countLetters, numCmd, isArg)                       \
        if (myStrcmp (cmd, #nameCmd) == 0)                                      \
        {                                                                       \
            printf (">in DEF_CMD: cmd = %s, #nameCmd = %s\n",cmd, #nameCmd);    \
            if (isArg)                                                          \
            {                                                                   \
                countArgs++;                                                    \
                getArg (&code, arrStrs[i], countLetters, numCmd, tags);         \
                code++;                                                         \
            }                                                                   \
            else                                                                \
            {                                                                   \
                *code = CMD_##nameCmd;                                          \
                printf (">in DEF_CMD:  *code = %d\n", *code);                   \
                code++;                                                         \
            }                                                                   \
        }                                                                       \
        else 

    #define CALLS_AND_JMPS(nameJmp, length)                                     \
        if (myStrcmp (cmd, #nameJmp) == 0)                                      \
        {                                                                       \
            countArgs++;                                                        \
            *code = CMD_##nameJmp;                                              \
            code++;                                                             \
                                                                                \
            skipSpace (&(arrStrs[i]), length);                                  \
                                                                                \
            int lengthTag = strlen (arrStrs[i]);                                \
                                                                                \
            char * nameTag = (char *) calloc (lengthTag, sizeof(char));         \
            scanTag (arrStrs[i], nameTag, lengthTag);                           \
                                                                                \
            tagCallOrJmps = findTag (tags, nameTag, tmp, numTags);              \
            printf (">>> in no_ram: indexTag = %ld\n", tagCallOrJmps);          \
            if (tagCallOrJmps == NO_TAGS)                                       \
            {                                                                   \
                freeFunc->ptrToArrWithCode = code;                              \
                freeFunc->tag = nameTag;                                        \
                N_FUNC_WITHOUT_ARG++;                                           \
                printf ("free FUNC: *(freeFunc->ptrToArrWithCode) = %d,"        \
                " N_FUNC_WITHOUT_ARG = %d, freeFunc->tag = %s\n",               \
                *(code-1), N_FUNC_WITHOUT_ARG, freeFunc->tag);                  \
                code++;                                                         \
                freeFunc++;                                                     \
            }                                                                   \
            else                                                                \
            {                                                                   \
                printf ("filled call\n");                                       \
                *code = tagCallOrJmps;                                          \
                code++;                                                         \
                free (nameTag);                                                 \
            }                                                                   \
        }                                                                       \
        else

    char * cmd = (char *) calloc (STANDART_SIZE, sizeof(char));
    MY_ASSERT (cmd == nullptr, "Unable to allocate new memory");

    int countArgs = 0;
    for (int i = 0, ip = -1; i < prog->nStrs; i++)
    {
        printf ("\n");
        sscanf (arrStrs[i], "%s", cmd);
        printf ("arrStrs[%d] = \033[51;1m %s \033[0m \n", i, arrStrs[i]);

        if (strchr (cmd, ':') != nullptr) 
        {
            int freeTag = findFreePlace (tags, numTags);
            MY_ASSERT (freeTag == -1, "There are no free cells in the tag array");

            int lengthSrc = strlen (cmd);
            printf ("in createBinFile in for (\033[31m length tag \033[0m): %d\n", lengthSrc);

            tags[freeTag].nameTag = (char *) calloc (lengthSrc, sizeof(char));
            MY_ASSERT (tags[freeTag].nameTag == nullptr, "Unable to allocate new memory");
            scanTag (cmd, tags[freeTag].nameTag, lengthSrc);

            tags[freeTag].ptr = code;   
            printf ("tags[%d].ptr = %d; *code = %d\n", freeTag, *(tags[freeTag].ptr), *(code-1));       
            printf ("tags[%d].nameTag = %s\n", freeTag, tags[freeTag].nameTag);
        }
        else
        #include "jmps.h"
        #include "cmd.h"
        {
            fprintf (stderr, "command undifined is \"%s\" (arrStrs[%d])\n", arrStrs[i], i);
            MY_ASSERT (1, "This command is not defined.\n");
        }
        printf ("\n");
    }
    printf ("N_FUNC_WITHOUT_ARG = %d\n", N_FUNC_WITHOUT_ARG);
    for (int i = 0; i < N_FUNC_WITHOUT_ARG; i++)
    {
        printf ("*(saveTagCallsOrJumps->ptrToArrWithCode) = %d\n", *(saveTagCallsOrJumps->ptrToArrWithCode));
        tagCallOrJmps = findTag (tags, saveTagCallsOrJumps->tag, tmp, numTags);
        printf (">>> in no_ram: indexTag = %ld\n", tagCallOrJmps);
        MY_ASSERT (tagCallOrJmps == NO_TAGS, "This tag does not exist");
        *(saveTagCallsOrJumps->ptrToArrWithCode) = tagCallOrJmps;
        saveTagCallsOrJumps++;
    }   

    for (int i = 0; i < prog->nStrs * 3; i++)
    {
        printf ("code[%d] = %d\n", i, tmp[i]);
    }

    fwrite (tmp, sizeof(int), prog->nStrs * 3, binFile);


    printf (">> in createBinFile (RESULT): countArgs = %d\n", countArgs);
    fclose (binFile);
    free (cmd);
    #undef DEF_CMD
}

static int findFreePlace (tag_t * tags, int sizeArrTags)
{
    for (int i = 0; i < sizeArrTags; i++)
    {
        if (tags[i].ptr == 0)
        {
            return i;
        }
    }
    return -1;
}

static void scanTag (char * src, char * dst, int lengthSrc)
{
    for (int i = 0; i < lengthSrc; i++)
    {
        if (*src == ':' || *src == ';')
        {
            *dst = '\0';
            break;
        }
        if (isalnum(*src) || ispunct(*src))
        {
            *dst = *src;
            dst++;
            src++;
        }
        else
        {
            MY_ASSERT (1, "Incorrect tag: missing \":\" or there are invalid characters");
        }
    }
}

void getArg (int ** code, char * str_text_code, int countLetters, int numCmd, tag_t * tags)
{
	char * firstBracket = nullptr;
    if ((firstBracket = strchr (str_text_code, '[')) != nullptr)
    {
        printf ("[]\n");
        ram(code, firstBracket, numCmd);
    }
    else
	{
		printf ("no []\n");
        no_ram(code, str_text_code, countLetters, numCmd, tags); //есть ли неинициал тэг
	}
}

static void no_ram (int ** code, char * strCode, int countLetters, int numCmd, tag_t * tags) //только push
{
    printf ("in no_ram: \033[32;1m numCmd \033[0m = %d\n", numCmd);

    skipSpace (&strCode, countLetters);
    char * ptrToArg = strCode;

    //ищем минус
    //если он есть, то сразу после него только число, причем это push
    //если минуса нет, то возможны случаи pop и push
    //если есть rax, если нет rax
    //нет rax, то push 5

    char * reg  = (char *) calloc (4, sizeof(char));
    MY_ASSERT (reg == nullptr, "It's impossible to read the argument");

    char * ptrToReg = nullptr;

    char * placeOfPlus = nullptr;

    if (strchr(ptrToArg, '-') != nullptr) //ситуация push -5
    {
        MY_ASSERT (numCmd != CMD_PUSH, "the minus can only be present in the \"push\" command"); 
		printf ("command is push 111\n");
        **code = 33;
        (*code)++;
        **code = atoi(strCode);
        printf ("in no_ram: push -...: **code = %d\n", **code);
    }
    else if ((ptrToReg = strchr(ptrToArg, 'r')) != nullptr) //если есть 
    {
        printf ("there is reg\n");
        reg = (char *) memcpy (reg, ptrToReg, 3 * sizeof(char));
        *(reg+3) = '\0';
        printf ("reg = %s\n", reg);

        char count_reg = 0;
        if      (strcmp (reg, "rax") == 0) count_reg = (char) RAX;
        else if (strcmp (reg, "rbx") == 0) count_reg = (char) RBX;
        else if (strcmp (reg, "rcx") == 0) count_reg = (char) RCX;
        else if (strcmp (reg, "rdx") == 0) count_reg = (char) RDX;
        else    MY_ASSERT (1, "The case is specified incorrectly");
            
        if ((placeOfPlus = strchr(ptrToArg, '+')) != nullptr) //pop rax + 5, push rax + 5
        {
            placeOfPlus++;
            if (numCmd == CMD_PUSH) //push rax + 5, push 5 + rax
            {
                **code = 97;
                (*code)++;
            }
            else if (numCmd == CMD_POP)
            {
                **code = 98;
                (*code)++;
            }   
            else
            {
                MY_ASSERT (1, "Incorrect expression with \"+\"");
            }

            skipSpace(&placeOfPlus, 0);
            printf ("placeOfPlus = %s\n", placeOfPlus);
            int num = 0;
            int nSymbols = readNum (ptrToArg, &num);
            if (nSymbols == 0)                              //push rax + 5
            {
                readNum (placeOfPlus, &num);
                **code = count_reg;
                printf ("**code (reg) = %d\n", **code);
                (*code)++;
                **code = num;
                printf ("**code (num) = %d\n", **code);
            }
            else                                            //push 5 + rax
            {
                **code = count_reg;
                printf ("**code (reg) = %d\n", **code);
                (*code)++;
                **code = num;
                printf ("**code (num) = %d\n", **code);
            }
        }
        else                                                //push rax, pop rax
        {
            if (numCmd == CMD_PUSH)
            {
                printf ("push rax");
                **code = 65;
                (*code)++;
                **code = count_reg;
            }
            else if (numCmd == CMD_POP)
            {
                **code = 66;
                (*code)++;
                **code = count_reg;
            }
            else
            {
                MY_ASSERT (1, "Incorrect expression with a case without \"+\"");
            }
        }
    }
    else if (numCmd == CMD_PUSH) //push 5
    {
        int num = 0;
        int nSymbols = readNum (ptrToArg, &num);
        MY_ASSERT (nSymbols == 0, "Push without a numeric argument\n");
        **code = 33;
        (*code)++;
        **code = num;
    }
    else if (numCmd == CMD_POP)
    {
        int num = 0;
        int nSymbols = readNum (ptrToArg, &num);
        MY_ASSERT (nSymbols != 0, "Pop with a numeric argument\n");
        **code = 34;
        (*code)++;
        **code = num;
    }
    else
    {
        MY_ASSERT (1, "Incorrect push or pop");
    }
    free (reg);
}

static void ram (int ** code, char * firstBracket, int numCmd)
{ 
	printf ("Works ram func\n");
	printf ("numCmd = %d\n", numCmd);

    char * reg  = (char *) calloc (3, sizeof(char));
    MY_ASSERT (reg == nullptr, "It's impossible to read the argument");
    char * trash = (char *) calloc (8, sizeof(char)); 
    MY_ASSERT (trash == nullptr, "func GetArgument: it's impossible to read other symbols");

	int num = -1;
    firstBracket++; //сдвинулись со скобки
    if (strcmp (firstBracket, "r") >= 0)
    {
        sscanf (firstBracket, "%[rabcdx]", reg);  //обработка push [rax + 5]
        firstBracket = firstBracket + 3;
        skipSpace (&firstBracket, 0);
        sscanf (firstBracket, "%[+]", trash);
        firstBracket = firstBracket + 1;
        skipSpace (&firstBracket, 0);
        sscanf (firstBracket, "%d", &num);
    }
    else 
    {
        int nDigit = readNum (firstBracket, &num);//обработка push [5 + rax]
        firstBracket = firstBracket + nDigit;
        skipSpace (&firstBracket, 0);      
        sscanf (firstBracket, "%[+]", trash);
        firstBracket = firstBracket + 1;
        skipSpace (&firstBracket, 0);
        sscanf (firstBracket, "%[rabcdx]", reg);
    }

    printf ("num = %d, trash = %s, reg = %s\n", num, trash, reg);

    MY_ASSERT (num == -1 && *reg == 0, "Your argument in square brackets are incorrect");

    if (num == -1 && *reg != 0)     //ситуация вида push [rax] и pop [rax]
    {
        char count_reg = 0;
        if      (strcmp (reg, "rax") == 0) count_reg = (char) RAX;
        else if (strcmp (reg, "rbx") == 0) count_reg = (char) RBX;
        else if (strcmp (reg, "rcx") == 0) count_reg = (char) RCX;
        else if (strcmp (reg, "rdx") == 0) count_reg = (char) RDX;
        else                                  MY_ASSERT (1, "The case is specified incorrectly");


        if (numCmd == CMD_PUSH) //0000 | 0000
        {                       //1100 | 0001
			printf ("There is push 222\n");
            **code = 193;
            (*code)++;
            **code = count_reg;
        }

        if (numCmd == CMD_POP) //1100 | 0010
        {
			printf ("There is pop 222\n");
            **code = 194;
            printf ("**code = %d\n", **code);
            (*code)++;
            **code = count_reg;
            printf ("**code = %d\n", **code);
        }
    }
    
    else if (*reg == 0 && num != -1) //ситуация вида push [5] и pop [5]
    {
        if (numCmd == CMD_PUSH) //1010 | 0001      push [5] 
        {
			printf ("There is push 333\n");

            **code = 161;
            (*code)++;
            **code = num;
        }
            
        if (numCmd == CMD_POP)  //if менять на else //1010 | 0010 pop [5]
        {
			printf ("There is pop 333\n");
            **code = 162;
            (*code)++;
            **code = num;
        }
    }
    else //ситуация вида push [5 + rax] или pop [5 + rax] или [rax + 5]
    {
		char count_reg = 0;
        if      (strcmp (reg, "rax") == 0) count_reg = (char) RAX;
        else if (strcmp (reg, "rbx") == 0) count_reg = (char) RBX;
        else if (strcmp (reg, "rcx") == 0) count_reg = (char) RCX;
        else if (strcmp (reg, "rdx") == 0) count_reg = (char) RDX;
        else                                  MY_ASSERT (1, "The case is specified incorrectly");

        if (numCmd == CMD_PUSH) //1110 | 0001  push [5 + rax]
        {
            **code = 225;
            (*code)++;
            **code = count_reg;
            (*code)++;
            **code = num;
        }

        if (numCmd == CMD_POP) //1110 | 0010  pop [5 + rax]
        {
			printf ("There is pop 444\n");
            **code = 226;
            (*code)++;
            **code = count_reg;
            (*code)++;
            **code = num;
        }
    }
    free (reg);
    free (trash);
}

void pushSignature (FILE * dst, code_t codeFile)
{
	MY_ASSERT (dst == nullptr, "null pointer to file");
	int * signature = (int *) calloc (SIGNATURE, sizeof(int));
	signature[0] = 'ASM';
	signature[1] = VERSION;
	signature[2] = codeFile.nStrs;
	
	fwrite (signature, sizeof(int), 3, dst);
}

//---------------------------------string functions--------------------------------------------------
void skipSpace (char ** strCode, int countLetters)
{
    *strCode = *strCode + countLetters;
	int i = 0;
    for (; **strCode == ' ' || **strCode == '\t'; i++)
    {
        (*strCode)++;
    }
}

static long int findTag (tag_t * tags, char * argument, int * startTagWithCode, int numTags)
{
    MY_ASSERT (tags == nullptr, "There is no access to array with tags");
    for (int i = 0; i < numTags; i++)
    {
        printf (">>>in findTag: argument [%d/%d] = %s, tags[%d].nameTag = %s\n", i, numTags, argument, i, tags[i].nameTag);
        if (tags[i].nameTag != nullptr && myStrcmp (argument, tags[i].nameTag) == 0) //==?
        {
            printf (">>>hyi Landau\n");
            return tags[i].ptr - startTagWithCode;
        }
    }
    printf ("NO_TAGS!\n");
    return NO_TAGS;
}

int readNum (char * src, int * dst)
{
    MY_ASSERT (src == nullptr, "There is no access to source string");
    MY_ASSERT (dst == nullptr, "There is no access to destination string");
    char sym = 0;
    char * tmp = src;

    while (48 <= *tmp && *tmp <= 57)
    {
        tmp++;
        sym++;
    }
    *dst = atoi(src);
    return sym;
}

int myStrcmp (const char * string1, const char * string2)
{
	int i = 0, j = 0;
	for (; string1[i] != '\0' && string2[j] != '\0'; i++, j++)
	{
		while (!isalpha(string1[i]) && string1[i] != '\0')
			i++;
		while (!isalpha(string2[j]) && string2[j] != '\0')
			j++;
		if (tolower(string1[i]) == tolower(string2[j]))
			continue;
		return (tolower(string1[i]) - tolower(string2[j]));
	}

	while (!isalpha(string1[i]) && string1[i] != '\0')
			i++;

	while (!isalpha(string2[j]) && string2[j] != '\0')
			j++;

	return (tolower(string1[i]) - tolower(string2[j]));
}
