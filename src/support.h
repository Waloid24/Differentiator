#ifndef SUPPORT_H
#define SUPPORT_H

#include <stdio.h>
#include <ctype.h>
#include "MY_ASSERT.h"
#include "tree.h"

struct forVarName {
    char vName;
    bool isVar;
};
typedef struct forVarName var_t;

int checkInput (int * degreeOfNum);
void graphicDumpTree (const node_t * node);
int myStrcmp (const char * string1, const char * string2);
FILE * openFile (char * nameFile);
char varName (const node_t * node);

#endif
