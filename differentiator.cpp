#include "differentiator.h"

#define dumpTexTree(text, ...)\
    fprintf (latexFile, text, ##__VA_ARGS__);

//-------------------------------------------------------------building a tree-------------------------------------------------------
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
        printf ("Fucking hands, they didn't write the expression again\n");
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
    if (isalpha(*s))
    {
        char buf[10] = "";
        sscanf (s, "%[a-z]", buf);
            printf ("after sscanf: buf = %s\n", buf);

        int wordLength = numOfLetters (buf);

        MY_ASSERT (wordLength == 0, "Failed line reading");
        s = s+wordLength;
        printf ("after read func *s = %c\n", *s);
        if (wordLength == 1)
        {
            return (createNodeWithVariable (buf[0]));
        }
        else
        {
            if (myStrcmp ((const char *) buf, "ln") == 0)
            {
                buf[0] = 'l';
                buf[1] = 'o';
                buf[2] = 'g';
                buf[3] = '\0';
            }
            printf ("buf = %s\n", buf);
            node_t * function = createNodeWithFunction (buf);
            node_t * followingExpression = getBracket ();
            function->right = nullptr;
            function->left = followingExpression;
            followingExpression->parent = function;

            return (function);
        }
    }
    else
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
}
//----------------------------------------------------------------------------------------------------------------------------------


//---------------------------------------------dump to console----------------------------------------------------------------------

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

//----------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------support functions---------------------------------------------------------
int numOfLetters (const char * string)
{
    MY_ASSERT (string == nullptr, "There is no access to string");

    int number = 0;

    while (*string != '\0')
    {
        number++;
        string++;
    }
    return number;
}
//---------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------simplify---------------------------------------------------------
void simplifyExpToOne (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to node for simplify");
    printf ("There!\n");
    if ((node->left != nullptr) && (node->left->op_t == OP_DEG) && (compareFractionalNum(node->left->right->elem, 1) == 1))
    {
        node_t * saveNodeLeftLeft = node->left->left;

        deleteNode (node->left->right);
        deleteNode (node->left);

        node->left = saveNodeLeftLeft;
        printf ("node->left->elem = %lf\n", node->left->elem);
    }
    else if ((node->right != nullptr) && (node->right->op_t == OP_DEG) && (compareFractionalNum(node->right->right->elem, 1) == 1))
    {
        node_t * saveNodeRightLeft = node->right->left;
        printf ("node->right->op_t = %c\n", node->right->op_t);
        deleteNode (node->right->right);
        deleteNode (node->right);

        node->right = saveNodeRightLeft;
        printf ("node->right->elem1111 = %lf\n", node->right->elem);
        printf ("Here!1111\n");
    }
    if (node->left != nullptr)
    {
        printf ("once again in this function\n");
        simplifyExpToOne (node->left);
        printf ("exited this function\n");

    }

    printf ("Here!");

    if (node->right != nullptr)
        simplifyExpToOne (node->right);
}
//---------------------------------------------------------------------------------------------------------------------------------

