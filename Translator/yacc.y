/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

%{
#include <stdio.h>
#include <string.h>

#include "ArithmeticTranslator.h"
#include "Attribute.h"
#include "CodeGen.h"
#include "Node.h"
#include "Selector.h"
#include "vector.h"

static vector *currentBlockList = NULL;
static Node *currentBlock = NULL;
static vector *currentSelectorGroupList = NULL;
static vector *currentSelectorGroup = NULL;
static Selector *currentSelector = NULL;
static vector *currentAttrList = NULL;
static Attribute *currentAttr = NULL;
char tempValue[1024] = {0};
char attrComments[4096] = {0};
AT_expression *currentExpr = NULL;

void resetHierarchy();

extern char *yytext;
#define YYDEBUG_LEXER_TEXT yytext
int yydebug=0;
void yyerror(const char *str)
{
	fprintf(stderr, "ERROR: %s\n", str);
	CG_abort();
}

AT_expression *getCurrentExpression();

%}

%union
{
    int intval;
    char* text;
}

%token MEDIA CLASS ID EQ_OP STRING SIZE URL IDENTIFIER BRACE_OPEN BRACE_CLOSE BRACK_OPEN BRACK_CLOSE 
%token NUMBER COMMA SEMICOLON COLON ADD MULT PAREN_OPEN PAREN_CLOSE 

%type<intval> NUMBER 
%type<text> MEDIA CLASS ID STRING SIZE URL IDENTIFIER attrName

%%

styleSheet:
	styleBlock
	|
	styleSheet styleBlock
	{
	}
	;

styleBlock:
	styleEntry
	{
		CG_parseNodes(currentBlockList);
		resetHierarchy();
	}
	|
	mediaBlock
	{
		CG_parseNodes(currentBlockList);
		resetHierarchy();
	}
	;

mediaBlock:
	MEDIA BRACE_OPEN BRACE_CLOSE
	{
	
	}
	|
	MEDIA BRACE_OPEN styleEntryRep BRACE_CLOSE
	{
		printf("End of media block: %s\n", $1);
		CG_setMediaBlock($1);
	}
	;

styleEntryRep:
	styleEntry
	{
	}
	|
	styleEntryRep styleEntry
	{
	}
	;

styleEntry:
	selectorGroup BRACE_OPEN BRACE_CLOSE
	{
	}
	|
	selectorGroup BRACE_OPEN attributeList BRACE_CLOSE
	{
		if (!currentBlockList)
		{
			currentBlockList = (vector *)malloc(sizeof(vector));
			vector_init(currentBlockList);
		}
		currentBlock = (Node *)malloc(sizeof(Node));
		Node_init(currentBlock, currentSelectorGroupList, currentAttrList);
		vector_add(currentBlockList, currentBlock);
		//TODO: fix leak
		currentAttrList = (vector *)malloc(sizeof(vector));
		vector_init(currentAttrList);
		currentSelectorGroupList = (vector *)malloc(sizeof(vector));
		vector_init(currentSelectorGroupList);
	}
	;

selectorGroup:
	selector
	{
		if (!currentSelectorGroupList)
		{
			currentSelectorGroupList = (vector *)malloc(sizeof(vector));
			vector_init(currentSelectorGroupList);
		}
		vector_add(currentSelectorGroupList, currentSelectorGroup);
		currentSelectorGroup = (vector *)malloc(sizeof(vector));
		vector_init(currentSelectorGroup);
	}
	|
	selectorGroup COMMA selector
	{
		if (!currentSelectorGroupList)
		{
			currentSelectorGroupList = (vector *)malloc(sizeof(vector));
			vector_init(currentSelectorGroupList);
		}
		vector_add(currentSelectorGroupList, currentSelectorGroup);
		currentSelectorGroup = (vector *)malloc(sizeof(vector));
		vector_init(currentSelectorGroup);
	}
	;

selector:
	simpleSelector
	|
	selector simpleSelector
	;
	
simpleSelector:
	mainSelector
	|
	otherSelector
	;

