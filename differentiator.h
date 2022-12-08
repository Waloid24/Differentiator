#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
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
node_t * getGrammar (void);
node_t * getExpression (char ** str);
node_t * getT (char ** str);
node_t * getDegree (char ** str);
node_t * getBracket (char ** str);
node_t * getNumber (char ** str);
void deleteNode (node_t * node);
void deleteTree (node_t * node);

//--------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------support functions---------------------------------------------------------
FILE * openTexfile (void);
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

void texStart (FILE * texfile);
void selectingNameOfLatexFile (void);
void texPrintNode (FILE * texfile, node_t * node);
void texPrintOperation (FILE * texfile, node_t * node);
void texPrintVar (FILE * texfile, node_t * node);
void texPrintNum (FILE * texfile, node_t * node);
void texPrintFunc (FILE * texfile, node_t * node);
void texPrintConst (FILE * texfile, node_t * node);

int texFinish(FILE * texfile);

//----------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------decompose by the Taylor formula-----------------------------------------------------

void decomposeByTaylor (node_t * node, FILE * texfile, char varInEquation);
void fillArrOfNodes (node_t * node, node_t ** arrNodes, int n);
void resetVar (node_t * node);
int checkInput (int * degreeOfNum);
void startEquation (FILE * texfile, char var);
void startDifEquation (FILE * texfile, char var);
void endEquation (FILE * texfile);
char saveVar (const node_t * node);

//--------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------build a graph using python------------------------------------------------------

void buildGraph (node_t * node, FILE * texfile);
void pyPrintNode (FILE * pyfile, node_t * node);
void pyPrintOperation (FILE * pyfile, node_t * node);
void pyPrintVar (FILE * pyfile, node_t * node);
void pyPrintNum (FILE * pyfile, node_t * node);
void pyPrintFunc (FILE * pyfile, node_t * node);
void pyPrintConst (FILE * pyfile, node_t * node);

//--------------------------------------------------------------------------------------------------------------------------------

#endif
