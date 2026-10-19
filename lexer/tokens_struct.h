#pragma once
#include "tokens.h"
#include "stdlib.h"

typedef struct{
    Token_type type;
    char *content;
    size_t len;
    int line;
    int col;
}Token;