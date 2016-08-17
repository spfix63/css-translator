/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#ifndef HEADERGEN_H
#define HEADERGEN_H

#include "MethodDeclaration.h"
#include "VariableDeclaration.h"
#include "vector.h"

typedef struct CppClass
{
	//no multiple inheritence
	//no friends
	char *name;
	char *parent;

	vector publicMethods;
	vector protectedMethods;
	vector privateMethods;
	
	vector publicVars;
	vector protectedVars;
	vector privateVars;
} CppClass;

void CppClass_init(CppClass *h, const char *className, const char *parent);
void CppClass_destroy(CppClass *h);

void CppClass_addPublicMethod(CppClass *h, MethodDeclaration *m);
void CppClass_addProtectedMethod(CppClass *h, MethodDeclaration *m);
void CppClass_addPrivateMethod(CppClass *h, MethodDeclaration *m);
void CppClass_addPublicVariable(CppClass *h, VariableDeclaration *v);
void CppClass_addProtectedVariable(CppClass *h, VariableDeclaration *v);
void CppClass_addPrivateVariable(CppClass *h, VariableDeclaration *v);
char *CppClass_declarationString(CppClass *h);
char *CppClass_implementationString(CppClass *h);

#endif
