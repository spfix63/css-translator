/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Attribute.h"
#include "Node.h"
#include "Selector.h"

#include "vector.h"


void Node_init(Node *node, vector *selectorGroup, vector *attributes)
{
	int i;
	vector *v;
	vector_init(&node->selectorGroup);
	//printf("Node: s%p a%p\n", selectorGroup, attributes);	
	node->media = NULL;
	if (selectorGroup)
	{
		i = 0;
		v = selectorGroup;
		for (i = 0; i < vector_count(v); i++)
		{
			int j = 0;
			vector *vcopy;
			vector *w;
			vcopy = (vector *)malloc(sizeof(vector));
			w = vector_get(v, i);
			vector_init(vcopy);
			for (i = 0; i < vector_count(w); i++)
			{
				Selector *var = vector_get(w, i);
				Selector *scopy = (Selector *)malloc(sizeof(Selector));
				Selector_initWithCopy(scopy, var);
				vector_add(vcopy, scopy);
			}		
			vector_add(&node->selectorGroup, vcopy);
		}
	}
	vector_init(&node->attributes);
	if (attributes)
	{
		i = 0;
		v = attributes;
	
		for (i = 0; i < vector_count(v); i++)
		{
			Attribute *var = vector_get(v, i);
			Attribute *acopy = (Attribute *)malloc(sizeof(Attribute));
			Attribute_initWithCopy(acopy, var);
			vector_add(&node->attributes, acopy);
		}
	}
}

void Node_initWithMedia(Node *node, vector *selectorGroup, vector *attributes, const char *media)
{
	Node_init(node, selectorGroup, attributes);
	node->media = media ? strdup(media) : NULL;
}

void Node_destroy(Node *node)
{
	int i;
	if (node->media)
		free(node->media);
	for (i = 0; i < vector_count(&node->attributes); i++)
	{
		Attribute *var = vector_get(&node->attributes, i);
		Attribute_destroy(var);
		free(var);
	}
	vector_free(&node->attributes);
	
	for (i = 0; i < vector_count(&node->selectorGroup); i++)
	{
		int j = 0;
		vector *w = vector_get(&node->selectorGroup, i);
		for (i = 0; i < vector_count(w); i++)
		{
			Selector *var = vector_get(w, i);
			Selector_destroy(var);
			free(var);
		}		
		vector_free(w);
		free(w);
	}
	vector_free(&node->selectorGroup);
}

void Node_setMedia(Node *node, const char *media)
{
	if (node->media)
	{
		free(node->media);
		node->media = NULL;
	}
	node->media = media ? strdup(media) : NULL;
}
