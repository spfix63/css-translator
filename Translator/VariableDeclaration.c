/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "VariableDeclaration.h"

void VariableDeclaration_init(VariableDeclaration *v, const char *name, const char *type)
{
	v->name = strdup(name);
	v->type = strdup(type);
}

void VariableDeclaration_initWithCopy(VariableDeclaration *v, VariableDeclaration *in)
{
	v->name = strdup(in->name);
	v->type = strdup(in->type);
}

void VariableDeclaration_destroy(VariableDeclaration *v)
{
	if (v->name)
	{
		free(v->name);
		v->name = NULL;
	}
	if (v->type)
	{
		free(v->type);
		v->type = NULL;
	}
}


char *VariableDeclaration_toString(VariableDeclaration *v)
{
	char *ret = NULL;
	int retLen = 0;
	retLen += strlen(" ;");
	retLen += strlen(v->type);
	retLen += strlen(v->name);
	ret = (char *)calloc(retLen + 1, sizeof(char));
	sprintf(ret, "%s %s;", v->type, v->name);
	return ret;
}
