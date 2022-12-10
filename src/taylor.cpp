#include "taylor.h"

static var_t findNameVar (const node_t * node);

void decomposeByTaylor (node_t * node, FILE * texfile, char varInEquation)
{
    MY_ASSERT (node == nullptr, "There is no access to this node");
    node_t * copyHeadNode = copyNode (node);
    graphicDumpTree (copyHeadNode);

    int degree = 0;

    printf ("To what extent decompose?\n");
    degree = checkInput (&degree);

    node_t ** arrDerivatives = (node_t **) calloc (degree+1, sizeof(node_t *));

    fillArrOfNodes (copyHeadNode, arrDerivatives, degree);

    resetVar (copyHeadNode);

    simplify (&copyHeadNode);
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

void fillArrOfNodes (node_t * node, node_t ** arrNodes, int n)
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
