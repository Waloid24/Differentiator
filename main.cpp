#include "tree.h"
#include "differentiator.h"

int main (void)
{
    FILE * logFile = fopen ("log.txt", "a");

    node_t * firstNode = getGrammar ();

    graphicDumpTree (firstNode);

    simplifyExpression (&firstNode);
    graphicDumpTree (firstNode);

    node_t * difNode = getGrammarForDif (firstNode);
    graphicDumpTree (difNode);

    simplifyExpression (&firstNode);
    graphicDumpTree (firstNode);

}
