#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include "my_assert.h"

#define dumplineTree(text, ...)\
		fprintf (graphicDump, text, ##__VA_ARGS__)

enum nodeType {
    OPER_T,
    VAR_T,
    NUM_T
};

enum operationType {
    OP_ADD = '+',
    OP_SUB = '-',
    OP_MUL = '*',
    OP_DIV = '/',
    OP_DEG = '^'
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
    struct nodeTree * left;
    struct nodeTree * right;

    enum nodeType type;
    enum operationType op_t;
};

typedef struct nodeTree node_t;

//-----------------------------------------------create nodes----------------------------------------------------
node_t * createNodeWithNum (elem_t num);
node_t * createNodeWithOperation (enum operationType operation, node_t * valLeftNode, node_t * valRightNode);
node_t * createNodeWithVariable (const char variableName);
//---------------------------------------------------------------------------------------------------------------

//--------------------------------------------graphical tree dump-------------------------------------------------
void graphicDumpTree (const node_t * node);
void dotFileHeaderTree (const node_t * node, const char * nameDotFileTree);
void writeNodeToDotFile (const node_t * node, FILE * graphicDump);
void writeEdgeToDotFile (const node_t * node, FILE * graphicDump);
void createDotFileTree (const char * nameDotFile, unsigned int timesCreatePicture);
static void createHtmlFileTree(const char * nameFileDump, unsigned int * timesCreatePicture);
//----------------------------------------------------------------------------------------------------------------



#endif
