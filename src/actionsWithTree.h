#ifndef ACTIONSWITHTREE_H
#define ACTIONSWITHTREE_H

#include "support.h"
#include <stdlib.h>
// #include "tree.h"
// do not separate tree declaration and it's interface

node_t * createNodeWithNum (elem_t num);
node_t * createNodeWithOperation (enum operationType operation, node_t * valLeftNode, node_t * valRightNode);
node_t * createNodeWithVariable (char nameOfVar);
node_t * createNodeWithFunction (const char * nameFunction, node_t * leftDescendant);

// stack_push(double elem);
void insert_node (node_t * node, node_t * root);
node_t * copyNode (node_t * nodeForCopy);
void deleteNode (node_t * node);
void deleteTree (node_t * node);

#endif