mainSelector:
	IDENTIFIER
	{
		if (!currentSelectorGroup)
		{
			currentSelectorGroup = (vector *)malloc(sizeof(vector));
			vector_init(currentSelectorGroup);
		}
		currentSelector = (Selector *)malloc(sizeof(Selector));
		Selector_initWithID(currentSelector, $1, SEL_MAIN);
		vector_add(currentSelectorGroup, currentSelector);
		printf("Found main selector: %s\n", $1);
	}
	;

otherSelector:
	attrSelector | idSelector | pseudoElement
	;

pseudoElement:
	/*PSEUDOCLASS*/
	COLON IDENTIFIER
	{
		if (!currentSelectorGroup)
		{
			currentSelectorGroup = (vector *)malloc(sizeof(vector));
			vector_init(currentSelectorGroup);
		}
		currentSelector = (Selector *)malloc(sizeof(Selector));
		Selector_initWithID(currentSelector, $2, SEL_PSEUDO);
		vector_add(currentSelectorGroup, currentSelector);
		printf("Found pseudo element selector: %s\n", $2);
	}
	;

idSelector:
	ID
	{
		if (!currentSelectorGroup)
		{
			currentSelectorGroup = (vector *)malloc(sizeof(vector));
			vector_init(currentSelectorGroup);
		}
		currentSelector = (Selector *)malloc(sizeof(Selector));
		Selector_initWithID(currentSelector, $1, SEL_ID);
		vector_add(currentSelectorGroup, currentSelector);
		printf("Found ID selector: %s\n", $1);
	}
	;


attrSelector:
	attributeSelector
	|
	classSelector
	;

classSelector:
	CLASS
	{
		if (!currentSelectorGroup)
		{
			currentSelectorGroup = (vector *)malloc(sizeof(vector));
			vector_init(currentSelectorGroup);
		}
		currentSelector = (Selector *)malloc(sizeof(Selector));
		Selector_initWithID(currentSelector, $1, SEL_CLASS);
		vector_add(currentSelectorGroup, currentSelector);
		printf("Found class selector: %s\n", $1);
	}
	;

attributeSelector:
	BRACK_OPEN IDENTIFIER BRACK_CLOSE
	{
		if (!currentSelectorGroup)
		{
			currentSelectorGroup = (vector *)malloc(sizeof(vector));
			vector_init(currentSelectorGroup);
		}
		Attribute at;
		Attribute_init(&at, $2, NULL, NULL);
		currentSelector = (Selector *)malloc(sizeof(Selector));
		Selector_initWithAttr(currentSelector, &at);
		vector_add(currentSelectorGroup, currentSelector);
		printf("Found attribute selector: %s\n", $2);
	}
	|
	BRACK_OPEN IDENTIFIER EQ_OP attrSelectorValue BRACK_CLOSE
	{
		if (!currentSelectorGroup)
		{
			currentSelectorGroup = (vector *)malloc(sizeof(vector));
			vector_init(currentSelectorGroup);
		}
		Attribute at;
		Attribute_init(&at, $2, tempValue, NULL);
		currentSelector = (Selector *)malloc(sizeof(Selector));
		Selector_initWithAttr(currentSelector, &at);
		vector_add(currentSelectorGroup, currentSelector);
		printf("Found attribute selector: %s=%s\n", $2, tempValue);
		tempValue[0] = 0;
	}
	;

attrSelectorValue:
	STRING 
	{
		strcat(tempValue, $1);
	}
	| 
	identifierRep
	{
		
	}
	;

/* handles a case when last assignment doesn't end in semicolon*/
attributeList:
	attributeRep attrName attributeValueList SEMICOLON
	{
		if (!currentAttrList)
		{
			currentAttrList = (vector *)malloc(sizeof(vector));
			vector_init(currentAttrList);
		}
		tempValue[strlen(tempValue) - 1] = 0;
		currentAttr = (Attribute *)malloc(sizeof(Attribute));
		Attribute_init(currentAttr, $2, tempValue, attrComments);
		tempValue[0] = 0;
		attrComments[0] = 0;
		vector_add(currentAttrList, currentAttr);
	}
	|
	attributeRep attrName attributeValueList
	{
		if (!currentAttrList)
		{
			currentAttrList = (vector *)malloc(sizeof(vector));
			vector_init(currentAttrList);
		}
		tempValue[strlen(tempValue) - 1] = 0;
		currentAttr = (Attribute *)malloc(sizeof(Attribute));
		Attribute_init(currentAttr, $2, tempValue, attrComments);
		tempValue[0] = 0;
		attrComments[0] = 0;
		vector_add(currentAttrList, currentAttr);
	}
	;
	
