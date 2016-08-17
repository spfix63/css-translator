/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "vector.h"

typedef struct Attribute
{
	char *name;
	char *value;
	char *prep;
} Attribute;

/* param[i] prepCode code inserted just before attribute assignment code */
void Attribute_init(Attribute *attr, const char *name, const char *value, const char *prepCode);
void Attribute_initWithCopy(Attribute *attr, Attribute *in);
void Attribute_destroy(Attribute *attr);

char *Attribute_toStringPrep(Attribute *attr);
char *Attribute_toString(Attribute *attr);
int Attribute_getLengthPrep(Attribute *attr);
int Attribute_getLength(Attribute *attr);

#endif
