#include "tree.h"
#include "processExpr.h"
#include "createPyGraph.h"
#include "graphDumpTree.h"
#include "dumpToTex.h"
#include "diff.h"
#include "taylor.h"
#include "simplify.h"

struct forVarName {
    char vName;
    bool isVar;
};
typedef struct forVarName var_t;

FILE * openFile (char * nameFile);
char varName (const node_t * node);
static var_t findNameVar (const node_t * node);

int main (int argc, char * argv[])
{
    MY_ASSERT (argc != 3, "Too few arguments on the command line");
    FILE * texfile = openFile (argv[1]);
    FILE * pyfile = openFile (argv[2]);

    node_t * firstNode = getGrammar ();
    simplify (&firstNode);

    graphicDumpTree (firstNode);

    char var = varName (firstNode);

    node_t * difNode = diff (firstNode);

    simplify (&difNode);

    texStart (texfile);
    startEquation (texfile, var);
    texPrintNode (texfile, firstNode);
    endEquation (texfile);

    startDifEquation (texfile, var);
    texPrintNode (texfile, difNode);
    endEquation (texfile);

    node_t * copyFirstNode = copyNode (firstNode);
    int degree = decAccuracy ();
    node_t ** arrDerivatives = taylor (copyFirstNode, degree);
    dumpTaylor (texfile, var, arrDerivatives, degree);
    clearTaylorMemory (arrDerivatives, degree, copyFirstNode);

    buildGraph (firstNode, texfile, pyfile);

    texFinish (texfile);

    deleteTree (firstNode);
    deleteTree (difNode);
}

FILE * openFile (char * nameFile)
{
    FILE * texfile = fopen (nameFile, "w");
    MY_ASSERT (texfile == nullptr, "There is no access to logfile");
    setbuf (texfile, nullptr);

    return texfile;
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