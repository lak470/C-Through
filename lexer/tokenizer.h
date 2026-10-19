#pragma once
#include "tokens_struct.h"
#include "../arena allocator.h"
#include "../Vector.h"

typedef struct{
    int position;
    int line;
    int column;
    Vector Tokens;
}lexer;