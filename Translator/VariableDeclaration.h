/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#ifndef VARIABLEDECLARATION_H
#define VARIABLEDECLARATION_H

typedef struct VariableDeclaration
{
//no const
//no static
	char *name;
	char *type;
} VariableDeclaration;

void VariableDeclaration_init(VariableDeclaration *v, const char *name, const char *type);
void VariableDeclaration_initWithCopy(VariableDeclaration *v, VariableDeclaration *in);
void VariableDeclaration_destroy(VariableDeclaration *v);
char *VariableDeclaration_toString(VariableDeclaration *v);

#endif
