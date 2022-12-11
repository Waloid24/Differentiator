#ifndef PROCESSEXPR_H
#define PROCESSEXPR_H

#include <stdlib.h>
#include <string.h>
#include "support.h"
#include "tree.h"

void removeSpaces (char * dest, const char * source);
node_t * getGrammar (void);
node_t * getExpression (char ** str);
node_t * getT (char ** str);
node_t * getDegree (char ** str);
node_t * getBracket (char ** str);
node_t * getNumber (char ** str);


#endif
