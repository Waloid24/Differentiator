#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int getGrammar (const char * str);
int getExpression (void);
int getT (void);
int getBracket (void);
int getNumber (void);
void removeSpaces (char * dest, const char * source);
int getDegree (void);

const char * s = nullptr;

int main (void)
{
    printf ("Enter the expression\n");
    char * string = (char *) calloc (40, sizeof(char));
    char * strFixed = (char *) calloc (40, sizeof(char));

    fgets (string, 40, stdin);
    printf ("string = %s", string);

    removeSpaces (strFixed, string);
    printf ("strFixed = %s\n", strFixed);

    int value = getGrammar (strFixed);
    printf ("value = %d\n", value);
}

void removeSpaces (char * dest, const char * source)
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

int getGrammar (const char * str)
{
    printf ("in getGrammar: str = %s\n", str);
    s = str;
    int val = getExpression();
    if (*s == '\0')
    {
        printf ("ERROR\n");
        return val;
    }
    return val;
}

int getExpression (void)
{
    int val = getT();
    while (*s == '+' || *s == '-')
    {
        char op = *s;
        s++;
        int val2 = getT ();

        if (op == '+')
            val += val2;
        else
            val -= val2;
    }
    return val;
}

int getT (void)
{
    int val = getDegree ();
    while (*s == '*' || *s == '/')
    {
        char op = *s;
        s++;
        int val2 = getDegree (); //getT

        if (op == '*')
        {
            val *= val2;
        }
        else
        {
            val /= val2;
        }
    }
    return val;
}

int getDegree (void)
{
    int val = getBracket();
    printf ("in getDegree: val = %d\n", val);
    while (*s == '^')
    {
        char op = *s;
        s++;
        int val2 = getBracket();
        int startingValue = val;
        printf ("in getDegree: val2 = %d\n", val2);

        for (int i = 1; i < val2; i++)
        {
            val *= startingValue;
            printf ("val = %d\n", val);
        }
    }
    printf ("in getDegree after while: %d\n", val);
    return val;
}

int getBracket (void)
{
    int val = 0;
    if (*s == '(')
    {
        s++;
        val = getExpression ();
        if (*s == '(')
        {
            printf ("*(s+1) = %c\n", *(s+1));
        }
        // assert (*s == '(');
        s++;
    }
    else
    {
        val = getNumber ();
    }
    return val;
}

int getNumber (void)
{
    int val = 0;
    const char * sOld = s;
    while ('0' <= *s && *s <= '9')
    {
        val = val*10 + *s - '0';
        s++;
    }
    assert (s != sOld);
    return val;
}


