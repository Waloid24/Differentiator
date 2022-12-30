#ifndef DUMPTOTEX_H
#define DUMPTOTEX_H

#define dumpTexTree(text, ...)\
    fprintf (texfile, text, ##__VA_ARGS__)

#include <stdio.h>
#include "MY_ASSERT.h"
#include "tree.h"

void texStart (FILE * texfile);
void texPrintNode (FILE * texfile, node_t * node);
int texFinish(FILE * texfile);

#endif
