#include "taylor.h"

static node_t ** taylor (node_t * node, int degree);
static void fillArrOfNodes (node_t * node, node_t ** arrNodes, int n);
static void resetVar (node_t * node);

void taylorInterface (node_t * node, FILE * texfile, char varInEquation)
{
    MY_ASSERT (node == nullptr, "There is no access to this node");
    node_t * copyHeadNode = copyNode (node);

    int degree = 0;

    printf ("To what extent decompose?\n");
    degree = checkInput (&degree);

    node_t ** arrDerivatives = taylor (copyHeadNode, degree);

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

    for (int i = 0; i < degree; i++)
    {
        deleteTree (arrDerivatives[i]);
    }

    deleteTree (copyHeadNode);
    free (arrDerivatives);
}

static node_t ** taylor (node_t * node, int degree)
{
    MY_ASSERT (node == nullptr, "There is no access to this node");

    node_t ** arrDerivatives = (node_t **) calloc (degree+1, sizeof(node_t *));

    fillArrOfNodes (node, arrDerivatives, degree);

    resetVar (node);

    simplify (&node);
    arrDerivatives[degree] = node;

    return arrDerivatives;
}

static void fillArrOfNodes (node_t * node, node_t ** arrNodes, int n)
{
    if (n > 0)
    {
        node = diff (node);
        simplify (&node);

        fillArrOfNodes (node, arrNodes, n-1);

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


