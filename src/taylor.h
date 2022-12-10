#ifndef DECOMPOSEBYTAYLOR_H
#define DECOMPOSEBYTAYLOR_H

#include <stdlib.h>
#include "tree.h"
#include "support.h"
#include "simplify.h"
#include "dumpToTex.h"
#include "diff.h"

struct forVarName {
    char vName;
    bool isVar;
};

typedef struct forVarName var_t;

void decomposeByTaylor (node_t * node, FILE * texfile, char varInEquation);
void fillArrOfNodes (node_t * node, node_t ** arrNodes, int n);
void resetVar (node_t * node);
void startEquation (FILE * texfile, char var);
void startDifEquation (FILE * texfile, char var);
void endEquation (FILE * texfile);
char varName (const node_t * node);

#endif
