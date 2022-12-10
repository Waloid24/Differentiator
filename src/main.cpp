#include "tree.h"
#include "processExpr.h"
#include "createPyGraph.h"
#include "dumpToTex.h"
#include "diff.h"
#include "taylor.h"
#include "simplify.h"
#include "actionsWithTree.h"

int main (int argc, char * argv[])
{
    printf ("argc = %d\n", argc);
    MY_ASSERT (argc != 3, "Too few arguments on the command line");
    FILE * texfile = openFile (argv[1]);
    FILE * pyfile = openFile (argv[2]);

    node_t * firstNode = getGrammar ();
    simplify (&firstNode);

    printf (">> in main\n");
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

    decomposeByTaylor (firstNode, texfile, var);

    printf ("in main after Teaylor\n");

    buildGraph (firstNode, texfile, pyfile);

    texFinish (texfile);

    deleteTree (firstNode);
    deleteTree (difNode);
}
