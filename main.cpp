#include "tree.h"
#include "differentiator.h"

int main (int argc, char * argv[])
{
    printf ("argc = %d\n", argc);
    MY_ASSERT (argc != 3, "Too few arguments on the command line");
    FILE * texfile = openFile (argv[1]);
    FILE * pyfile = openFile (argv[2]);

    node_t * firstNode = getGrammar ();
    simplifyExpression (&firstNode);

    char var = varName (firstNode);

    node_t * difNode = getGrammarForDif (firstNode);

    simplifyExpression (&difNode);

    texStart (texfile);
    startEquation (texfile, var);
    texPrintNode (texfile, firstNode);
    endEquation (texfile);

    startDifEquation (texfile, var);
    texPrintNode (texfile, difNode);
    endEquation (texfile);

    decomposeByTaylor (firstNode, texfile, var);

    buildGraph (firstNode, texfile, pyfile);

    texFinish (texfile);

    deleteTree (firstNode);
    deleteTree (difNode);
}
