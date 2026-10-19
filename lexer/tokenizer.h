#pragma once
#include "tokens_struct.h"
#include "../arena_allocator.h"
#include "../Vector.h"

typedef struct{
    char *code;
    int position;
    int line;
    int column;
    Vector Tokens;
    Arena Token_data;
}lexer;

char peek(const lexer *lex){
    return (lex->code[lex->position]);
}

int character_match(const lexer *lex , char c){
    return ((peek(lex) == c) ? 1 : 0);
}

char peek_next(const lexer *lex){
    if(character_match(lex , '\0')){
        return '\0';
    }
    return (lex->code[lex->position + 1]);
}

int is_at_end(const lexer *lex){
    return character_match(lex , '\0');
}

void advance(lexer *lex){
    if(!is_at_end(lex)){
        if(character_match(lex , '\n')){
            lex->line++;
            lex->column = 1;
        }
        else{
            lex->column++;
        }
        lex->position++;
    }
}

Token make_token_arena_allocated_tokens(Token_type type, int line , int col, Arena *a, size_t len){
    Token tok;
    tok.type = type;
    tok.line = line;
    tok.col = col;
    tok.len = len;
    tok.content = (char*)arena_alloc( a , tok.len + 1);
    return tok;
}

Token make_token_pre_defined_tokens(Token_type type , lexer *lex){
    Token tok;
    tok.type = type;
    tok.line = lex->line;
    tok.col = lex->column;
    tok.content = NULL;
    tok.len = 0;
    return tok;
}

int is_octal_digit(char c){
    return (c -'0' < 8 && c-'0' >= 0);
}

int is_hexadecimal_digit(char c){
    return ((c - '0' <= 9 && c - '0' >= 0) || (c - 'a' >= 0 && c - 'f' <=0) || (c - 'A' >= 0 && c - 'F' <=0 ));
}

int hexadecimal_digit_val(char c){
    if(c - '0' <= 9 && c - '0' >= 0){
        return (c - '0');
    }
    else if(c - 'a' >= 0 && c - 'f' <=0){
        return (c - 'a') + 10;
    }
    else{
        return (c - 'A') + 10;
    }
}

lexer *tokenizer(char *source){
    lexer *lex;
    lex->code = source;
    lex->position = 0;
    lex->line = 1;
    lex->column = 1;
    initialize_vector( &lex->Tokens , sizeof(Token));

    initialize_arena(&lex->Token_data);

    while(!is_at_end(lex)){

        //for strings
        if(character_match(lex , '"')){
            size_t string_len = 0;
            int start_line = lex->line;
            int start_col = lex->column;
            advance(lex);
            Vector string_data;
            initialize_vector(&string_data , 1);
            while(1){
                //if it ended
                if(is_at_end(lex)){
                    fprintf(stderr , "String Initialized but not Terminated\n");
                    exit(4);
                }
                //for escape sequences
                else if(character_match(lex , '\\')){
                    advance(lex);
                    if(is_at_end(lex)){
                        fprintf(stderr , "String Initialized but not Terminated\n");
                        exit(4);
                    }
                    unsigned char c;
                    switch (peek(lex)){
                        case 'n':
                            c ='\n';
                            break;
                        case 't':
                            c = '\t';
                            break;
                        case 'r':
                            c = '\r';
                            break;
                        case '\\':
                            c = '\\';
                            break;
                        case '"':
                            c = '"';
                            break;
                        case '\'':
                            c = '\'';
                            break;
                        case '?':
                            c = '\?';
                            break;
                        case 'a':
                            c = '\a';
                            break;
                        case 'b':
                            c = '\b';
                            break;
                        case 'f':
                            c = '\f';
                            break;
                        case 'v':
                            c = '\v';
                            break;
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        {
                            int sum = 0;
                            int i = 0;
                            while(i < 3 && is_octal_digit(peek(lex))){
                                sum *= 8;
                                sum += peek(lex) - '0';
                                if(is_octal_digit(peek_next(lex)) && i < 2){
                                    
                                    advance(lex);
                                }
                                else{
                                    break;
                                }
                                i++;
                            }
                            if(sum <= 255){
                                c = sum;
                            }
                            else{
                                fprintf(stderr , "Byte %d isnt assigned to any character" , sum);
                                exit(5);
                            }
                            break;
                        }
                        
                        case 'x':
                        {
                            int sum = 0;
                            int i = 0;
                            advance(lex);
                            int leading_0 = 1;
                            if(!is_hexadecimal_digit(peek(lex))){
                                fprintf(stderr , "Hexadecimal number initialized , but is empty");
                                exit(7);
                            }
                            while(is_hexadecimal_digit(peek(lex)) && i < 2){
                                if(leading_0){
                                    if(character_match(lex , '0')){
                                        if(is_hexadecimal_digit(peek_next(lex))){
                                            advance(lex);
                                        }
                                        else{
                                            break;
                                        }
                                        continue;
                                    }
                                    else{
                                        leading_0 = 0;
                                        continue;
                                    }
                                }
                                else{
                                    sum *= 16;
                                    sum += hexadecimal_digit_val(peek(lex));
                                    if(is_hexadecimal_digit(peek_next(lex)) && i < 1){
                                        advance(lex);
                                    }
                                    else{
                                        break;
                                    }
                                    i++;
                                }
                            }
                            c = sum;
                            break;
                        }
                        default:
                            fprintf(stderr , "Invalid character after backslash\n");
                            exit(1);
                            break;
                    }
                    push_vector(&string_data , &c);
                    string_len++;
                    advance(lex);
                }
                //string end
                else if(character_match(lex , '"')){
                    Token string_token = make_token_arena_allocated_tokens(TOK_STRING , start_line , start_col , &lex->Token_data , string_len);
                    if(string_len){
                        memcpy(string_token.content , string_data.data , string_len);
                    }
                    string_token.content[string_len] = '\0';
                    push_vector(&lex->Tokens , &string_token);  
                    free_vector_mem(&string_data);
                    advance(lex);
                    break;
                }
                // line changed mid string
                else if(character_match(lex , '\n')){
                    fprintf(stderr , "String initiated but not Terminated\n");
                    exit(5);
                }
                //normal chars
                else{
                    char c = peek(lex);
                    push_vector(&string_data , &c);
                    string_len++;
                    advance(lex);
                }
            }
        }

    }
}