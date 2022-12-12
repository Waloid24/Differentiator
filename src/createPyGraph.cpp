#include "createPyGraph.h"

#define dumpPython(text, ...)\
    fprintf (pyfile, text, ##__VA_ARGS__)

static void pyPrintNode (FILE * pyfile, node_t * node);
static void pyPrintOperation (FILE * pyfile, node_t * node);
static void pyPrintVar (FILE * pyfile, node_t * node);
static void pyPrintNum (FILE * pyfile, node_t * node);
static void pyPrintFunc (FILE * pyfile, node_t * node);
static void pyPrintConst (FILE * pyfile, node_t * node);

//--------------------------------------------------------------------------------------------

void buildGraph (node_t * node, FILE * texfile, FILE * pyfile)
{
    int leftX = 0;
    int rightX = 0;

    printf ("Enter the left border\n");
    leftX = checkInput (&leftX);
    printf ("Enter the right border\n");
    rightX = checkInput (&rightX);

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

static void pyPrintNode (FILE * pyfile, node_t * node)
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

static void pyPrintOperation (FILE * pyfile, node_t * node)
{
    MY_ASSERT (pyfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");
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

static void pyPrintVar (FILE * pyfile, node_t * node)
{
    MY_ASSERT (pyfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpPython ("%c", node->varName);
}

static void pyPrintNum (FILE * pyfile, node_t * node)
{
    MY_ASSERT (pyfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpPython ("%.2lf", node->elem);
}

static void pyPrintFunc (FILE * pyfile, node_t * node)
{
    MY_ASSERT (pyfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    if (myStrcmp (node->nameFunc, "ln") == 0 ||
        myStrcmp (node->nameFunc, "log") == 0)
    {
        dumpPython ("np.log");
    }
    else if (myStrcmp (node->nameFunc, "tg") == 0)
    {
        dumpPython ("np.tan");
    }
    else
    {
        dumpPython ("np.%s", node->nameFunc);
    }
}

static void pyPrintConst (FILE * pyfile, node_t * node)
{
    MY_ASSERT (pyfile == nullptr, "There is no access to this file");
    MY_ASSERT (node == nullptr, "There is no access to this node");

    dumpPython ("%c", node->varName);
}
