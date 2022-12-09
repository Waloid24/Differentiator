#ifndef SIMPLIFY_H
#define SIMPLIFY_H

#include <math.h>
#include "support.h"
#include "actionsWithTree.h"
#include "tree.h"

void simplifyDegOrDivToOne (node_t ** node);
void simplifyMulByZero (node_t * node);
void simplifyMulByOne (node_t ** node);
void simplifyAddWithZero (node_t ** node);
int compareFractionalNum (double firstNum, double secondNum);
void removeConstants (node_t ** node);
void simplifyExpression (node_t ** node);

#endif
