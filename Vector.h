#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_VECTOR_SIZE 8

typedef struct{
    void *data;
    size_t len;   
    size_t cap;
    size_t element_size; 
}Vector;

void initialize_vector(Vector *v, size_t elem_size){
    v->element_size = elem_size;
    v->cap = 0;
    v->len = 0;
    v->data = NULL;
}

void push_vector(Vector *v, const void *element){
    if(v->cap == 0){
        v->data = malloc(DEFAULT_VECTOR_SIZE * v->element_size);
        if(v->data == NULL){
            fprintf(stderr, "Memory allocation failure for vector\n");
            exit(2);
        }
        v->cap = DEFAULT_VECTOR_SIZE;
    }
    else if(v->len == v->cap){
        v->data = realloc(v->data , 2 * v->cap);
        if(v->data == NULL){
            fprintf(stderr, "Memory allocation failure for vector\n");
            exit(2);
        } 
        v->cap *= 2;
    }
    char *addr = (char*)v->data + (v->len * v->element_size);
    memcpy(addr , element , v->element_size);
    v->len++;
}
