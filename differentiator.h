#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include <stdio.h>
#include <assert.h>
#include "tree.h"

enum IS_LAST {
    NO_LAST = 0,
    LAST = 1
};

enum type_t {
    OPERATION = 1,
    VAR = 2,
    NUM = 3
};

enum op_t {
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
    EXP = 5,
    ROOT = 6
};

//-------------------------------------------------building a tree----------------------------------------------
void removeSpaces (char * dest, const char * source);
node_t * getGrammar (const char * str);
node_t * getExpression (void);
node_t * getT (void);
node_t * getDegree (void);
node_t * getBracket (void);
node_t * getNumber (void);
//--------------------------------------------------------------------------------------------------------------

//---------------------------------------------dump to console--------------------------------------------------
void textDump (node_t * node, FILE * log, unsigned int isLast, unsigned int numTABs);
void printfTab (unsigned int numTABs, FILE * log);
//--------------------------------------------------------------------------------------------------------------


#endif
