#ifndef DECOMPOSEBYTAYLOR_H
#define DECOMPOSEBYTAYLOR_H

#include <stdlib.h>
#include "tree.h"
#include "support.h"
#include "simplify.h"
#include "dumpToTex.h"
#include "diff.h"

void taylorInterface (node_t * node, FILE * texfile, char varInEquation);
void startEquation (FILE * texfile, char var);
void startDifEquation (FILE * texfile, char var);
void endEquation (FILE * texfile);


#endif
