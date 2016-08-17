/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#include "ArithmeticTranslator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define AT_STATE_LVALUE 1
#define AT_STATE_RVALUE 2
#define AT_STATE_OPERATOR 3
#define AT_STATE_RVALUE_REDUCE 4
#define AT_STATE_RVALUE_PUSH 5
#define AT_STATE_INITIAL 6

typedef struct AT_token
{
	int type;
	int value;
	char str[20];
} AT_token;

//private struct used for calculation
typedef struct AT_calc
{
	vector valueStack; //AT_token *
	vector stateStack; //int
	int state; //current state
	vector depthData; //AT_depthData *
	vector lines; //AT_depthData *
	int depth; //curr depthData index
} AT_calc;

typedef struct AT_depthData
{
	int addCount;
	int mulCount;
} AT_depthData;

static void catLineVector(char *out, vector *v, const char *head);
static int getVectorStrLen(vector *v);

/*
init/free
*/
static void Calc_init(AT_calc *calc);
static void Calc_destroy(AT_calc *calc);
static AT_token *AT_createToken(int value, int type);
static void AT_updateTokenStr(AT_token *t, int depth, int count, int op_type);
static AT_depthData *Calc_getDepth(AT_calc *calc);

//generates calculation string
static char *Calc_generateComment(AT_expression *expr, AT_calc *calc);

/* 0 - success */
/*
valueStack: push(pop * pop); add line
*/
static int Calc_mulTop(AT_calc *calc);

/*
valueStack: push(pop + pop); add line
*/
static int Calc_addTop(AT_calc *calc);

/*
value == number ? valueStack: push(value)
value == ( ? depth++, repeat 
*/
static int Calc_blockStart(AT_calc *calc, AT_token *tok);

/*
stateStack: while (pop != initial) Calc_addTop;
stateStack: back == after* ? Calc_mulTop
stateStack: back == after+ ? 
*/
static int Calc_blockEnd(AT_calc *calc);

/*
value == ( ? push(state) = initial
value == number ? push(value)
*/
static int Calc_afterAdd(AT_calc *calc, AT_token *tok);

/*
value == ( ? push(state) = initial
value == number ? push(value); Calc_mulTop
*/
static int Calc_afterMul(AT_calc *calc, AT_token *tok);

/*
value == ) ? Calc_blockEnd
value == + ? state = after+
value == * ? state = after*
*/
static int Calc_afterNumber(AT_calc *calc, AT_token *tok);

void AT_initExpression(AT_expression *expr)
{
	vector_init(&expr->tokens);
}

void AT_destroyExpression(AT_expression *expr)
{
	vector_deepFree(&expr->tokens);
}

void AT_addOperator(AT_expression *expr, int type)
{
	AT_token *t = (AT_token *)malloc(sizeof(AT_token));
	t->type = type;
	t->value = 0;
	
	vector_add(&expr->tokens, t);
}

void AT_addOperand(AT_expression *expr, int value)
{
	AT_token *t = (AT_token *)malloc(sizeof(AT_token));
	t->type = AT_TYPE_NUMERIC;
	t->value = value;
	
	vector_add(&expr->tokens, t);
}

int AT_performTranslation(AT_expression *expr, char **impComment, char **value)
{
	int ret = 0;
	
	AT_calc calculation;
	Calc_init(&calculation);
	calculation.depth = 1;
	vector *v = &expr->tokens;
	
	printf("Token count: %d\n", vector_count(v));
	int i;
	for (i = 0; i < vector_count(v) && !ret; i++)
	{
		AT_token *tok = vector_get(v, i);
		switch (calculation.state)
		{
			case AT_STATE_INITIAL:
				printf("AT_STATE_INITIAL       ret%d i%d type%d v%d\n", ret, i, tok->type, tok->value);
				ret = Calc_blockStart(&calculation, tok);
				break;
			case AT_STATE_RVALUE_PUSH:
				printf("AT_STATE_RVALUE_PUSH   ret%d i%d type%d v%d\n", ret, i, tok->type, tok->value);
				ret = Calc_afterAdd(&calculation, tok);
				break;
			case AT_STATE_RVALUE_REDUCE:
				printf("AT_STATE_RVALUE_REDUCE ret%d i%d type%d v%d\n", ret, i, tok->type, tok->value);
				ret = Calc_afterMul(&calculation, tok);
				break;
			case AT_STATE_OPERATOR:
				printf("AT_STATE_OPERATOR      ret%d i%d type%d v%d\n", ret, i, tok->type, tok->value);
				ret = Calc_afterNumber(&calculation, tok);
				break;
			default:
				printf("AT_performTranslation default");
				ret = -1;
				break;
		}
	}
	ret = Calc_blockEnd(&calculation);
	if (calculation.depth != 0)
		ret = -1;

	*value = (char *)malloc(30);
	snprintf(*value, 29, "%d", ((AT_token *)vector_back(&calculation.valueStack))->value);
	
	*impComment = Calc_generateComment(expr, &calculation);
	Calc_destroy(&calculation);
	
	printf("Value: %s\n", *value);
	printf("Comment: \n%s\n", *impComment);
	return ret;
}


