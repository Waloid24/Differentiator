#include "differentiator.h"

static var_t findNameVar (const node_t * node);

#define dumpTexTree(text, ...)\
    fprintf (texfile, text, ##__VA_ARGS__)

#define dumpPython(text, ...)\
    fprintf (pyfile, text, ##__VA_ARGS__)

static const char * TEX_OPTIONS =
"\\documentclass[12pt,a4paper]{article}\n"
"\\usepackage[utf8x]{inputenc}\n"
"\\usepackage{ucs}\n"
"\\usepackage{diagbox}\n"
"\\usepackage[english,russian]{babel}\n"
"\\usepackage[OT1]{fontenc}\n"
"\\usepackage{amsmath}\n"
"\\usepackage{amsfonts}\n"
"\\usepackage{amssymb}\n"
"\\usepackage{wasysym}\n"
"\\usepackage{wrapfig}\n"
"\\usepackage{physics}\n"
"\\usepackage[normalem]{ulem}\n"
"\\usepackage[left=2cm,right=2cm,top=2cm,bottom=2cm,includefoot,footskip=1.5cm]{geometry}\n"
"\\usepackage{mathrsfs}\n"
"\\usepackage{graphicx}\n"
"\\graphicspath{{pictures/}}\n"
"\\DeclareGraphicsExtensions{.png,.jpg,.jpeg}\n"
"\\usepackage{indentfirst}\n"
"\\usepackage[T2A]{fontenc}\n"
"\\usepackage{subfigure}\n"
"\\usepackage{fancyhdr}\n"
"\\pagestyle{fancy}\n"
"\\usepackage{caption}\n"
"\\captionsetup[table]{justification=}\n"
"\\usepackage{tabularx}\n"
"\\usepackage{floatrow}\n"
"\\usepackage{multicol}\n"
"\\usepackage[dvipsnames]{color}\n"
"\\usepackage{colortbl}\n\n";

static const char * TEX_TITLE_PAGE =
"\\begin{titlepage}\n"
    "\\begin{center}\n"
    "\\large Московский физико-технический институт\\\\ \n"
    "(национальный исследовательский университет)\\\\ \n"
    "\\includegraphics[scale=0.1]{mipt_image1.jpg}\\\\ \n"
    "\\vspace{5cm}\n"
    "\\Large Лабораторная работа № 1.3.1\\\\ \n"
    "\\vspace{2cm}\n"
    "\\textbf{\\Huge <<Дифференцируем в уме (почти)>>}\\\\ \n"
    "\\end{center}\n"

    "\\vspace{8cm}\n"
    "{\\raggedleft \\large Выполнили:\\\\ больные наголову студенты\\par\n"
    "}"
"\\end{titlepage}\n\n";

static char * readExpression (void);
static char * readString (char ** str);

//-------------------------------------------------------------building a tree-------------------------------------------------------

void removeSpaces (char * dest, const char * source)
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

node_t * getExpression (char ** str)
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
        char * buf = readString (str);

        int wordLength = strlen (buf);
        printf (">> getNumber: buf (%d) = %s\n", wordLength, buf);
        MY_ASSERT (wordLength == 0, "Failed line reading");
        (*str) = (*str)+wordLength;

        if (wordLength == 1)
        {
            // printf ("buf[0] = %c, buf[1] = %c\n", buf[0], buf[1]);
            return (createNodeWithVariable (buf[0]));
        }
        else
        {
            node_t * function = createNodeWithFunction (buf);
            node_t * followingExpression = getBracket (str);
            function->right = nullptr;
            function->left = followingExpression;
            followingExpression->parent = function;

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
        assert (*str != sOld);
        return (createNodeWithNum (val));
    }
}
//----------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------support functions---------------------------------------------------------

FILE * openFile (char * nameFile)
{
    printf (">> in openFile: nameFile = %s\n", nameFile);
    FILE * texfile = fopen (nameFile, "w");
    MY_ASSERT (texfile == nullptr, "There is no access to logfile");
    setbuf (texfile, nullptr);

    return texfile;
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
            MY_ASSERT (1, "There is no such operation");
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
            MY_ASSERT (1, "There is no such operation");
        }
    }
    if (((*node)->right != nullptr) && ((*node)->right->left != nullptr) &&
        ((*node)->right->right != nullptr) && ((*node)->right->left->type == NUM_T) &&
        ((*node)->right->right->type == NUM_T) && ((*node)->right->type == OPER_T))
    {
        node_t * newNode = createNodeWithNum (1);

        // COPYPASTE!!!!!!
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
            newNode->elem = pow ((*node)->right->left->elem, (*node)->right->right->elem);
            deleteNode ((*node)->right->left);
            deleteNode ((*node)->right->right);
            deleteNode ((*node)->right);
            (*node)->right = newNode;
        }
        else
        {
            MY_ASSERT (1, "There is no such operation\n");
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

    if (((*node)->op_t == OP_DIV || (*node)->op_t == OP_DEG) &&
        ((*node)->left != nullptr) && (compareFractionalNum((*node)->right->elem, 1) == 1))
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

    if (((*node)->op_t == OP_MUL) && (((*node)->right != nullptr) ) &&
        (compareFractionalNum((*node)->right->elem, 1) == 1))
    {
        node_t * saveLeftNode = (*node)->left;

        deleteNode ((*node)->right);
        deleteNode ((*node));

        (*node) = saveLeftNode;
    }
    if (((*node)->op_t == OP_MUL) && ((*node)->left != nullptr) &&
        (compareFractionalNum((*node)->left->elem, 1) == 1))
    {
        node_t * saveRightNode = (*node)->right;

        deleteNode ((*node)->left);
        deleteNode ((*node));

        (*node) = saveRightNode;
    }

    if (((*node)->left != nullptr) && ((*node)->left->op_t == OP_MUL))
    {
        if (((*node)->left->right != nullptr) &&
            compareFractionalNum((*node)->left->right->elem, 1) == 1)
        {
            node_t * saveNodeLeftLeft = (*node)->left->left;

            deleteNode ((*node)->left->right);
            deleteNode ((*node)->left);

            (*node)->left = saveNodeLeftLeft;
        }
        if (((*node)->left->left != nullptr) &&
            (compareFractionalNum((*node)->left->left->elem, 1) == 1))
        {
            node_t * saveNodeLeftRight = (*node)->left->right;

            deleteNode ((*node)->left->left);
            deleteNode ((*node)->left);

            (*node)->left = saveNodeLeftRight;
        }
    }
    if (((*node)->right != nullptr) && ((*node)->right->op_t == OP_MUL))
    {
        if (((*node)->right->right != nullptr) &&
            (compareFractionalNum((*node)->right->right->elem, 1) == 1))
        {
            node_t * saveNodeLeftLeft = (*node)->right->left;

            deleteNode ((*node)->right->right);
            deleteNode ((*node)->right);

            (*node)->right = saveNodeLeftLeft;
        }
        if (((*node)->right->left != nullptr) &&
            (compareFractionalNum((*node)->right->left->elem, 1) == 1))
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
        if (((*node)->right != nullptr) && ((*node)->right->type == NUM_T) &&
            (compareFractionalNum((*node)->right->elem, 0) == 1))
        {
            deleteNode ((*node)->right);
            (*node)->right = nullptr;
            node_t * newNode = copyNode ((*node)->left);
            MY_ASSERT (newNode == nullptr, "The left node cannot be copied");
            deleteTree ((*node)->left);
            (*node)->left = nullptr;
            deleteNode (*node);
            (*node) = newNode;
        }
        if (((*node)->left != nullptr) && ((*node)->left->type == NUM_T) &&
            (compareFractionalNum((*node)->left->elem, 0) == 1))
        {
            deleteNode ((*node)->left);
            (*node)->left = nullptr;
            node_t * newNode = copyNode ((*node)->right);
            MY_ASSERT (newNode == nullptr, "The left node cannot be copied");
            deleteTree ((*node)->right);
            (*node)->right = nullptr;
            deleteNode (*node);
            (*node) = newNode;
        }
    }
        if (((*node)->left != nullptr) && ((*node)->left->op_t == OP_ADD))
        {
            if (((*node)->left->right != nullptr) && ((*node)->left->right->type == NUM_T) &&
                (compareFractionalNum((*node)->left->right->elem, 0) == 1))
            {
                node_t * saveNodeLeftLeft = (*node)->left->left;

                deleteNode ((*node)->left->right);
                deleteNode ((*node)->left);

                (*node)->left = saveNodeLeftLeft;
            }
            if (((*node)->left->left != nullptr) && ((*node)->left->left->type == NUM_T) &&
                (compareFractionalNum((*node)->left->left->elem, 0) == 1))
            {
                node_t * saveNodeLeftRight = (*node)->left->right;

                deleteNode ((*node)->left->left);
                deleteNode ((*node)->left);

                (*node)->left = saveNodeLeftRight;
            }
        }
        else if (((*node)->right != nullptr) && ((*node)->right->op_t == OP_ADD))
        {
            if (((*node)->right->right != nullptr) && ((*node)->right->right->type == NUM_T) &&
                (compareFractionalNum((*node)->right->right->elem, 0) == 1))
            {
                node_t * saveNodeRightLeft = (*node)->right->left;

                deleteNode ((*node)->right->right);
                deleteNode ((*node)->right);

                (*node)->right = saveNodeRightLeft;
            }
            if (((*node)->right->left != nullptr) && ((*node)->right->left->type == NUM_T) &&
                (compareFractionalNum((*node)->right->left->elem, 0) == 1))
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
        (((node->right != nullptr) && (node->right->type == NUM_T) &&
            (compareFractionalNum(node->right->elem, 0) == 1)) ||
        ((node->left != nullptr) && (node->left->type == NUM_T) &&
            (compareFractionalNum(node->left->elem, 0) == 1))))
    {
        deleteNode (node->right);
        deleteNode (node->left);

        node->elem = 0;
        node->left = nullptr;
        node->right = nullptr;
        node->op_t = OP_ERROR;
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
node_t * getGrammarForDif (node_t * node)
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
        node_t * headNode = nullptr;

        if (node->op_t == OP_ADD)
        {
            headNode = createNodeWithOperation (OP_ADD, leftNode, rightNode);
        }
        else
        {
            headNode = createNodeWithOperation (OP_SUB, leftNode, rightNode);
        }

        return headNode;
    }
    return (difMulDiv (node));
}

