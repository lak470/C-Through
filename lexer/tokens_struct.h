#pragma once
#include "tokens.h"

typedef struct{
    Token_type type;
    char *content;
    int len;
    int line;
    int col;
}TOKEN;