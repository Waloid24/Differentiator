#ifndef DECOMPOSEBYTAYLOR_H
#define DECOMPOSEBYTAYLOR_H

#include <stdlib.h>
#include "MY_ASSERT.h"
#include "tree.h"
#include "simplify.h"
#include "dumpToTex.h"
#include "diff.h"

void startEquation (FILE * texfile, char var);
void startDifEquation (FILE * texfile, char var);
void endEquation (FILE * texfile);

node_t ** taylor (node_t * node, int degree);
int decAccuracy (void);
void dumpTaylor (FILE * texfile, char varInEquation, node_t ** arrDerivatives, int degree);
void clearTaylorMemory (node_t ** arrDerivatives, int degree, node_t * headNode);


#endif