node_t * difMulDiv (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to the node");
    while (node->op_t == OP_MUL)
    {
        node_t * difLeftNode = getExpressionForDif (node->left);
        node_t * difRightNode = getExpressionForDif (node->right);

        node_t * copyLeftNode  = copyNode (node->left);
        node_t * copyRightNode = copyNode (node->right);

        node_t * firstMul = createNodeWithOperation (OP_MUL, difLeftNode, copyRightNode);
        node_t * secondMul = createNodeWithOperation (OP_MUL, copyLeftNode, difRightNode);

        node_t * headNodeForMul = createNodeWithOperation (OP_ADD, firstMul, secondMul);

        return headNodeForMul;
    }
    while (node->op_t == OP_DIV)
    {
        node_t * difLeftNode = getExpressionForDif (node->left);
        node_t * difRightNode = getExpressionForDif (node->right);

        node_t * copyLeftNode  = copyNode (node->left);
        node_t * copyRightNode = copyNode (node->right);

        node_t * firstMul = createNodeWithOperation (OP_MUL, difLeftNode, copyRightNode);
        node_t * secondMul = createNodeWithOperation (OP_MUL, copyLeftNode, difRightNode);

        node_t * headNodeForNumerator = createNodeWithOperation (OP_SUB, firstMul, secondMul);

        node_t * degIndicator = createNodeWithNum (2);
        node_t * copyRightNodeForDenominator = copyNode (node->right);
        node_t * headNodeForDenominator = createNodeWithOperation (OP_DEG, copyRightNodeForDenominator, degIndicator);

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
            node_t * copyDegreeIndicator = copyNode (node->right);
            node_t * unit = createNodeWithNum (1);
            node_t * newDegreeIndicator = createNodeWithOperation (OP_SUB, copyDegreeIndicator, unit);

            node_t * copyLeftNode = copyNode (node->left);
            node_t * firstMul = createNodeWithOperation (OP_DEG, copyLeftNode, newDegreeIndicator);

            node_t * secondMul = copyNode (node->right);
            node_t * leftDescendant = createNodeWithOperation (OP_MUL, secondMul, firstMul);

            node_t * difOfDegreeBase = getExpressionForDif (node->left);
            node_t * headNodeForDeg = createNodeWithOperation (OP_MUL, leftDescendant, difOfDegreeBase);

            return headNodeForDeg;
        }
        else
        {
            node_t * copyExpression = copyNode (node);
            node_t * func = createNodeWithFunction ((char*) "ln");
            node_t * argOfFunc = createNodeWithNum (2);
            func->left = argOfFunc;
            argOfFunc->parent = func;
            node_t * firstMul = createNodeWithOperation (OP_MUL, copyExpression, func);

            node_t * difRightNode = getExpressionForDif (node->right);

            node_t * headNodeForExpFunc = createNodeWithOperation (OP_MUL, firstMul, difRightNode);

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
        MY_ASSERT (1, "Wrong type");
        return nullptr;
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
        // node_t * copyForRightDescendant = copyNode (node->left);
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
        // node_t * copyForRightDescendant = copyNode (node->left);
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

//----------------------------------------------------------dump to a texfile-------------------------------------------------------

void texStart (FILE * texfile)
{
    dumpTexTree (TEX_OPTIONS);
    dumpTexTree ("\\begin{document}\n\n");
    dumpTexTree (TEX_TITLE_PAGE);
}

void startEquation (FILE * texfile, char var)
{
    dumpTexTree ("\nДавайте возьмём производную от этого чуда!\\\\\n");
    dumpTexTree ("\\begin{equation}\n");
    printf (">>>>in startEquation: var = %c\n", var);
    dumpTexTree ("f(%c) = ", var);
}

void startDifEquation (FILE * texfile, char var)
{
    dumpTexTree ("\nЭто не должно быть страшно! Посмотрим...\\\\\n");
    dumpTexTree ("\\begin{equation}\n");
    dumpTexTree ("f(%c)^\\prime_%c = ", var, var);
}


void endEquation (FILE * texfile)
{
    dumpTexTree ("\n\\end{equation}\n");
    dumpTexTree ("Как говорится в старой пословице: \"Меньше знаешь -- крепче спишь\"."
                    "Не задумывайтесь о том, что здесь произошло\\\\\n");
}

void texPrintNode (FILE * texfile, node_t * node)
{
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");


    if (node->left != nullptr && node->right != nullptr)
    {
        if (node->op_t == OP_DIV)
        {
            dumpTexTree ("\\frac{");
        }
        if (node->op_t == OP_DEG && node->left->type == FUNC_T)
        {
            dumpTexTree ("(");
        }
        texPrintNode (texfile, node->left);
        if (node->op_t == OP_DEG && node->left->type == FUNC_T)
        {
            dumpTexTree (")");
        }
        if (node->op_t == OP_DIV)
        {
            dumpTexTree ("}");
        }
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
            dumpTexTree ("(");
            texPrintNode (texfile, node->left);
            dumpTexTree (")");
            break;
        case CONST_T:
            texPrintConst (texfile, node);
            break;
        default:
            fprintf (stderr, "Non-existent node type\n");
            break;
    }

    if (node->right != nullptr)
    {
        if (node->op_t == OP_DIV)
        {
            dumpTexTree ("{");
        }
        if ((node->op_t == OP_MUL || node->op_t == OP_DIV) &&
            (node->right->op_t == OP_ADD || node->right->op_t == OP_SUB))
        {
            dumpTexTree ("(");
        }
        texPrintNode (texfile, node->right);
        if (node->op_t == OP_DEG)
        {
            dumpTexTree ("}");
        }
        if ((node->op_t == OP_MUL || node->op_t == OP_DIV) &&
            (node->right->op_t == OP_ADD || node->right->op_t == OP_SUB))
        {
            dumpTexTree (")");
        }
        if (node->op_t == OP_DIV)
        {
            dumpTexTree ("}");
        }
    }
}

void texPrintOperation (FILE * texfile, node_t * node)
{
    MY_ASSERT (texfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    switch (node->op_t)
    {
        case OP_ADD:
            dumpTexTree ("+");
            break;
        case OP_SUB:
            dumpTexTree (" %c ", OP_ADD);
            break;
        case OP_MUL:
            dumpTexTree (" \\cdot ");
            break;
        case OP_DIV:
            break;
        case OP_DEG:
            dumpTexTree (" %c{ ", OP_DEG);
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
    dumpTexTree("\n\\end{document}\n");

    fclose(texfile);

    return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------decompose by the Taylor formula-----------------------------------------------------

void decomposeByTaylor (node_t * node, FILE * texfile, char varInEquation)
{
    MY_ASSERT (node == nullptr, "There is no access to this node");

    node_t * copyHeadNode = copyNode (node);
    graphicDumpTree (copyHeadNode);

    int degree = 0;

    degree = checkInput (&degree);

    node_t ** arrDerivatives = (node_t **) calloc (degree+1, sizeof(node_t *));
    fillArrOfNodes (copyHeadNode, arrDerivatives, degree);

    resetVar (copyHeadNode);

    simplifyExpression (&copyHeadNode);
    arrDerivatives[degree] = copyHeadNode;

    dumpTexTree ("Встречайте, разложение по формуле Тейлора в точке 0!\\\\\n");
    startEquation (texfile, varInEquation);

    for (int i = degree, j = 0; i >= 0; i--, j++)
    {
        dumpTexTree ("\\frac{");
        texPrintNode (texfile, arrDerivatives[i]);
        if (i != 0)
        {
            dumpTexTree ("}{%d!}\\cdot %c^{%d} + ", j, varInEquation, j);
        }
        else
        {
            dumpTexTree ("}{%d!}\\cdot %c^{%d}", j, varInEquation, degree);
        }
    }

    endEquation (texfile);

    for (int i = 0; i < degree; i++)
    {
        deleteTree (arrDerivatives[i]);
    }

    deleteTree (copyHeadNode);
    free (arrDerivatives);
}

int checkInput (int * degreeOfNum)
{
    MY_ASSERT (degreeOfNum == nullptr, "There is no access to this number");

    int enterSymbols = scanf (" %d", degreeOfNum);
    while (getchar() != '\n');

    if (enterSymbols == 0 || *degreeOfNum < 0)
    {
        printf ("Please, enter a positive number\n");
        return checkInput (degreeOfNum);
    }

    return *degreeOfNum;
}

void fillArrOfNodes (node_t * node, node_t ** arrNodes, int n)
{
    if (n > 0)
    {
        node = getGrammarForDif (node);
        simplifyExpression (&node);

        fillArrOfNodes (node, arrNodes, n-1);

        resetVar (node);
        simplifyExpression (&node);

        arrNodes[n-1] = node;
    }
}

void resetVar (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to this node");

    if (node->type == VAR_T)
    {
        node->type = NUM_T;
        node->varName = 0;
        node->elem = 0;
    }

    if (node->left != nullptr)
    {
        resetVar (node->left);
    }
    if (node->right != nullptr)
    {
        resetVar (node->right);
    }
}

char varName (const node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to this node");
    var_t varStruct = findNameVar (node);

    return varStruct.vName;
}

static var_t findNameVar (const node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to this node");
    var_t variable = {};
    if (node->type == VAR_T)
    {
        variable.vName = node->varName;
        variable.isVar = 1;
        return variable;
    }

    if (node->left != nullptr)
    {
        variable = findNameVar (node->left);
        if (variable.isVar == 1)
        {
            return variable;
        }
    }
    if (node->right != nullptr)
    {
        variable = findNameVar (node->right);
        if (variable.isVar == 1)
        {
            return variable;
        }
    }
    return variable;
}

//---------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------build a graph using python------------------------------------------------------

void buildGraph (node_t * node, FILE * texfile, FILE * pyfile)
{
    int leftX = 0;
    int rightX = 0;

    leftX = checkInput (&leftX);
    rightX = checkInput (&rightX);

    //TODO: WRITE SCRIPT WITH ARGUMENTS)
    dumpPython ("import numpy as np\n");
    dumpPython ("import matplotlib.pyplot as plt\n");
    dumpPython ("x = np.linspace(%d, %d, 200)\n", leftX, rightX);
    dumpPython ("y = ");
    pyPrintNode (pyfile, node);
    dumpPython ("\n");

    dumpPython ("plt.figure(figsize=(8,6), dpi=100)\n");
    dumpPython ("plt.grid(True, linestyle=\"--\")\n");
    dumpPython ("plt.axis([%d, %d, np.min(y), np.max(y)])\n", leftX, rightX);
    dumpPython ("plt.plot(x, y, \"-m\",linewidth=0.5)\n");

    dumpPython ("plt.savefig('plot.png')");
    fclose (pyfile);
    system ("python3 plot.py");

    dumpTexTree ("\\begin{figure}[h]\n");
    dumpTexTree ("\\center{\\includegraphics[width=0.7\\linewidth]{plot.png}}\n");
    dumpTexTree ("\\caption{Ваша кривая}\n");
    dumpTexTree ("\\label{ris:image}\n");
    dumpTexTree ("\\end{figure}\n");
}

void pyPrintNode (FILE * pyfile, node_t * node)
{
    MY_ASSERT (pyfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");


    if (node->left != nullptr && node->right != nullptr)
    {
        if (node->op_t == OP_DIV)
        {
            dumpPython ("(");
        }
        if (node->op_t == OP_DEG && node->left->type == FUNC_T)
        {
            dumpPython ("(");
        }
        pyPrintNode (pyfile, node->left);
        if (node->op_t == OP_DEG && node->left->type == FUNC_T)
        {
            dumpPython (")");
        }
        if (node->op_t == OP_DIV)
        {
            dumpPython (")");
        }
    }

    switch (node->type)
    {
        case OPER_T:
            pyPrintOperation (pyfile, node);
            break;
        case VAR_T:
            pyPrintVar (pyfile, node);
            break;
        case NUM_T:
            pyPrintNum (pyfile, node);
            break;
        case FUNC_T:
            pyPrintFunc (pyfile, node);
            dumpPython ("(");
            pyPrintNode (pyfile, node->left);
            dumpPython (")");
            break;
        case CONST_T:
            pyPrintConst (pyfile, node);
            break;
        default:
            fprintf (stderr, "Non-existent node type\n");
            break;
    }

    if (node->right != nullptr)
    {
        if (node->op_t == OP_DIV)
        {
            dumpPython ("(");
        }
        if ((node->op_t == OP_MUL || node->op_t == OP_DIV) && (node->right->op_t == OP_ADD ||
            node->right->op_t == OP_SUB))
        {
            dumpPython ("(");
        }
        pyPrintNode (pyfile, node->right);
        if (node->op_t == OP_DEG)
        {
            dumpPython (")");
        }
        if ((node->op_t == OP_MUL || node->op_t == OP_DIV) && (node->right->op_t == OP_ADD ||
            node->right->op_t == OP_SUB))
        {
            dumpPython (")");
        }
        if (node->op_t == OP_DIV)
        {
            dumpPython (")");
        }
    }
}

void pyPrintOperation (FILE * pyfile, node_t * node)
{
    MY_ASSERT (pyfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");
    printf (">>in pyPrintOperation\n");
    switch (node->op_t)
    {
        case OP_ADD:
            dumpPython (" %c ", OP_ADD);
            break;
        case OP_SUB:
            dumpPython (" %c ", OP_SUB);
            break;
        case OP_MUL:
            dumpPython (" * ");
            break;
        case OP_DIV:
            dumpPython (" / ");
            break;
        case OP_DEG:
            dumpPython (" **( ");
            break;
        default:
            printf ("There is no such node\n");
            break;
    }
}

void pyPrintVar (FILE * pyfile, node_t * node)
{
    MY_ASSERT (pyfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpPython ("%c", node->varName);
}

void pyPrintNum (FILE * pyfile, node_t * node)
{
    MY_ASSERT (pyfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpPython ("%.2lf", node->elem);
}

void pyPrintFunc (FILE * pyfile, node_t * node)
{
    MY_ASSERT (pyfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpPython ("np.%s", node->nameFunc);
}

void pyPrintConst (FILE * pyfile, node_t * node)
{
    MY_ASSERT (pyfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpPython ("%c", node->varName);
}


//--------------------------------------------------------------------------------------------------------------------------------

void deleteTree (node_t * node)
{
    MY_ASSERT (node == nullptr, "There is no access to this tree");

    if (node->left != nullptr)
    {
        deleteTree (node->left);
    }
    if (node->right != nullptr)
    {
        deleteTree (node->right);
    }

    free(node);
}