attributeRep:
	|
	attributeRep attrName attributeValueList SEMICOLON
	{
		if (!currentAttrList)
		{
			currentAttrList = (vector *)malloc(sizeof(vector));
			vector_init(currentAttrList);
		}
		tempValue[strlen(tempValue) - 1] = 0;
		currentAttr = (Attribute *)malloc(sizeof(Attribute));
		Attribute_init(currentAttr, $2, tempValue, attrComments);
		tempValue[0] = 0;
		attrComments[0] = 0;
		vector_add(currentAttrList, currentAttr);
	}
	;

attrName:
	IDENTIFIER COLON
	{
		{ $$ = $1; }
	}
	;

attributeValueList:
	attributeValue
	{
		strcat(tempValue, ",");
	}
	| 
	attributeValueList COMMA attributeValue
	{
		strcat(tempValue, ",");
	}
	;

attributeValue:
	STRING { strcat(tempValue, $1); }
	| arithmeticExpression 
	{
		char *comments = NULL;
		char *result = NULL;
		int err = AT_performTranslation(getCurrentExpression(), &comments, &result);
		if (!err)
		{
			strcat(tempValue, result);
			if (comments)
				strcat(attrComments, comments);
			free(comments);
			free(result);
		}
		else
		{
			yyerror("Syntax error within arithmetic expression");
			YYABORT;
		}
		AT_destroyExpression(currentExpr);
		free(currentExpr);
		currentExpr = NULL;		
	}
	| sizeRep {}
	| ID { strcat(tempValue, $1); }
	| identifierRep { } 
	| URL { strcat(tempValue, $1); }
	;
	
arithmeticExpression:
	NUMBER { AT_addOperand(getCurrentExpression(), $1); }
	| PAREN_OPEN { AT_addOperator(getCurrentExpression(), AT_TYPE_POPEN); }
	| PAREN_CLOSE { AT_addOperator(getCurrentExpression(), AT_TYPE_PCLOSE); }
	| MULT { AT_addOperator(getCurrentExpression(), AT_TYPE_PMULT); }
	| ADD { AT_addOperator(getCurrentExpression(), AT_TYPE_PADD); }
	| arithmeticExpression NUMBER {AT_addOperand(getCurrentExpression(), $2);  }
	| arithmeticExpression PAREN_OPEN { AT_addOperator(getCurrentExpression(), AT_TYPE_POPEN); }
	| arithmeticExpression PAREN_CLOSE { AT_addOperator(getCurrentExpression(), AT_TYPE_PCLOSE); }
	| arithmeticExpression MULT { AT_addOperator(getCurrentExpression(), AT_TYPE_PMULT); }
	| arithmeticExpression ADD { AT_addOperator(getCurrentExpression(), AT_TYPE_PADD); }
	;
	
identifierRep:
	IDENTIFIER
	{
		strcat(tempValue, $1);
	}
	|
	identifierRep IDENTIFIER
	{
		strcat(tempValue, " ");
		strcat(tempValue, $2);
	}
	;

sizeRep:
	SIZE
	{
		strcat(tempValue, $1);
	}
	|
	sizeRep SIZE
	{
		strcat(tempValue, " ");
		strcat(tempValue, $2);
	}
	;

%%

void resetHierarchy()
{
	int count = 0;
	for (count = 0; count < vector_count(currentBlockList); count++)
	{	
		Node *node = (Node *)vector_get(currentBlockList, count);
		Node_destroy(node);
		free(node);
	}
	CG_setMediaBlock(NULL);
	vector_free(currentBlockList);
	free(currentBlockList);
	currentBlockList = (vector *)malloc(sizeof(vector));
	vector_init(currentBlockList);
}

AT_expression *getCurrentExpression()
{
	if (!currentExpr)
	{
		currentExpr = (AT_expression *)malloc(sizeof(AT_expression));
		AT_initExpression(currentExpr);
	}
	return currentExpr;
}



	