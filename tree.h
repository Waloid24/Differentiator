#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "my_assert.h"

enum nodeType {
    OPER_T          = 1,
    VAR_T           = 2,
    NUM_T           = 3,
    FUNC_T          = 4,
    CONST_T         = 5
};

enum operationType {
    OP_ERROR      = 0, // OP_...
    OP_ADD          = '+',
    OP_SUB          = '-',
    OP_MUL          = '*',
    OP_DIV          = '/',
    OP_DEG          = '^'
};

typedef double elem_t;

// typedef struct
// {
//     myStruct* structPtr;
//
// } myStruct;

struct nodeTree {
    struct nodeTree * parent;
    elem_t elem; // TODO: elem but its only one of element types
    char varName;
    char * nameFunc;
    struct nodeTree * left;
    struct nodeTree * right;

    enum nodeType type;
    enum operationType op_t;

    // TODO: nodeType type;
    // union
    // {
    //     op
    //     var
    //     ...
    // } elem;
};

// TODO: unite with struct declaration
typedef struct nodeTree node_t;

//-----------------------------------------------create nodes----------------------------------------------------
node_t * createNodeWithNum (elem_t num);
node_t * createNodeWithOperation (enum operationType operation, node_t * valLeftNode, node_t * valRightNode);
node_t * createNodeWithVariable (char nameOfVar);
node_t * createNodeWithFunction (char * nameFunction);
node_t * copyNode (node_t * nodeForCopy);
//---------------------------------------------------------------------------------------------------------------

//--------------------------------------------graphical tree dump-------------------------------------------------
void graphicDumpTree (const node_t * node);
//----------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------support functions---------------------------------------------------------
int myStrcmp (const char * string1, const char * string2);
//---------------------------------------------------------------------------------------------------------------------------------


#endif
