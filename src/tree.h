#ifndef TREE_H
#define TREE_H

#include <stdlib.h>

enum op_t {
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
    EXP = 5,
    ROOT = 6
};

enum nodeType {
    OPER_T          = 1,
    VAR_T           = 2,
    NUM_T           = 3,
    FUNC_T          = 4,
    CONST_T         = 5
};

enum operationType {
    OP_ERROR      = 0,
    OP_ADD          = '+',
    OP_SUB          = '-',
    OP_MUL          = '*',
    OP_DIV          = '/',
    OP_DEG          = '^'
};

typedef double elem_t;

struct nodeTree {
    struct nodeTree * parent;
    elem_t elem;
    char varName;
    const char * nameFunc;
    struct nodeTree * left;
    struct nodeTree * right;

    enum nodeType type;
    enum operationType op_t;
};

typedef struct nodeTree node_t;

node_t * createNodeWithNum (elem_t num);
node_t * createNodeWithOperation (enum operationType operation, node_t * valLeftNode, node_t * valRightNode);
node_t * createNodeWithVariable (char nameOfVar);
node_t * createNodeWithFunction (const char * nameFunction, node_t * leftDescendant);

void insert_node (node_t * node, node_t * root);
node_t * copyNode (node_t * nodeForCopy);
void deleteNode (node_t * node);
void deleteTree (node_t * node);

#endif
