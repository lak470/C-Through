#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define KB 1024
#define block_size_default (64*KB)

typedef struct Block{
    char *data;
    size_t used;
    size_t cap;
    struct Block *next;
}Block;

typedef struct{
    Block *current_block;
}Arena;

Block *block_initialize(size_t size, size_t used, Block *next){
    Block *b = (Block *)malloc(sizeof(Block));
    if(b == NULL){
        fprintf(stderr, "Memory allocation Failure\n");
        exit(1);
    }
    b->data = (char *)malloc(size);
    if(b->data == NULL){
        fprintf(stderr, "Memory allocation Failure\n");
        exit(1);
    }
    b->cap = size;
    b->used = used;
    b->next = next;
    return b;
}

void initialize_arena(Arena *a){
    a->current_block = block_initialize(block_size_default , 0 , NULL);
}

void *arena_alloc(Arena *a, size_t n){

    if( n > a->current_block->cap - a->current_block->used){
        size_t size = (n > block_size_default)? n : block_size_default;
        Block *temp = block_initialize(size , n , NULL);
        if(n >= block_size_default){
            temp->next = a->current_block->next;
            a->current_block->next = temp;
            return a->current_block->next->data;
        }
        temp->next = a->current_block;
        a->current_block = temp;
        return a->current_block->data;
    }

    void *ptr = a->current_block->data + a->current_block->used;
    a->current_block->used += n;
    return ptr;
}

void free_blocks(Block *b){
    if(b != NULL){
        free_blocks(b->next);
        free(b->data);
        free(b);
    }
}

void free_arena(Arena *a){
    free_blocks(a->current_block);
    a->current_block = NULL;
}

