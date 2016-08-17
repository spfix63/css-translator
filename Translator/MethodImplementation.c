/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MethodImplementation.h"
#include "VariableDeclaration.h"


void MethodImplementation_init(MethodImplementation *m)
{
	m->code = NULL;
	m->codeSize = 0;
}

void MethodImplementation_initWithCopy(MethodImplementation *m, MethodImplementation *in)
{
	if (in->code)
		m->code = strdup(in->code);
	else
		m->code = NULL;
	m->codeSize = in->codeSize;
}

void MethodImplementation_destroy(MethodImplementation *m)
{
	if (m->code)
	{
		free(m->code);
		m->code = NULL;
	}
}

void MethodImplementation_addCode(MethodImplementation *m, const char *code)
{
	if (!m->code)
	{
		m->codeSize = strlen(code) + 1;
		m->code = (char *)malloc(m->codeSize);
		m->code[0] = 0;
	}
	else
	{
		int size = m->codeSize + strlen(code) + 1;
		if (size > m->codeSize)
		{
			m->code = realloc(m->code, size);
			m->codeSize = size;
		}
	}
	strcat(m->code, code);
}

char *MethodImplementation_toString(MethodImplementation *m, MethodDeclaration *d, const char *className)
{
	char *ret = NULL;
	char *head = NULL;
	int headLen = 0;
	int retLen = 0;
	int i = 0;
	headLen += strlen(d->type);
	headLen += strlen(" ()\r\n");
	if (className)
	{
		headLen += strlen(className) + strlen("::");
	}
	headLen += strlen(d->name);
	for (i = 0; i < vector_count(&d->variableList); i++)
	{
		VariableDeclaration *var = (VariableDeclaration *)vector_get(&d->variableList, i);
		headLen += strlen(var->type);
		headLen += strlen(var->name);
	}
	if (i > 0)
		headLen += 2*(i-1);//space and commas in between arguments
		
		
	head = (char *)calloc(sizeof(char), headLen+1);
	if (className)
		sprintf(head, "%s %s%s%s(", d->type, className, "::", d->name);
	else
		sprintf(head, "%s %s(", d->type, d->name);
	
	for (i = 0; i < vector_count(&d->variableList) - 1; i++)
	{
		VariableDeclaration *var = (VariableDeclaration *)vector_get(&d->variableList, i);
		strcat(head, var->type);
		strcat(head, var->name);
		strcat(head, ", ");
	}
	if (i > 0)
	{
		VariableDeclaration *var = (VariableDeclaration *)vector_get(&d->variableList, i);
		strcat(head, var->type);
		strcat(head, var->name);
	}
	strcat(head, ")\r\n");
	
	retLen = strlen("{\r\n\r\n}") + strlen(head);
	retLen += m->code ? strlen(m->code) : 0;
	ret = (char *)calloc(sizeof(char), retLen + 1);
	
	strcpy(ret, head);
	strcat(ret, "{\r\n");
	if (m->code)
		strcat(ret, m->code);
	strcat(ret, "\r\n}");
	free(head);
	
	return ret;
}
