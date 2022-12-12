#include "processExpr.h"

static char * readExpression (void);
static char * readString (char ** str);
static void removeSpaces (char * dest, const char * source);
static node_t * getExpression (char ** str);
static node_t * getT (char ** str);
static node_t * getDegree (char ** str);
static node_t * getBracket (char ** str);
static node_t * getNumber (char ** str);

static void removeSpaces (char * dest, const char * source)
{
    while (*source != '\0')
    {
        while (*source == ' ' || *source == '\t')
        {
            source++;
        }
        *dest = *source;
        dest++;
        source++;
    }
}

node_t * getGrammar (void)
{
    char * strFixed = readExpression ();
    printf ("strFixed = %s\n", strFixed);
    char * saveStrFixed = strFixed;
    node_t * firstNode = getExpression(&strFixed);

    MY_ASSERT (*strFixed == '\0', "The processing line is empty");

    free (saveStrFixed);
    return firstNode;
}

static node_t * getExpression (char ** str)
{
    node_t * leftNode = getT (str);
    while (1)
    {
        operationType op = OP_ERROR;
        if (**str == '+')
        {
            op = OP_ADD;
            (*str)++;
            node_t * rightNode = getT (str);
            leftNode = createNodeWithOperation (OP_ADD, leftNode, rightNode);
        }
        else if (**str == '-')
        {
            op = OP_SUB;
            (*str)++;
            node_t * rightNode = getT (str);
            leftNode = createNodeWithOperation (OP_SUB, leftNode, rightNode);
        }
        else
        {
            break;
        }
    }
    return leftNode;
}

static node_t * getT (char ** str)
{
    node_t * leftNode = getDegree (str);
    while (**str == '*' || **str == '/')
    {
        char op = **str;
        (*str)++;
        node_t * rightNode = getDegree (str);

        if (op == '*')
        {
            leftNode = createNodeWithOperation (OP_MUL, leftNode, rightNode);
        }
        else
        {
            leftNode = createNodeWithOperation (OP_DIV, leftNode, rightNode);
        }
    }
    return leftNode;
}

static node_t * getDegree (char ** str)
{
    node_t * leftNode = getBracket(str);
    while (**str == '^')
    {
        char op = **str;
        (*str)++;
        node_t * rightNode = getBracket(str);

        leftNode = createNodeWithOperation (OP_DEG, leftNode, rightNode);
    }
    return leftNode;
}

static node_t * getBracket (char ** str)
{
    node_t * val = nullptr;
    if (**str == '(')
    {
        (*str)++;
        val = getExpression (str);
        (*str)++;
    }
    else
    {
        val = getNumber (str);
    }
    return val;
}

static node_t * getNumber (char ** str)
{
    if (isalpha(**str))
    {
        char * buf = readString (str);

        int wordLength = strlen (buf);
        MY_ASSERT (wordLength == 0, "Failed line reading");
        (*str) = (*str)+wordLength;

        if (wordLength == 1)
        {
            // printf ("buf[0] = %c, buf[1] = %c\n", buf[0], buf[1]);
            return (createNodeWithVariable (buf[0]));
        }
        else
        {
            node_t * followingExpression = getBracket (str);
            node_t * function = createNodeWithFunction (buf, followingExpression);
            return (function);
        }
        free (buf);
    }
    else if (**str == '-' && isalnum(*(*str+1)))
    {
        (*str)++;
        node_t * varOrNum = getNumber(str);
        node_t * unit = createNodeWithNum (-1);
        varOrNum = createNodeWithOperation (OP_MUL, unit, varOrNum);
        return varOrNum;
    }
    else
    {
        int val = 0;
        const char * sOld = *str;
        while ('0' <= **str && **str <= '9')
        {
            val = val*10 + **str - '0';
            (*str)++;
        }
        MY_ASSERT (*str == sOld, "Wrong pointer");
        return (createNodeWithNum (val));
    }
}

static char * readExpression (void)
{
    printf ("Enter the expression\n");
    char * string = (char *) calloc (50, sizeof(char));
    MY_ASSERT (string == nullptr, "Unable to allocate new memory");
    char * strFixed = (char *) calloc (50, sizeof(char));
    MY_ASSERT (strFixed == nullptr, "Unable to allocate new memory");

    fgets (string, 50, stdin);

    removeSpaces (strFixed, string);

    free (string);
    return strFixed;
}

static char * readString (char ** str)
{
    MY_ASSERT (str == nullptr, "There is no access to this string");

    char * buf = (char *) calloc (10, sizeof(char));
    sscanf (*str, "%[a-z]", buf);

    return buf;
}
