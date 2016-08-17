/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#include <stdlib.h>
#include <string.h>

#include "Selector.h"

void Selector_initWithID(Selector *node, const char *name, int type)
{
	node->identifier = strdup(name);
	node->attr = NULL;
	node->type = type;
}

void Selector_initWithAttr(Selector *node, Attribute *attr)
{
	node->identifier = NULL;
	node->attr = (Attribute *) malloc(sizeof(Attribute));
	Attribute_initWithCopy(node->attr, attr);
	node->type = SEL_ATTR;
}

void Selector_initWithCopy(Selector *s, Selector *in)
{
	if (in->identifier)
		s->identifier = strdup(in->identifier);
	else
		s->identifier = NULL;
	if (in->attr)
	{
		s->attr = (Attribute *) malloc(sizeof(Attribute));
		Attribute_initWithCopy(s->attr, in->attr);
	}
	else
		s->attr = NULL;
	s->type = in->type;
}

void Selector_destroy(Selector *node)
{
	if (node->identifier)
		free(node->identifier);
	if (node->attr)
		free(node->attr);
}

