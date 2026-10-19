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
    if(v->len == v->cap){
        size_t next_size = 2 * v->cap * v->element_size;
        if(v->cap == 0){
            next_size = DEFAULT_VECTOR_SIZE * v->element_size;
        }
        void *temp = realloc(v->data , next_size);
        if(temp == NULL){
            fprintf(stderr, "Memory allocation failure for vector\n");
            exit(2);
        }
        v->data = temp; 
        v->cap *= 2;
        if(v->cap == 0){
            v->cap = DEFAULT_VECTOR_SIZE;
        }
    }
    char *addr = (char*)v->data + (v->len * v->element_size);
    memcpy(addr , element , v->element_size);
    v->len++;
}

void *get_element(const Vector *v , size_t n){
    if(n >= v->len){
        fprintf(stderr, "Vector index out of range\n");
        exit(3);
    }
    return ((char*)v->data + (n * v->element_size));
}

void free_vector_mem(Vector *v){
    free(v->data);
    v->data = NULL;
    v->len = 0;
    v->cap = 0;
}