int compareFractionalNum (double firstNum, double secondNum)
{
    if (abs(firstNum-secondNum) < 0.00001)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void deleteNode (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");
    free (node);
}

//--------------------------------------------creating a tree after differentiation------------------------------------------------
node_t * getGrammarForDif (node_t * node) //возможно, когда сделаешь двойной указатель, потом начнешь вылезать за строку из-за сдвига
{
    MY_ASSERT (node == nullptr, "There is no access to the node");

    printf ("in getGrammarForDif\n");

    node_t * firstNode = getExpressionForDif(node);

    return firstNode;
}

node_t * getExpressionForDif (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");
    while (node->op_t == OP_ADD || node->op_t == OP_SUB)
    {
        node_t * leftNode = getExpressionForDif (node->left);
        node_t * rightNode = getExpressionForDif (node->right);
        node->left = leftNode;
        node->right = rightNode;
        leftNode->parent = node;
        rightNode->parent = node;
        return node;

    }
    return (difMulDiv (node));
}

node_t * difMulDiv (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");

    while (node->op_t == OP_MUL)
    {
        node_t * copyRightNode = copyNode (node->right);
        node_t * copyLeftNode = copyNode (node->left);

        node_t * firstNode = createNodeWithOperation (OP_MUL, node->left, node->right);
        node_t * difLeftNodeForMul = getExpressionForDif (node->left);
        firstNode->left = difLeftNodeForMul;
        firstNode->right = copyRightNode;
        difLeftNodeForMul->parent = firstNode;
        copyRightNode->parent = firstNode;

        node_t * secondNode = createNodeWithOperation (OP_MUL, node->left, node->right);
        node_t * difRightNodeForMul = getExpressionForDif (node->right);
        secondNode->left = copyLeftNode;
        secondNode->right = difRightNodeForMul;
        difRightNodeForMul->parent = secondNode;
        copyLeftNode->parent = secondNode;

        node_t * headNodeForMul = createNodeWithOperation (OP_ADD, firstNode, secondNode);
        return headNodeForMul;
    }
    while (node->op_t == OP_DIV)
    {
        graphicDumpTree (node->right);
        node_t * copyRightNode = copyNode (node->right);
        graphicDumpTree (copyRightNode);
        node_t * copyLeftNode = copyNode (node->left);

        node_t * firstNode = createNodeWithOperation (OP_MUL, copyLeftNode, copyRightNode); //? (OP_MUL, node->left, node->right)
        node_t * difLeftNodeForMul = getExpressionForDif (node->left);
        firstNode->left = difLeftNodeForMul;
        firstNode->right = copyRightNode;
        difLeftNodeForMul->parent = firstNode;
        copyRightNode->parent = firstNode;

        node_t * secondNode = createNodeWithOperation (OP_MUL, node->left, node->right); //(OP_MUL, node->left, node->right)
        node_t * difRightNodeForMul = getExpressionForDif (node->right);
        secondNode->left = copyLeftNode;
        secondNode->right = difRightNodeForMul;
        difRightNodeForMul->parent = secondNode;
        copyLeftNode->parent = secondNode;

        node_t * headNodeForNumerator = createNodeWithOperation (OP_SUB, firstNode, secondNode);

        graphicDumpTree (headNodeForNumerator);

        node_t * degreeOfDenominator = createNodeWithNum (2);
        node_t * copiedRightNode = copyNode (copyRightNode); //node->right
        node_t * headNodeForDenominator = createNodeWithOperation (OP_DEG, copiedRightNode, degreeOfDenominator);

        node_t * headNodeForDiv = createNodeWithOperation (OP_DIV, headNodeForNumerator, headNodeForDenominator);

        return headNodeForDiv;
    }
    return (difDegree(node));
}

node_t * difDegree (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");

    while (node->op_t == OP_DEG)
    {
        if (node->right->type == NUM_T)
        {
            node_t * copyRightNodeForExpression = copyNode (node->right);
            node_t * copyRightNodeForDegree = copyNode (node->right);
            node_t * copyLeftNode = copyNode (node->left);

            node_t * numForDegreeIndicator = createNodeWithNum (1);

            node_t * newDegreeIndicator = createNodeWithOperation (OP_SUB, copyRightNodeForDegree, numForDegreeIndicator);
            node_t * newDegree = createNodeWithOperation (OP_DEG, copyLeftNode, newDegreeIndicator);
            node_t * headNodeForDeg = createNodeWithOperation (OP_MUL, copyRightNodeForExpression, newDegree);

            return headNodeForDeg;
        }
        else
        {
            node_t * copyOfExpression = copyNode (node);
            node_t * nodeForLog = createNodeWithFunction ((char *) "log");
            nodeForLog->left = createNodeWithVariable ('e'); //!!!createNodeWithConst
            nodeForLog->right = copyNode (node->left);
            node_t * headNodeForExpFunc = createNodeWithOperation (OP_MUL, copyOfExpression, nodeForLog);

            return headNodeForExpFunc;
        }
    }
    return (difNumberOrVar(node));
}

node_t * difNumberOrVar (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");
    if (node->type == NUM_T)
    {
        return (createNodeWithNum(0));
    }
    else if (node->type == VAR_T)
    {
        return (createNodeWithNum(1));
    }
    else
    {
        return node;
    }
}
//---------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------dump to file------------------------------------------------------------
void selectingNameOfLatexFile (void)
{
    printf ("Please, enter the name of the LaTeX file file with the \".tex\" extension\n");
    char * nameLatexFile = (char *) calloc (30, sizeof(char));
    fgets (nameLatexFile, 29, stdin);

    FILE * latexFile = fopen (nameLatexFile, "w");
    MY_ASSERT (latexFile == nullptr, "There is no access to LaTeX file");

    dumpTexTree ("Hello!\n");

}
//---------------------------------------------------------------------------------------------------------------------------------

