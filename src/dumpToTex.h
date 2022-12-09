#ifndef DUMPTOTEX_H
#define DUMPTOTEX_H

#define dumpTexTree(text, ...)\
    fprintf (texfile, text, ##__VA_ARGS__)

#include "support.h"
#include "tree.h"

void texStart (FILE * texfile);
void texPrintNode (FILE * texfile, node_t * node);
void texPrintOperation (FILE * texfile, node_t * node);
void texPrintVar (FILE * texfile, node_t * node);
void texPrintNum (FILE * texfile, node_t * node);
void texPrintFunc (FILE * texfile, node_t * node);
void texPrintConst (FILE * texfile, node_t * node);
int texFinish(FILE * texfile);

#endif
