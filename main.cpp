#include "tree.h"
#include "differentiator.h"

int main (void)
{
    FILE * texfile = openTexfile ();

    node_t * firstNode = getGrammar ();

    simplifyExpression (&firstNode);
    graphicDumpTree (firstNode);

    char var = saveVar (firstNode);

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

    buildGraph (firstNode, texfile);

    texFinish (texfile);
}
