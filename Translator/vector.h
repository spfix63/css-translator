/***********************************************************
* Kurso "Transliavimo metodai" 2012/13 m.m. rudens sem.
* 4-as praktinis darbas.
* Darbà atliko: Jonas Inèius
************************************************************/

#ifndef VECTOR_H__
#define VECTOR_H__

typedef struct vector_ {
    void** data;
    int size;
    int count;
} vector;

void vector_init(vector*);
int vector_count(vector*);
void vector_add(vector*, void*);
void vector_set(vector*, int, void*);
void *vector_get(vector*, int);
void *vector_back(vector*);
void *vector_pop_back(vector*);
void vector_delete(vector*, int);
void vector_free(vector*);
void vector_deepFree(vector *v);

#endif
