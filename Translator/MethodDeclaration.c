/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MethodDeclaration.h"
#include "MethodImplementation.h"



void MethodDeclaration_init(MethodDeclaration *m, const char *name, const char *type)
{
	m->name = strdup(name);
	m->type = strdup(type);
	m->isVirtual = 0;
	vector_init(&m->variableList);
	m->implementation = (MethodImplementation *)malloc(sizeof(MethodImplementation));
	MethodImplementation_init(m->implementation);
}

void MethodDeclaration_initWithCopy(MethodDeclaration *m, MethodDeclaration *in)
{
	//printf(("MethodDeclaration_initWithCopy1\n");
	m->name = strdup(in->name);
	m->type = strdup(in->type);
	m->isVirtual = in->isVirtual;
	//printf(("MethodDeclaration_initWithCopy2\n");
	vector_init(&m->variableList);
	vector *v = &in->variableList;
	//printf(("MethodDeclaration_initWithCopy3\n");
	int i = 0;
	for (i = 0; i < vector_count(v); i++)
	{
		VariableDeclaration *var = vector_get(v, i);
		VariableDeclaration *copy = (VariableDeclaration *)malloc(sizeof(VariableDeclaration));
		VariableDeclaration_initWithCopy(copy, var);
		vector_add(&m->variableList, copy);
	}
	//printf(("MethodDeclaration_initWithCopy4\n");
	m->implementation = (MethodImplementation *)malloc(sizeof(MethodImplementation));
	MethodImplementation_initWithCopy(m->implementation, in->implementation);
	//printf(("MethodDeclaration_initWithCopy5\n");
}

void MethodDeclaration_destroy(MethodDeclaration *m)
{
	vector *v = &m->variableList;
	int i = 0;
	for (i = 0; i < vector_count(v); i++)
	{
		VariableDeclaration *var = vector_get(v, i);
		VariableDeclaration_destroy(var);
		free(var);
	}
	vector_free(v);
	if (m->implementation)
	{
		MethodImplementation_destroy(m->implementation);
		free(m->implementation);
		m->implementation = NULL;
	}
}

void MethodDeclaration_setVirtual(MethodDeclaration *m, int is)
{
	m->isVirtual = is;
}

void MethodDeclaration_addVariableDeclaration(MethodDeclaration *m, VariableDeclaration *v)
{
	VariableDeclaration *copy = (VariableDeclaration *)malloc(sizeof(VariableDeclaration));
	VariableDeclaration_initWithCopy(copy, v);
	vector_add(&m->variableList, copy);
}

void MethodDeclaration_addCode(MethodDeclaration *m, const char *code)
{
	MethodImplementation_addCode(m->implementation, code);
}

char *MethodDeclaration_implementationString(MethodDeclaration *m, const char *className)
{
	return MethodImplementation_toString(m->implementation, m, className);
}

char *MethodDeclaration_toString(MethodDeclaration *m)
{
	char *ret = NULL;
	int retLen = 0;
	int i = 0;
	retLen += strlen(" ();");
	if (m->isVirtual)
		retLen += strlen("virtual ");
	retLen += strlen(m->type);
	retLen += strlen(m->name);
	for (i = 0; i < vector_count(&m->variableList); i++)
	{
		VariableDeclaration *var = (VariableDeclaration *)vector_get(&m->variableList, i);
		retLen += strlen(var->type);
		retLen += strlen(var->name);
	}
	if (i > 0)
		retLen += 3*(i-1);//space and commas in between arguments and space between var type and name
	
	ret = (char *)calloc(retLen + 1, sizeof(char));
	if (m->isVirtual)
		sprintf(ret, "virtual %s %s(", m->type, m->name);
	else
		sprintf(ret, "%s %s(", m->type, m->name);
	for (i = 0; i < vector_count(&m->variableList) - 1; i++)
	{
		VariableDeclaration *var = (VariableDeclaration *)vector_get(&m->variableList, i);
		strcat(ret, var->type);
		strcat(ret, var->name);
		strcat(ret, ", ");
	}
	if (i > 0)
	{
		VariableDeclaration *var = (VariableDeclaration *)vector_get(&m->variableList, i);
		strcat(ret, var->type);
		strcat(ret, var->name);
	}
	strcat(ret, ");");
	return ret;
}
