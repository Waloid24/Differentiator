#ifndef CREATEPYGRAPH_H
#define CREATEPYGRAPH_H

#include "dumpToTex.h"
#include "support.h"

void buildGraph (node_t * node, FILE * texfile, FILE * pyfile);
void pyPrintNode (FILE * pyfile, node_t * node);
void pyPrintOperation (FILE * pyfile, node_t * node);
void pyPrintVar (FILE * pyfile, node_t * node);
void pyPrintNum (FILE * pyfile, node_t * node);
void pyPrintFunc (FILE * pyfile, node_t * node);
void pyPrintConst (FILE * pyfile, node_t * node);

#endif
