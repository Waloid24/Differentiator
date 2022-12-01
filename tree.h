#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "my_assert.h"

#define dumplineTree(text, ...)\
		fprintf (graphicDump, text, ##__VA_ARGS__)

enum nodeType {
    OPER_T          = 1,
    VAR_T           = 2,
    NUM_T           = 3,
    FUNC_T          = 4,
    CONST_T         = 5
};

enum operationType {
    WITHOUT_OP      = 0,
    OP_ADD          = '+',
    OP_SUB          = '-',
    OP_MUL          = '*',
    OP_DIV          = '/',
    OP_DEG          = '^'
};

enum createNode {
    RIGHT = 1,
    LEFT = 2,
    FIRST = 3
};

typedef double elem_t;

struct nodeTree {
    struct nodeTree * parent;
    elem_t elem;
    char varName;
    char * nameFunc;
    struct nodeTree * left;
    struct nodeTree * right;

    enum nodeType type;
    enum operationType op_t;
};

typedef struct nodeTree node_t;

//-----------------------------------------------create nodes----------------------------------------------------
node_t * createNodeWithNum (elem_t num);
node_t * createNodeWithOperation (enum operationType operation, node_t * valLeftNode, node_t * valRightNode);
node_t * createNodeWithVariable (char variableName);
node_t * createNodeWithConst (char variableName);
node_t * createNodeWithFunction (char * nameFunction);
node_t * copyNode (node_t * nodeForCopy);
//---------------------------------------------------------------------------------------------------------------

//--------------------------------------------graphical tree dump-------------------------------------------------
void graphicDumpTree (const node_t * node);
void dotFileHeaderTree (const node_t * node, const char * nameDotFileTree);
void writeNodeToDotFile (const node_t * node, FILE * graphicDump);
void writeEdgeToDotFile (const node_t * node, FILE * graphicDump);
void createDotFileTree (const char * nameDotFile, unsigned int timesCreatePicture);
static void createHtmlFileTree(const char * nameFileDump, unsigned int * timesCreatePicture);
//----------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------support functions---------------------------------------------------------
int myStrcmp (const char * string1, const char * string2);
//---------------------------------------------------------------------------------------------------------------------------------


#endif
