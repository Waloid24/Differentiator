#include <stdio.h>
#include <sys/stat.h>
#include "MY_ASSERT.h"
#include "readCode.h"

static size_t size (const char * arg_console);
static void writeTextToBuf (code_t * code, FILE * codeFile);
static int correctBuf (char * buf, int nElements);
static char ** arrPtrToStrings (int nStrs, size_t size);
static void fillArr (char ** arrPtrsToStrings, char * buf, int nElem, code_t code);
static void skipSpace (char ** strCode, int countLetters);
static void removeSpaces (char * dest, const char * source);

code_t readCode (const char * nameFile)
{
    MY_ASSERT (nameFile == nullptr, "You did not pass the file name");
    code_t fileInfo = {};
    fileInfo.sizeFile = size (nameFile);

    FILE * codeFile = fopen (nameFile, "r");
    MY_ASSERT (codeFile == nullptr, "Unable to open code file");
    setbuf(codeFile, NULL);
    writeTextToBuf (&fileInfo, codeFile);
    fileInfo.nStrs = correctBuf (fileInfo.buf, fileInfo.sizeFile);
    fileInfo.arrStrs = arrPtrToStrings (fileInfo.nStrs, sizeof(char *));
    fillArr (fileInfo.arrStrs, fileInfo.buf, fileInfo.sizeFile, fileInfo);
    for (int i = 0; i < fileInfo.nStrs; i++)
    {
        printf ("---final readCode: %s\n", fileInfo.arrStrs[i]);
    }

    fclose (codeFile);

    return fileInfo;
}

static size_t size (const char * argConsole)
{
    struct stat code = {};
    size_t val = stat (argConsole, &code);

    return code.st_size;
}

static void writeTextToBuf (code_t * code, FILE * codeFile)
{
    MY_ASSERT (code == nullptr, "There is no access to the struct with the necessary information");
    MY_ASSERT (codeFile == nullptr, "There is no access to the code file");

    code->buf = (char *) calloc (code->sizeFile + 1, sizeof(char));
    MY_ASSERT (code->buf == nullptr, "Unable to allocate new memory");

    size_t read_ok = fread (code->buf, sizeof(char), code->sizeFile,  codeFile);
    (code->buf)[code->sizeFile] = '\0';
}

static int correctBuf (char * buf, int nElements) 
{
	int nStrings = 0;

	for (int i = 0; i < nElements; i++) 
	{   
        if (buf[i] == ';')
        {
            while (buf[i] != '\n')
            {
                buf[i] = '\0';
                i++;
            }
            buf[i] = '\0'; 
            nStrings++; 
        }

		if (buf[i] == '\n')
		{
			buf[i] = '\0';
		}
        
	}
    printf ("nStrings = %d, buf = %s\n\n", nStrings, buf);
    for (int i = 0; i < nElements; i++)
    {
        printf ("%c", buf[i]);
    }
    printf ("\n");
	return nStrings;
}

static char ** arrPtrToStrings (int nStrs, size_t size)
{
	char ** array = (char **) calloc (nStrs, size);
	MY_ASSERT (array == NULL, "Memory allocation error\n");
	return array;
}

static void fillArr (char ** arrPtrsToStrings, char * buf, int nElem, code_t code) 
{
    skipSpace (&buf, 0);
	arrPtrsToStrings[0] = buf;
	for (int nSym = 0, j = 1; nSym < nElem && j < code.nStrs && *buf != EOF; nSym++) //added j < code.nStrs
	{
		if (*buf == '\0')
		{
            while (*buf == '\0')
            {
                buf++;
            }
            // for (int i = 0; i < 8; i++)
            // {
            //     printf ("buf[%d] = %d\n", i, buf[i]);
            // }
            skipSpace (&buf, 0);
            // printf ("after skipSpace: %s\n", buf);
			arrPtrsToStrings[j] = buf;
            // printf ("arrPtrsToStrings[%d] = %s\n", j, arrPtrsToStrings[j]);
			j++;
		}
		buf++;
	}
}

static void skipSpace (char ** strCode, int countLetters)
{
    *strCode = *strCode + countLetters;
    for (; **strCode == ' ' || **strCode == '\t' || **strCode == '\0'; ) //*strCode != nullptr && 
    {
        (*strCode)++;
    }
}
