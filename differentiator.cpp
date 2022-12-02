#include "differentiator.h"

#define dumpTexTree(text, ...)\
    fprintf (texfile, text, ##__VA_ARGS__)

static const char * TEX_PREVIEW = "\\documentclass[a4paper, 12pt]{article}\n"
                          "\\usepackage[a4paper,top=1.5cm, bottom=1.5cm, left=1cm, right=1cm]"
                          "{geometry}\n"
                          "\\usepackage[utf8]{inputenc}\n"
                          "\\usepackage{mathtext}                \n"
                          "\\usepackage[english, russian]{babel} \n"
                          "\\title{Matanchik}\n"
                          "\\author{Witcherok, DedSquad}\n"
                          "\\date{\\today}\n";

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

node_t * getGrammar (void)
{
    printf ("Enter the expression\n");
    char * string = (char *) calloc (50, sizeof(char));
    char * strFixed = (char *) calloc (50, sizeof(char));

    fgets (string, 50, stdin);

    removeSpaces (strFixed, string);

    node_t * firstNode = getExpression(&strFixed);
    if (*strFixed == '\0')
    {
        printf ("Fucking hands, they didn't write the expression again\n");
        return firstNode;
    }
    return firstNode;
}

node_t * getExpression (char ** str)
{
    node_t * leftNode = getT (str);
    while (**str == '+' || **str == '-')
    {
        char op = **str;
        (*str)++;
        node_t * rightNode = getT (str);

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

node_t * getT (char ** str)
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

node_t * getDegree (char ** str)
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

node_t * getBracket (char ** str)
{
    node_t * val = nullptr;
    if (**str == '(')
    {
        (*str)++;
        val = getExpression (str);
        if (**str == '(')
        {
            printf ("*(s+1) = %c\n", *(*str+1));
        }
        (*str)++;
    }
    else
    {
        val = getNumber (str);
    }
    return val;
}

node_t * getNumber (char ** str)
{
    if (isalpha(**str))
    {
        char buf[10] = "";
        sscanf (*str, "%[a-z]", buf);
        int wordLength = numOfLetters (buf);

        MY_ASSERT (wordLength == 0, "Failed line reading");
        (*str) = (*str)+wordLength;
        if (wordLength == 1)
        {
            return (createNodeWithVariable (buf[0]));
        }
        else
        {
            if (myStrcmp ((const char *) buf, "ln") == 0)
            {
                buf[0] = 'l';
                buf[1] = 'n';
                buf[2] = '\0';
            }
            printf ("buf = %s\n", buf);
            node_t * function = createNodeWithFunction (buf);
            node_t * followingExpression = getBracket (str);
            function->right = nullptr;
            function->left = followingExpression;
            followingExpression->parent = function;

            return (function);
        }
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
        assert (*str != sOld);
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
void simplifyExpression (node_t ** node)
{
    for (int i = 0; i < 3; i++)
    {
        removeConstants (node);

        simplifyDegOrDivToOne (node);

        simplifyMulByZero (*node);

        simplifyAddWithZero (node);

        simplifyMulByOne (node);
    }
}

void removeConstants (node_t ** node)
{
    MY_ASSERT ((*node) == nullptr, "There is no access to this node");
    if ((*node)->type == OPER_T && ((*node)->right != nullptr) && ((*node)->left != nullptr) &&
        (*node)->right->type == NUM_T && (*node)->left->type == NUM_T)
    {
        node_t * newNode = createNodeWithNum (1);
        if ((*node)->op_t == OP_ADD)
        {
            newNode->elem = (*node)->right->elem + (*node)->left->elem;
            deleteNode ((*node)->right);
            deleteNode ((*node)->left);
            deleteNode ((*node));
            (*node) = newNode;
        }
        else if ((*node)->op_t == OP_SUB)
        {
            newNode->elem = (*node)->right->elem + (*node)->left->elem;
            deleteNode ((*node)->right);
            deleteNode ((*node)->left);
            deleteNode ((*node));
            (*node) = newNode;
        }
        else if ((*node)->op_t == OP_MUL)
        {
            newNode->elem = (*node)->right->elem * (*node)->left->elem;
            deleteNode ((*node)->right);
            deleteNode ((*node)->left);
            deleteNode ((*node));
            (*node) = newNode;
        }
        else if ((*node)->op_t == OP_DIV)
        {
            newNode->elem = (*node)->right->elem / (*node)->left->elem;
            deleteNode ((*node)->right);
            deleteNode ((*node)->left);
            deleteNode ((*node));
            (*node) = newNode;
        }
        else if ((*node)->op_t == OP_DEG)
        {
            newNode->elem = pow ((*node)->right->elem, (*node)->left->elem);
            deleteNode ((*node)->right);
            deleteNode ((*node)->left);
            deleteNode ((*node));
            (*node) = newNode;
        }
        else
        {
            printf ("Mistake! There is no such operation\n");
        }
    }
    if (((*node)->left != nullptr) && ((*node)->left->left != nullptr) && ((*node)->left->right != nullptr) &&
        ((*node)->left->type == OPER_T) && ((*node)->left->left->type == NUM_T) && ((*node)->left->right->type == NUM_T))
    {
        node_t * newNode = createNodeWithNum (1);
        if ((*node)->left->op_t == OP_ADD)
        {
            newNode->elem = (*node)->left->left->elem + (*node)->left->right->elem;
            deleteNode ((*node)->left->left);
            deleteNode ((*node)->left->right);
            deleteNode ((*node)->left);
            (*node)->left = newNode;
        }
        else if ((*node)->left->op_t == OP_SUB)
        {
            newNode->elem = (*node)->left->left->elem - (*node)->left->right->elem;
            deleteNode ((*node)->left->left);
            deleteNode ((*node)->left->right);
            deleteNode ((*node)->left);
            (*node)->left = newNode;
        }
        else if ((*node)->left->op_t == OP_MUL)
        {
            newNode->elem = (*node)->left->left->elem * (*node)->left->right->elem;
            deleteNode ((*node)->left->left);
            deleteNode ((*node)->left->right);
            deleteNode ((*node)->left);
            (*node)->left = newNode;
        }
        else if ((*node)->left->op_t == OP_DIV)
        {
            newNode->elem = (*node)->left->left->elem / (*node)->left->right->elem;
            deleteNode ((*node)->left->left);
            deleteNode ((*node)->left->right);
            deleteNode ((*node)->left);
            (*node)->left = newNode;
        }
        else if ((*node)->left->op_t == OP_DEG)
        {
            newNode->elem = pow((*node)->left->left->elem, (*node)->left->right->elem);
            deleteNode ((*node)->left->left);
            deleteNode ((*node)->left->right);
            deleteNode ((*node)->left);
            (*node)->left = newNode;
        }
        else
        {
            printf ("Mistake! There is no such operation\n");
        }
    }
    if (((*node)->right != nullptr) && ((*node)->right->left != nullptr) && ((*node)->right->right != nullptr) &&
            ((*node)->right->left->type == NUM_T) && ((*node)->right->right->type == NUM_T) && ((*node)->right->type == OPER_T))
    {
        node_t * newNode = createNodeWithNum (1);
        if ((*node)->right->op_t == OP_ADD)
        {
            newNode->elem = (*node)->right->left->elem + (*node)->right->right->elem;
            deleteNode ((*node)->right->left);
            deleteNode ((*node)->right->right);
            deleteNode ((*node)->right);
            (*node)->right = newNode;
        }
        else if ((*node)->right->op_t == OP_SUB)
        {
            newNode->elem = (*node)->right->left->elem - (*node)->right->right->elem;
            deleteNode ((*node)->right->left);
            deleteNode ((*node)->right->right);
            deleteNode ((*node)->right);
            (*node)->right = newNode;
        }
        else if ((*node)->right->op_t == OP_MUL)
        {
            newNode->elem = (*node)->right->left->elem * (*node)->right->right->elem;
            deleteNode ((*node)->right->left);
            deleteNode ((*node)->right->right);
            deleteNode ((*node)->right);
            (*node)->right = newNode;
        }
        else if ((*node)->right->op_t == OP_DIV)
        {
            newNode->elem = (*node)->right->left->elem / (*node)->right->right->elem;
            deleteNode ((*node)->right->left);
            deleteNode ((*node)->right->right);
            deleteNode ((*node)->right);
            (*node)->right = newNode;
        }
        else if ((*node)->right->op_t == OP_DEG)
        {
            printf ("Here!\n");
            newNode->elem = pow ((*node)->right->left->elem, (*node)->right->right->elem);
            deleteNode ((*node)->right->left);
            deleteNode ((*node)->right->right);
            deleteNode ((*node)->right);
            (*node)->right = newNode;
        }
        else
        {
            printf ("Mistake! There is no such operation\n");
        }
    }
    if ((*node)->left != nullptr)
    {
        removeConstants (&((*node)->left));
    }
    if ((*node)->right != nullptr)
    {
        removeConstants (&((*node)->right));
    }
}

void simplifyDegOrDivToOne (node_t ** node)
{
    MY_ASSERT (node == nullptr, "There is no access to node for simplify");
    MY_ASSERT ((*node) == nullptr, "There is no access to node for simplify");

    if (((*node)->op_t == OP_DIV || (*node)->op_t == OP_DEG) && ((*node)->left != nullptr) && (compareFractionalNum((*node)->right->elem, 1) == 1))
    {
        node_t * saveLeftNode = (*node)->left;

        deleteNode ((*node)->right);
        deleteNode ((*node));

        (*node) = saveLeftNode;
    }

    if (((*node)->left != nullptr) && ((*node)->left->op_t == OP_DEG || (*node)->left->op_t == OP_DIV) &&
        ((*node)->left->right != nullptr) && (compareFractionalNum((*node)->left->right->elem, 1) == 1))
    {
        node_t * saveNodeLeftLeft = (*node)->left->left;

        deleteNode ((*node)->left->right);
        deleteNode ((*node)->left);

        (*node)->left = saveNodeLeftLeft;
    }
    else if (((*node)->right != nullptr) && ((*node)->right->op_t == OP_DEG || (*node)->right->op_t == OP_DIV) &&
            ((*node)->right->right != nullptr) && (compareFractionalNum((*node)->right->right->elem, 1) == 1))
    {
        node_t * saveNodeRightLeft = (*node)->right->left;
        deleteNode ((*node)->right->right);
        deleteNode ((*node)->right);

        (*node)->right = saveNodeRightLeft;
    }

    if ((*node)->left != nullptr)
    {
        simplifyDegOrDivToOne (&((*node)->left));
    }
    if ((*node)->right != nullptr)
    {
        simplifyDegOrDivToOne (&((*node)->right));
    }
}

void simplifyMulByOne (node_t ** node)
{
    MY_ASSERT (node == nullptr, "There is no access to node for simplify");
    MY_ASSERT ((*node) == nullptr, "There is no access to node for simplify");

    if (((*node)->op_t == OP_MUL) && (((*node)->right != nullptr) ) && (compareFractionalNum((*node)->right->elem, 1) == 1))
    {
        node_t * saveLeftNode = (*node)->left;

        deleteNode ((*node)->right);
        deleteNode ((*node));

        (*node) = saveLeftNode;
    }
    if (((*node)->op_t == OP_MUL) && ((*node)->left != nullptr) && (compareFractionalNum((*node)->left->elem, 1) == 1))
    {
        node_t * saveRightNode = (*node)->right;

        deleteNode ((*node)->left);
        deleteNode ((*node));

        (*node) = saveRightNode;
    }

    if (((*node)->left != nullptr) && ((*node)->left->op_t == OP_MUL))
    {
        if (((*node)->left->right != nullptr) && compareFractionalNum((*node)->left->right->elem, 1) == 1)
        {
            node_t * saveNodeLeftLeft = (*node)->left->left;

            deleteNode ((*node)->left->right);
            deleteNode ((*node)->left);

            (*node)->left = saveNodeLeftLeft;
        }
        if (((*node)->left->left != nullptr) && (compareFractionalNum((*node)->left->left->elem, 1) == 1))
        {
            node_t * saveNodeLeftRight = (*node)->left->right;

            deleteNode ((*node)->left->left);
            deleteNode ((*node)->left);

            (*node)->left = saveNodeLeftRight;
        }
    }
    if (((*node)->right != nullptr) && ((*node)->right->op_t == OP_MUL))
    {
        if (((*node)->right->right != nullptr) && (compareFractionalNum((*node)->right->right->elem, 1) == 1))
        {
            node_t * saveNodeLeftLeft = (*node)->right->left;

            deleteNode ((*node)->right->right);
            deleteNode ((*node)->right);

            (*node)->right = saveNodeLeftLeft;
        }
        if (((*node)->right->left != nullptr) && (compareFractionalNum((*node)->right->left->elem, 1) == 1))
        {
            node_t * saveNodeLeftRight = (*node)->right->right;

            deleteNode ((*node)->right->left);
            deleteNode ((*node)->right);

            (*node)->right = saveNodeLeftRight;
        }
    }

    if ((*node)->left != nullptr)
    {
        simplifyMulByOne (&((*node)->left));
    }
    if ((*node)->right != nullptr)
    {
        simplifyMulByOne (&((*node)->right));
    }
}

void simplifyAddWithZero (node_t ** node)
{
    MY_ASSERT ((*node) == nullptr, "There is no access to node for simplify");
    MY_ASSERT (node == nullptr, "There is no access to node for simplify");

    if ((*node)->op_t == OP_ADD)
    {
        if (((*node)->right != nullptr) && ((*node)->right->type == NUM_T) && (compareFractionalNum((*node)->right->elem, 0) == 1))
        {
            deleteNode ((*node)->right);
            (*node)->right = nullptr; //!!!!!!!!!!!!
            node_t * newNode = copyNode ((*node)->left);
            MY_ASSERT (newNode == nullptr, "The left node cannot be copied");
            (*node) = newNode;
        }
        if (((*node)->left != nullptr) && ((*node)->left->type == NUM_T) && (compareFractionalNum((*node)->left->elem, 0) == 1))
        {
            deleteNode ((*node)->left);
            (*node)->left = nullptr; //!!!!!!!!!!!! чтобы упрощать такой случай, нужно передать двойной указатель и работать с ним тут
            node_t * newNode = copyNode ((*node)->right);
            MY_ASSERT (newNode == nullptr, "The left node cannot be copied");
            (*node) = newNode;
        }
    }
        if (((*node)->left != nullptr) && ((*node)->left->op_t == OP_ADD))
        {
            if (((*node)->left->right != nullptr) && ((*node)->left->right->type == NUM_T) && (compareFractionalNum((*node)->left->right->elem, 0) == 1))
            {
                node_t * saveNodeLeftLeft = (*node)->left->left;

                deleteNode ((*node)->left->right);
                deleteNode ((*node)->left);

                (*node)->left = saveNodeLeftLeft;
            }
            if (((*node)->left->left != nullptr) && ((*node)->left->left->type == NUM_T) && (compareFractionalNum((*node)->left->left->elem, 0) == 1))
            {
                node_t * saveNodeLeftRight = (*node)->left->right;

                deleteNode ((*node)->left->left);
                deleteNode ((*node)->left);

                (*node)->left = saveNodeLeftRight;
            }
        }
        else if (((*node)->right != nullptr) && ((*node)->right->op_t == OP_ADD))
        {
            if (((*node)->right->right != nullptr) && ((*node)->right->right->type == NUM_T) && (compareFractionalNum((*node)->right->right->elem, 0) == 1))
            {
                node_t * saveNodeRightLeft = (*node)->right->left;
                deleteNode ((*node)->right->right);
                deleteNode ((*node)->right);

                (*node)->right = saveNodeRightLeft;
            }
            if (((*node)->right->left != nullptr) && ((*node)->right->left->type == NUM_T) && (compareFractionalNum((*node)->right->left->elem, 0) == 1))
            {
                node_t * saveNodeRightRight = (*node)->right->right;
                deleteNode ((*node)->right->left);
                deleteNode ((*node)->right);

                (*node)->right = saveNodeRightRight;
            }
        }


    if ((*node)->left != nullptr)
    {
        simplifyAddWithZero (&((*node)->left));
    }
    if ((*node)->right != nullptr)
    {
        simplifyAddWithZero (&((*node)->right));
    }
}

void simplifyMulByZero (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to this node");

    if ((node->op_t == OP_MUL) &&
        (((node->right != nullptr) && (node->right->type == NUM_T) && (compareFractionalNum(node->right->elem, 0) == 1)) ||
        ((node->left != nullptr) && (node->left->type == NUM_T) && (compareFractionalNum(node->left->elem, 0) == 1))))
    {
        deleteNode (node->right);
        deleteNode (node->left);

        node->elem = 0;
        node->left = nullptr;
        node->right = nullptr;
        node->op_t = WITHOUT_OP;
        node->type = NUM_T;
    }
    else
    {
        if ((node->left != nullptr) && (node->left->op_t == OP_MUL) &&
            (node->left->right->type == NUM_T) && (compareFractionalNum(node->left->right->elem, 0) == 1))
        {
            deleteNode (node->left->right);
            deleteNode (node->left);

            node_t * newNode = createNodeWithNum (0);
            node->left = newNode;
        }
        else if ((node->right != nullptr) && (node->right->op_t == OP_MUL) &&
                (node->right->right->type == NUM_T) && (compareFractionalNum(node->right->right->elem, 0) == 1))
        {
            deleteNode (node->right->right);
            deleteNode (node->right);

            node_t * newNode = createNodeWithNum (0);
            node->right = newNode;
        }
    }

    if (node->left != nullptr)
    {
        simplifyMulByZero (node->left);
    }
    if (node->right != nullptr)
    {
        simplifyMulByZero (node->right);
    }
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
    printf ("hello!\n");
    while (node->op_t == OP_MUL)
    {
        node_t * copyRightNode = copyNode (node->right);
        printf ("now graphic for \"copyRightNode\"\n");
        graphicDumpTree (copyRightNode);

        node_t * copyLeftNode = copyNode (node->left);
        printf ("now graphic for \"copyLeftNode\"\n");
        graphicDumpTree (copyLeftNode);

        node_t * firstNode = createNodeWithOperation (OP_MUL, node->left, node->right);

        printf ("now graphic for \"firstNode\"\n");
        graphicDumpTree (firstNode);

        node_t * difLeftNodeForMul = getExpressionForDif (node->left);
        firstNode->left = difLeftNodeForMul;
        firstNode->right = copyRightNode;
        difLeftNodeForMul->parent = firstNode;
        copyRightNode->parent = firstNode;

        printf ("now graphic for \"difLeftNodeForMul\"\n");
        graphicDumpTree (difLeftNodeForMul);

        node_t * secondNode = createNodeWithOperation (OP_MUL, node->left, node->right);
        node_t * difRightNodeForMul = getExpressionForDif (node->right);
        secondNode->left = copyLeftNode;
        secondNode->right = difRightNodeForMul;
        difRightNodeForMul->parent = secondNode;
        copyLeftNode->parent = secondNode;

        printf ("now graphic for \"secondNode\"\n");
        graphicDumpTree (secondNode);

        printf ("now graphic for \"difRightNodeForMul\"\n");
        graphicDumpTree (difRightNodeForMul);

        node_t * headNodeForMul = createNodeWithOperation (OP_ADD, firstNode, secondNode);
        printf ("now graphic for \"headNodeForMul\"\n");
        graphicDumpTree (headNodeForMul);

        return headNodeForMul;
    }
    while (node->op_t == OP_DIV)
    {
        node_t * copyRightNode = copyNode (node->right);
        node_t * copyLeftNode = copyNode (node->left);

        node_t * firstNode = createNodeWithOperation (OP_MUL, copyLeftNode, copyRightNode); //? (OP_MUL, node->left, node->right)
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

        node_t * headNodeForNumerator = createNodeWithOperation (OP_SUB, firstNode, secondNode);

        node_t * degreeOfDenominator = createNodeWithNum (2);
        node_t * copiedRightNode = copyNode (copyRightNode);
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

            node_t * firstCopyLeftNode = copyNode (node->left);

            node_t * secondCopyLeftNode = copyNode (node->left);

            node_t * numForDegreeIndicator = createNodeWithNum (1);

            node_t * newDegreeIndicator = createNodeWithOperation (OP_SUB, copyRightNodeForDegree, numForDegreeIndicator);
            node_t * newDegree = createNodeWithOperation (OP_DEG, firstCopyLeftNode, newDegreeIndicator);
            node_t * leftDescendant = createNodeWithOperation (OP_MUL, copyRightNodeForExpression, newDegree);

            node_t * rightDescendant = getExpressionForDif (secondCopyLeftNode);
            node_t * headNodeForDeg = createNodeWithOperation (OP_MUL, leftDescendant, rightDescendant);

            return headNodeForDeg;
        }
        else
        {
            node_t * leftDescendant = copyNode (node);
            node_t * copyRight = copyNode (node->right);
            node_t * nodeForLog = createNodeWithFunction ((char *) "ln");
            nodeForLog->left = copyNode (node->left);

            node_t * difExp = getExpressionForDif (copyRight);

            node_t * rightDescendant = createNodeWithOperation (OP_MUL, nodeForLog, difExp);

            node_t * headNodeForExpFunc = createNodeWithOperation (OP_MUL, leftDescendant, rightDescendant);

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
    else if (node->type == FUNC_T)
    {
        return (difFunc (node));
    }
    else
    {
        return node;
    }
}

node_t * difFunc (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to this node");

    if (myStrcmp (node->nameFunc, "ln") == 0)
    {
        node_t * unit = createNodeWithNum (1);
        node_t * copyArgument = copyNode (node->left);
        node_t * leftDescendant = createNodeWithOperation (OP_DIV, unit, copyArgument);
        node_t * rightDescendant = getExpressionForDif (node->left);
        node_t * headNode = createNodeWithOperation (OP_MUL, leftDescendant, rightDescendant);

        return headNode;
    }

    else if (myStrcmp (node->nameFunc, "sin") == 0)
    {
        node_t * leftDescendant = createNodeWithFunction ((char *) "cos");
        node_t * copyArgument = copyNode (node->left);
        leftDescendant->left = copyArgument;
        copyArgument->parent = leftDescendant;
        node_t * rightDescendant = getExpressionForDif (node->left);
        node_t * headNode = createNodeWithOperation (OP_MUL, leftDescendant, rightDescendant);
        rightDescendant->parent = headNode;

        return headNode;
    }
    else if (myStrcmp (node->nameFunc, "cos") == 0)
    {
        node_t * unit = createNodeWithNum (-1);
        node_t * copyArgument = copyNode (node->left);
        node_t * difCos = createNodeWithFunction ((char *) "sin");
        difCos->left = copyArgument;
        copyArgument->parent = difCos;
        node_t * leftDescendant = createNodeWithOperation (OP_MUL, unit, difCos);
        node_t * rightDescendant = getExpressionForDif (node->left);
        node_t * headNode =  createNodeWithOperation (OP_MUL, leftDescendant, rightDescendant);

        return headNode;
    }
    else if (myStrcmp (node->nameFunc, "tg") == 0)
    {
        node_t * unit = createNodeWithNum (1);
        node_t * degIndicator = createNodeWithNum (2);
        node_t * copyArgument = copyNode (node->left);
        node_t * difSin = createNodeWithFunction ((char *) "cos");
        difSin->left = copyArgument;
        copyArgument->parent = difSin;
        node_t * degree = createNodeWithOperation (OP_DEG, difSin, degIndicator);
        node_t * leftDescendant = createNodeWithOperation (OP_DIV, unit, degree);
        node_t * rightDescendant = getExpressionForDif (node->left);
        node_t * headNode = createNodeWithOperation (OP_MUL, leftDescendant, rightDescendant);

        return headNode;
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
}
//---------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------dump to a texfile------------------------------------------------

void dumpToTexFile (node_t * node)
{
    FILE * texfile = fopen("Derivative.tex", "w");
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    setbuf (texfile, nullptr);

    texStart (texfile);

    texPrintNode (texfile, node);

    texFinish (texfile);
}

void texStart (FILE * texfile)
{
    dumpTexTree (TEX_PREVIEW);
    dumpTexTree ("\\begin{document}\n");
    // dumpTexTree ("\n\\maketitle\n");
    dumpTexTree ("$f(x) = ");
}


void texPrintNode (FILE * texfile, node_t * node)
{
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    if (node->left != nullptr && node->right != nullptr)
    {
        dumpTexTree ("{");
        texPrintNode (texfile, node->left);
    }

    switch (node->type)
    {
        case OPER_T:
            texPrintOperation (texfile, node);
            break;
        case VAR_T:
            texPrintVar (texfile, node);
            break;
        case NUM_T:
            texPrintNum (texfile, node);
            break;
        case FUNC_T:
            texPrintFunc (texfile, node);
            break;
        case CONST_T:
            texPrintConst (texfile, node);
            break;
        default:
            fprintf (stderr, "Non-existent node type\n");
            break;
    }

    if (node->left != nullptr)
    {
        texPrintNode (texfile, node->left);
        dumpTexTree ("}");
    }
    if (node->right != nullptr)
    {
        texPrintNode (texfile, node->right);
        dumpTexTree ("}");
    }
}

void texPrintOperation (FILE * texfile, node_t * node)
{
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    switch (node->op_t)
    {
        case OP_ADD:
            dumpTexTree (" %c ", OP_ADD);
            break;
        case OP_SUB:
            dumpTexTree (" %c ", OP_ADD);
            break;
        case OP_MUL:
            dumpTexTree (" %c ", OP_MUL);
            break;
        case OP_DIV:
            dumpTexTree (" %c ", OP_DIV);
            break;
        case OP_DEG:
            dumpTexTree (" %c ", OP_DEG);
            break;
        default:
            printf ("There is no such node\n");
            break;
    }
}

void texPrintVar (FILE * texfile, node_t * node)
{
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpTexTree ("%c", node->varName);
}

void texPrintNum (FILE * texfile, node_t * node)
{
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpTexTree ("%.2lf", node->elem);
}

void texPrintFunc (FILE * texfile, node_t * node)
{
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpTexTree ("%s", node->nameFunc);
}

void texPrintConst (FILE * texfile, node_t * node)
{
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpTexTree ("%c", node->varName);
}

int texFinish(FILE * texfile)
{
    dumpTexTree("$\n\\end{document}\n");

    fclose(texfile);

    return 0;
}

