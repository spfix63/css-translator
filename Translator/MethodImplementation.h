/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#ifndef METHODIMPLEMENTATION_H
#define METHODIMPLEMENTATION_H

#include "MethodDeclaration.h"
#include "vector.h"

typedef struct MethodImplementation
{
	char *code;
	int codeSize;
} MethodImplementation;

void MethodImplementation_init(MethodImplementation *m);
void MethodImplementation_initWithCopy(MethodImplementation *m, MethodImplementation *in);
void MethodImplementation_destroy(MethodImplementation *m);
void MethodImplementation_addCode(MethodImplementation *m, const char *code);

char *MethodImplementation_toString(MethodImplementation *m, MethodDeclaration *d, const char *className);
#endif