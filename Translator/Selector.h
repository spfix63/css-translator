/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#ifndef SELECTOR_H
#define SELECTOR_H

#define SEL_PSEUDO 0
#define SEL_ID 1
#define SEL_ATTR 2
#define SEL_CLASS 3
#define SEL_MAIN 4

#include "Attribute.h"

typedef struct Selector
{
	char *identifier;
	struct Attribute *attr;
	int type;
} Selector;

void Selector_initWithID(Selector *node, const char *name, int type);
void Selector_initWithAttr(Selector *node, Attribute *attr);
void Selector_initWithCopy(Selector *s, Selector *in);
void Selector_destroy(Selector *node);

#endif
