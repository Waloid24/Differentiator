#include "taylor.h"

static void difForTaylor (node_t * node, node_t ** arrNodes, int n);
static void resetVar (node_t * node);
static int checkInput (int * degreeOfNum);

node_t ** taylor (node_t * node, int degree)
{
    MY_ASSERT (node == nullptr, "There is no access to this node");

    node_t ** arrDerivatives = (node_t **) calloc (degree+1, sizeof(node_t *));

    difForTaylor (node, arrDerivatives, degree);

    resetVar (node);

    simplify (&node);
    arrDerivatives[degree] = node;

    return arrDerivatives;
}

static void difForTaylor (node_t * node, node_t ** arrNodes, int n)
{
    if (n > 0)
    {
        node = diff (node);
        simplify (&node);

        difForTaylor (node, arrNodes, n-1);

        resetVar (node);
        simplify (&node);
        arrNodes[n-1] = node;
    }
}

static void resetVar (node_t * node)
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

void dumpTaylor (FILE * texfile, char varInEquation, node_t ** arrDerivatives, int degree)
{
    dumpTexTree ("Decomposition by the Maclaurin formula!\\\\\n");
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
}

void clearTaylorMemory (node_t ** arrDerivatives, int degree, node_t * headNode)
{
    for (int i = 0; i < degree; i++)
    {
        deleteTree (arrDerivatives[i]);
    }

    deleteTree (headNode);
    free (arrDerivatives);
}

int decAccuracy (void)
{
    int degree = 0;
    printf ("To what extent decompose?\n");
    degree = checkInput (&degree);
    
    return degree;
}

static int checkInput (int * degreeOfNum)
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