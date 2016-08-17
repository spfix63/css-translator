/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#ifndef CODEGEN_H
#define CODEGEN_H

#include "vector.h"

int CG_init(const char *headerFn, const char *codeFn);
void CG_abort();
void CG_destroy();
void CG_setMediaBlock(const char *media);
void CG_parseNodes(vector *nodes);
void CG_addHeaderCode(const char *code);
void CG_addImplementationCode(const char *code);
int CG_flush();


#endif
