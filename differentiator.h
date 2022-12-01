#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
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

//--------------------------------------------operations with a tree--------------------------------------------
void removeSpaces (char * dest, const char * source);
node_t * getGrammar ();
node_t * getExpression (void);
node_t * getT (void);
node_t * getDegree (void);
node_t * getBracket (void);
node_t * getNumber (void);
void deleteNode (node_t * node);
//--------------------------------------------------------------------------------------------------------------

//---------------------------------------------dump to console--------------------------------------------------
void textDump (node_t * node, FILE * log, unsigned int isLast, unsigned int numTABs);
void printfTab (unsigned int numTABs, FILE * log);
//--------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------support functions---------------------------------------------------------
int numOfLetters (const char * string);
//---------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------creating a tree after differentiation------------------------------------------------
void simplifyExpression (node_t ** node);
node_t * getGrammarForDif (node_t * node); //возможно, когда сделаешь двойной указатель, потом начнешь вылезать за строку из-за сдвига
node_t * getExpressionForDif (node_t * node);
node_t * difMulDiv (node_t * node);
node_t * difDegree (node_t * node);
node_t * difNumberOrVar (node_t * node);
node_t * difFunc (node_t * node);
//----------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------simplify-------------------------------------------------------------
void simplifyDegOrDivToOne (node_t ** node);
void simplifyMulByZero (node_t * node);
void simplifyMulByOne (node_t ** node);
void simplifyAddWithZero (node_t ** node);
int compareFractionalNum (double firstNum, double secondNum);
void removeConstants (node_t ** node);
//---------------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------dump to tex file------------------------------------------------------------
void selectingNameOfLatexFile (void);
//----------------------------------------------------------------------------------------------------------------------------------


#endif
