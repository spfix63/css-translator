/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#ifndef METHODDECLARATION_H
#define METHODDECLARATION_H

#include "VariableDeclaration.h"
#include "vector.h"

struct MethodImplementation;
typedef struct MethodDeclaration
{
//no const
//no volatile
//no static
	char *name;
	char *type;
	int isVirtual;
	vector variableList;
	struct MethodImplementation *implementation;
} MethodDeclaration;

void MethodDeclaration_init(MethodDeclaration *m, const char *name, const char *type);
void MethodDeclaration_initWithCopy(MethodDeclaration *m, MethodDeclaration *in);
void MethodDeclaration_destroy(MethodDeclaration *m);
void MethodDeclaration_setVirtual(MethodDeclaration *m, int is);
void MethodDeclaration_addVariableDeclaration(MethodDeclaration *m, VariableDeclaration *v);
void MethodDeclaration_addCode(MethodDeclaration *m, const char *code);
char *MethodDeclaration_implementationString(MethodDeclaration *m, const char *className);
char *MethodDeclaration_toString(MethodDeclaration *m);

#endif