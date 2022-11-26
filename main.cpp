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

    printf ("firstNode = %c, %p\n", firstNode->op_t, firstNode);
    if (firstNode->left == firstNode->right)
    {
        printf ("firstNode->left->elem = %lf\n", firstNode->left->elem);
    }
    else
    {
        printf ("firstNode->left->elem = %lf, %p\n", firstNode->left->elem, &firstNode->left);
        printf ("firstNode->right->elem = %lf, %p\n", firstNode->right->elem, &firstNode->right);
    }

    // textDump (firstNode, logFile, NO_LAST, 0);
    graphicDumpTree (firstNode);
}
