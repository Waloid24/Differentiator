#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "my_assert.h"
const int stringLength = 200;

char * fullBracketEntry (char ** str, char ** strWithBracket);
int getExpression (char ** str, char ** strWithBracket);
int getT (char ** str, char ** strWithBracket);
int getDegree (char ** str, char ** strWithBracket);
int getBracket (char ** str, char ** strWithBracket);
int getNumber (char ** str, char ** strWithBracket);

static void removeSpaces (char * dest, const char * source)
{
    while (*source != '\0')
    {
        if (*source == ' ')
        {
            source++;
            removeSpaces(dest, source);
        }
        else
        {
        *dest = *source;
        dest++;
        source++;
        }
    }
}

int main (void)
{
    printf ("Please, enter the expression\n");
    char * string = (char *) calloc (stringLength, sizeof(char));
    char * strFixed = (char *) calloc (stringLength, sizeof(char));
    char * strWithBracket = (char *) calloc (stringLength*2, sizeof(char));

    char * lol1 = strWithBracket;
    char * lolStart = strFixed;

    fgets (string, 40, stdin);
    printf ("string = %s", string);

    removeSpaces (strFixed, string);
    printf ("strFixed = %s\n", strFixed);
    fullBracketEntry (&strFixed, &strWithBracket);

    printf ("strWithBracket = %s\n", strWithBracket);
    printf ("strWithBracket = %s\n", lol1);

    //createTree (strFixed);
}

char * fullBracketEntry (char ** str, char ** strWithBracket)
{
    MY_ASSERT (str == nullptr, "There is no access to expression");
    int val = getExpression(str, strWithBracket);
    **strWithBracket = '\0';
    if (**str == '\0')
    {
        printf ("ERROR\n");
        return *strWithBracket;
    }
    return *strWithBracket;
}

int getExpression (char ** str, char ** strWithBracket)
{
    MY_ASSERT (*str == nullptr, "Error in getting the source data");

    // **strWithBracket = '(';
    // (*strWithBracket)++;

    int val = getT(str, strWithBracket);

    // **strWithBracket = '(';
    printf ("in getExpression1: strWithBracket = %s\n", *strWithBracket);
    printf ("in getExpression: str = %s\n", *str);
    char * lol1 = *strWithBracket;
    // (*strWithBracket)++;

    while (**str == '+' || **str == '-')
    {
        // **strWithBracket = '(';
        // (*strWithBracket)++;

        **strWithBracket = **str;
        printf ("**strWithBracket = %c\n", **strWithBracket);
        (*strWithBracket)++;
        (*str)++;
        int val2 = getT (str, strWithBracket);

        **strWithBracket = ')';
        (*strWithBracket)++;
    }

    // **strWithBracket = ')';
    printf ("in getExpression2: strWithBracket = %s\n", lol1);
    // (*strWithBracket)++;

    return val;
}

int getT (char ** str, char ** strWithBracket)
{
    **strWithBracket = '(';
    (*strWithBracket)++;

    int val = getDegree (str, strWithBracket);
    // **strWithBracket = '(';
    printf ("in getT1: strWithBracket = %s\n", *strWithBracket);
    char * lol1 = *strWithBracket;
    // (*strWithBracket)++;
    while (**str == '*' || **str == '/')
    {
        // **strWithBracket = '(';
        // (*strWithBracket)++;

        **strWithBracket = **str;
        (*str)++;
        (*strWithBracket)++;
        int val2 = getDegree (str, strWithBracket); //getT

        **strWithBracket = ')';
        (*strWithBracket)++;

    }
    // **strWithBracket = ')';
    printf ("in getT2: strWithBracket = %s\n", lol1);
    // (*strWithBracket)++;
    return val;
}

int getDegree (char ** str, char ** strWithBracket)
{
    int val = getBracket(str, strWithBracket);
    // **strWithBracket = '(';
    printf ("in getDegree1: strWithBracket = %s\n", *strWithBracket);
    char * lol1 = *strWithBracket;
    // (*strWithBracket)++;
    printf ("strWithBracket = %c\n", **strWithBracket);

    while (**str == '^')
    {
        **strWithBracket = '(';
        (*strWithBracket)++;

        **strWithBracket = **str;
        (*str)++;
        (*strWithBracket)++;
        int val2 = getBracket(str, strWithBracket);

        **strWithBracket = ')';
        (*strWithBracket)++;
    }

    // **strWithBracket = ')';
    printf ("in getDegree2: strWithBracket = %s\n", lol1);
    // (*strWithBracket)++;

    return val;
}

int getBracket (char ** str, char ** strWithBracket)
{
    printf ("in getBracket1: strWithBracket = %s\n", *strWithBracket);
    char * lol1 = *strWithBracket;
    int val = 0;
    if (**str == '(')
    {
        **strWithBracket = **str;
        (*strWithBracket)++;
        (*str)++;
        val = getExpression (str, strWithBracket);
        (*str)++;
    }
    else
    {
        val = getNumber (str, strWithBracket);
    }
    printf ("in getBracket2: strWithBracket = %s\n", lol1); //lol1
    return val;
}

int getNumber (char ** str, char ** strWithBracket)
{
    printf ("in getNumber1: strWithBracket = %s\n", *strWithBracket);
    char * lol1 = *strWithBracket;
    **strWithBracket = '(';
    (*strWithBracket)++;

    int val = 0;
    const char * sOld = *str;

    while ('0' <= **str && **str <= '9')
    {
        **strWithBracket = **str;
        (*str)++;
        (*strWithBracket)++;
    }

    assert (*str != sOld);

    **strWithBracket = ')';
    printf ("in getNumber2: strWithBracket = %s\n", lol1); //lol1
    (*strWithBracket)++;

    return val;
}