static void Calc_init(AT_calc *calc)
{
	vector_init(&calc->valueStack); //AT_token *
	vector_init(&calc->stateStack); //int
	vector_init(&calc->lines); //char *
	vector_init(&calc->depthData); //AT_depthData *
	calc->state = AT_STATE_INITIAL;
	calc->depth = 0;
}

static void Calc_destroy(AT_calc *calc)
{
	vector_deepFree(&calc->valueStack); //AT_token *
	vector_free(&calc->stateStack); //int
	vector_deepFree(&calc->lines); //char *
	vector_deepFree(&calc->depthData); //AT_depthData *
}

static AT_token *AT_createToken(int value, int type)
{
	AT_token *t = (AT_token *)malloc(sizeof(AT_token));
	t->value = value;
	t->type = type;
	snprintf(t->str, 19, "%d", value);
	return t;
}


static void AT_updateTokenStr(AT_token *t, int depth, int count, int op_type)
{
	if (t->type == AT_TYPE_EXPR)
	{
		snprintf(t->str, 19, "L%d_%s%d", depth, op_type == AT_TYPE_PMULT ? "MUL" : "ADD", count);
	}
}

static char *Calc_getHeaderLine(AT_expression *expr)
{
	int i;
	vector *v = &expr->tokens;
	char *str = (char *)malloc(300);
	str[0] = 0;
	strcat(str, "\t\tOriginal expression: ");
	char num[30];
	for (i = 0; i < vector_count(v); i++)
	{
		AT_token *tok = vector_get(v, i);
		switch (tok->type)
		{
			//richness: 90 * (10 + 50) * 3;
			case AT_TYPE_POPEN:
				strcat(str, "(");
				break;
			case AT_TYPE_PCLOSE:
				strcat(str, ")");
				break;
			case AT_TYPE_PADD:
				strcat(str, " + ");
				break;
			case AT_TYPE_PMULT:
				strcat(str, " * ");
				break;
			case AT_TYPE_NUMERIC:
				snprintf(num, 29, "%d", tok->value);
				strcat(str, num);
				break;
			default:
				break;
		}
	}
	strcat(str, "\r\n");
	return str;
}

static char *Calc_generateComment(AT_expression *expr, AT_calc *calc)
{
	char *head = Calc_getHeaderLine(expr);
	char *ret = (char *)malloc(getVectorStrLen(&calc->lines)+strlen(head)+20);
	ret[0] = 0;
	strcat(ret, "\t\t/*\r\n");
	strcat(ret, head);
	catLineVector(ret, &calc->lines, NULL);
	strcat(ret, "\t\t*/");
	free(head);
	return ret;
}

static int Calc_mulTop(AT_calc *calc)
{
	int ret = 0;
	if (vector_count(&calc->valueStack) > 1)
	{
		AT_depthData *dd = Calc_getDepth(calc);
		dd->mulCount++;
		
		AT_token *rval = vector_pop_back(&calc->valueStack);
		AT_token *lval = vector_pop_back(&calc->valueStack);
		AT_token *newVal = AT_createToken(rval->value * lval->value, AT_TYPE_EXPR);
		AT_updateTokenStr(newVal, calc->depth, dd->mulCount, AT_TYPE_PMULT);
		
		char *line = malloc(101);
		snprintf(line, 100, "\t\t%s = %s * %s\r\n", newVal->str, lval->str, rval->str);
		
		vector_add(&calc->valueStack, newVal);
		vector_add(&calc->lines, line);
		
		free(rval);
		free(lval);
	}
	else
		ret = -1;
	return ret;
}

static int Calc_addTop(AT_calc *calc)
{
	int ret = 0;
	if (vector_count(&calc->valueStack) > 1)
	{
		AT_depthData *dd = Calc_getDepth(calc);
		dd->addCount++;
		
		AT_token *rval = vector_pop_back(&calc->valueStack);
		AT_token *lval = vector_pop_back(&calc->valueStack);
		AT_token *newVal = AT_createToken(rval->value + lval->value, AT_TYPE_EXPR);
		AT_updateTokenStr(newVal, calc->depth, dd->addCount, AT_TYPE_PADD);
		
		char *line = malloc(101);
		snprintf(line, 100, "\t\t%s = %s + %s\r\n", newVal->str, lval->str, rval->str);
		
		vector_add(&calc->valueStack, newVal);
		vector_add(&calc->lines, line);
		
		free(rval);
		free(lval);
	}
	else
		ret = -1;
	return ret;
}

