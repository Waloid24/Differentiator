#ifndef SUPPORT_H
#define SUPPORT_H

#include <stdio.h>
#include <ctype.h>
#include "tree.h"
#include "my_assert.h"

int checkInput (int * degreeOfNum);
void graphicDumpTree (const node_t * node);
int myStrcmp (const char * string1, const char * string2);
FILE * openFile (char * nameFile);

#endif
