/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

void vector_init(vector *v)
{
	v->data = NULL;
	v->size = 0;
	v->count = 0;
}

int vector_count(vector *v)
{
	return v->count;
}

void vector_add(vector *v, void *e)
{
	if (v->size == 0) {
		v->size = 10;
		v->data = malloc(sizeof(void*) * v->size);
		memset(v->data, '\0', sizeof(void) * v->size);
	}

	if (v->size == v->count) {
		v->size *= 2;
		v->data = realloc(v->data, sizeof(void*) * v->size);
	}

	v->data[v->count] = e;
	v->count++;
}

void vector_set(vector *v, int index, void *e)
{
	if (index >= v->count) {
		return;
	}

	v->data[index] = e;
}

void *vector_get(vector *v, int index)
{
	if (index >= v->count) {
		return NULL;
	}

	return v->data[index];
}

void *vector_back(vector *v)
{
	void *ret;
	if (v->count > 0) 
		ret = v->data[v->count - 1];
	else
		ret = NULL;
	return ret;
}

void *vector_pop_back(vector *v)
{
	void *ret;
	if (v->count > 0) 
	{
		v->count--;
		ret = v->data[v->count];
		v->data[v->count] = NULL;
	}
	else
	{
		ret = NULL;
	}
	return ret;
}

void vector_delete(vector *v, int index)
{
	if (index >= v->count) {
		return;
	}

	v->data[index] = NULL;

	int i, j;
	void **newarr = (void**)malloc(sizeof(void*) * v->count * 2);
	for (i = 0, j = 0; i < v->count; i++) {
		if (v->data[i] != NULL) {
			newarr[j] = v->data[i];
			j++;
		}		
	}

	free(v->data);

	v->data = newarr;
	v->count--;
}

void vector_free(vector *v)
{
	free(v->data);
}

void vector_deepFree(vector *v)
{
	int i;
	for (i = 0; i < vector_count(v); i++) {
		free(v->data[i]);
	}
	free(v->data);
}
