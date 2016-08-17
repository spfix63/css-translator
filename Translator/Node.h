/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#ifndef NODE_H
#define NODE_H

#include "vector.h"

typedef struct Node
{
	vector selectorGroup; //vector of vectors :(
	vector attributes;
	char *media;
} Node;

void Node_init(Node *node, vector *selectorGroup, vector *attributes);
void Node_initWithMedia(Node *node, vector *selectorGroup, vector *attributes, const char *media);
void Node_destroy(Node *node);
void Node_setMedia(Node *node, const char *media);
#endif
