#include "differentiator.h"

const char * s = nullptr;

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

node_t * getGrammar (const char * str)
{
    printf ("in getGrammar: str = %s\n", str);
    s = str;
    node_t * firstNode = getExpression();
    if (*s == '\0')
    {
        printf ("ERROR\n");
        return firstNode;
    }
    return firstNode;
}

node_t * getExpression (void)
{
    node_t * leftNode = getT();
    while (*s == '+' || *s == '-')
    {
        char op = *s;
        s++;
        node_t * rightNode = getT ();

        if (op == '+')
        {
            leftNode = createNodeWithOperation (OP_ADD, leftNode, rightNode);
        }
        else
        {
            leftNode = createNodeWithOperation (OP_SUB, leftNode, rightNode);
        }

    }
    return leftNode;
}

node_t * getT (void)
{
    node_t * leftNode = getDegree ();
    while (*s == '*' || *s == '/')
    {
        char op = *s;
        s++;
        node_t * rightNode = getDegree ();

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

node_t * getDegree (void)
{
    node_t * leftNode = getBracket();
    while (*s == '^')
    {
        char op = *s;
        s++;
        node_t * rightNode = getBracket();

        leftNode = createNodeWithOperation (OP_DEG, leftNode, rightNode);
    }
    return leftNode;
}

node_t * getBracket (void)
{
    node_t * val = nullptr;
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

node_t * getNumber (void)
{
    int val = 0;
    const char * sOld = s;
    while ('0' <= *s && *s <= '9')
    {
        val = val*10 + *s - '0';
        s++;
    }
    assert (s != sOld);
    return (createNodeWithNum (val));
}

//---------------------------------------------dump to console--------------------------------------------------
void textDump (node_t * node, FILE * log, unsigned int isLast, unsigned int numTABs)
{
 	MY_ASSERT (node == nullptr, "There is no access to the node of the tree");
	MY_ASSERT (log == nullptr, "There is no access to the logfile");

	printfTab (numTABs, log);
	if (isLast == NO_LAST)
	{
		fprintf (log, "%lf\n", node->elem);
		if (node->left->left == nullptr && node->left->right == nullptr)
		{
			textDump (node->left, log, LAST, ++numTABs);
		}
		else
		{
			textDump (node->left, log, NO_LAST, ++numTABs);
		}

		if (node->right->left == nullptr && node->right->right == nullptr)
		{
			textDump (node->right, log, LAST, numTABs);
		}
		else
		{
			textDump (node->right, log, NO_LAST, numTABs);
		}
	}
	else
		fprintf (log, ".%lf\n", node->elem);
}

void printfTab (unsigned int numTABs, FILE * log)
{
	MY_ASSERT (log == nullptr, "There is no access to the logfile");

	for (unsigned int i = 0; i < numTABs; i++)
	{
		fprintf (log, "  ");
	}
}
//-----------------------------------------------------------------------------------------------------------
