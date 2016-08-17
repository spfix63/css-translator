/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#ifndef ARITHMETICTRANSLATOR_H
#define ARITHMETICTRANSLATOR_H

#include "vector.h"

#define AT_TYPE_POPEN 1
#define AT_TYPE_PCLOSE 2
#define AT_TYPE_PADD 3
#define AT_TYPE_PMULT 4
#define AT_TYPE_NUMERIC 5 //used for internal calc
#define AT_TYPE_EXPR 6 //used for internal calc

typedef struct AT_expression
{
	vector tokens; //AT_token *
} AT_expression;


void AT_initExpression(AT_expression *expr);
void AT_destroyExpression(AT_expression *expr);

void AT_addOperator(AT_expression *expr, int type);
void AT_addOperand(AT_expression *expr, int value);

/*
Translates arithmetic expression to C++ block.

@note basic usage:
char *imp;
char *value;
int err = AT_performTranslation(AT_expression *expr char **impComment, char **value);
if (!err) { 
	success... 
	free(imp);
	free(value);
}
On error no memory will be allocated

@param expr - arithmetic expression to be translated..
@param[out] impComment - C++ comment
@param[out] value - calculated value of the expression
@return 0 success, otherwise error. translated buffer. NULL on error.
*/

int AT_performTranslation(AT_expression *expr, char **impComment, char **value);


#endif

