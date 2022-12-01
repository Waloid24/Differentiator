#include "tree.h"
#include "differentiator.h"

int main (void)
{
    FILE * logFile = fopen ("log.txt", "a");

    printf ("Enter the expression\n");
    char * string = (char *) calloc (50, sizeof(char));
    char * strFixed = (char *) calloc (50, sizeof(char));

    fgets (string, 50, stdin);
    printf ("string = %s", string);

    removeSpaces (strFixed, string);
    printf ("strFixed = %s\n", strFixed);

    node_t * firstNode = getGrammar (strFixed);

    graphicDumpTree (firstNode);
    printf ("firstNode before simplify = %p\n", firstNode);

    simplifyExpToOne (firstNode);
    printf ("firstNode after simplify = %p\n", firstNode);
    graphicDumpTree (firstNode);

    node_t * difNode = getGrammarForDif (firstNode);
    printf ("difNode before simplify = %p\n", difNode);

    graphicDumpTree (difNode);

//     simplifyExpToOne (difNode);
//     printf ("difNode after simplify = %p\n", difNode);
//
//     graphicDumpTree (difNode);
}
