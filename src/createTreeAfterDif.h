#ifndef CREATETREEAFTERDIF_H
#define CREATETREEAFTERDIF_H

#include "support.h"
#include "actionsWithTree.h"
#include "tree.h"

node_t * getGrammarForDif (node_t * node);
node_t * getExpressionForDif (node_t * node);
node_t * difMulDiv (node_t * node);
node_t * difDegree (node_t * node);
node_t * difNumberOrVar (node_t * node);
node_t * difFunc (node_t * node);

#endif