static int Calc_afterMul(AT_calc *calc, AT_token *tok)
{
	int ret = 0;
	if (tok->type == AT_TYPE_NUMERIC)
	{
		vector_add(&calc->valueStack, AT_createToken(tok->value, tok->type));
		calc->state = AT_STATE_OPERATOR;
		ret = Calc_mulTop(calc);
	}
	else if (tok->type == AT_TYPE_POPEN)
	{
		vector_add(&calc->stateStack, (void *)AT_STATE_RVALUE_REDUCE);
		calc->state = AT_STATE_INITIAL;
		calc->depth++;
	}
	else
		ret = -1;
	return ret;
}


static int Calc_afterAdd(AT_calc *calc, AT_token *tok)
{
	int ret = 0;
	if (tok->type == AT_TYPE_NUMERIC)
	{
		vector_add(&calc->valueStack, AT_createToken(tok->value, tok->type));
		vector_add(&calc->stateStack, (void *)AT_STATE_RVALUE_PUSH);
		calc->state = AT_STATE_OPERATOR;
	}
	else if (tok->type == AT_TYPE_POPEN)
	{
		vector_add(&calc->stateStack, (void *)AT_STATE_RVALUE_PUSH);
		calc->state = AT_STATE_INITIAL;
		calc->depth++;
	}
	else
		ret = -1;
	return ret;
}

static int Calc_blockStart(AT_calc *calc, AT_token *tok)
{
	int ret = 0;
	vector_add(&calc->stateStack, (void *)AT_STATE_INITIAL);
	if (tok->type == AT_TYPE_POPEN)
		calc->depth++;	
	else if (tok->type == AT_TYPE_NUMERIC)
	{
		calc->state = AT_STATE_OPERATOR;
		vector_add(&calc->valueStack, AT_createToken(tok->value, tok->type));
	}
	else
		ret = -1;
	return ret;
}

static int Calc_blockEnd(AT_calc *calc)
{
	int ret = 0;
	if (vector_count(&calc->stateStack) > 0)
	{
		while (!ret && (int)vector_pop_back(&calc->stateStack) != AT_STATE_INITIAL)
			ret = Calc_addTop(calc);
		
		calc->depth--;
		if (calc->depth < 0)
			ret = -1;
			
		if (!ret && vector_count(&calc->stateStack))
		{
			if ((int)vector_back(&calc->stateStack) == AT_STATE_RVALUE_REDUCE)
			{
				vector_pop_back(&calc->stateStack); //consume AT_STATE_RVALUE_REDUCE
				ret = Calc_mulTop(calc);
			}
		}
		
		calc->state = AT_STATE_OPERATOR;
	}
	else 
		ret = -1;
	return ret;
}

static int Calc_afterNumber(AT_calc *calc, AT_token *tok)
{
	int ret = 0;
	if (tok->type == AT_TYPE_PADD)
	{
		calc->state = AT_STATE_RVALUE_PUSH;
	}
	else if (tok->type == AT_TYPE_PMULT)
	{
		calc->state = AT_STATE_RVALUE_REDUCE;
	}
	else if (tok->type == AT_TYPE_PCLOSE)
	{
		ret = Calc_blockEnd(calc);
	}
	else
		ret = -1;
	return ret;
}

static AT_depthData *Calc_getDepth(AT_calc *calc)
{
	while (vector_count(&calc->depthData) < calc->depth + 1)
	{
		AT_depthData *dd = (AT_depthData *)malloc(sizeof(AT_depthData));
		dd->addCount = 0;
		dd->mulCount = 0;
		vector_add(&calc->depthData, dd);
	}
	return vector_get(&calc->depthData, calc->depth);
}

static void catLineVector(char *out, vector *v, const char *head)
{
	int i = 0;
	for (i = 0; i < vector_count(v); i++)
	{
		//printf("IC: %s\n", vector_get(v, i));
		if (head)
			strcat(out, head);
		strcat(out, vector_get(v, i));
	}
	
}

static int getVectorStrLen(vector *v)
{
	int len = 0;
	int i = 0;
	for (i = 0; i < vector_count(v); i++)
		len += strlen(vector_get(v, i));
	return len;
}
