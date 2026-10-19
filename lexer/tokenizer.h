#pragma once
#include "tokens_struct.h"
#include "../utils/arena_allocator.h"
#include "../utils/vector.h"
#include "../utils/errors.h"

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

int character_match(const lexer *lex , const char *str){
    char inp = peek(lex);
    return (inp != '\0'  && strchr(str , inp) != NULL);
}

char peek_next(const lexer *lex){
    if(character_match(lex , "\0")){
        return '\0';
    }
    return (lex->code[lex->position + 1]);
}

int is_at_end(const lexer *lex){
    return (peek(lex) == '\0');
}

void advance(lexer *lex){
    if(!is_at_end(lex)){
        if(character_match(lex , "\n")){
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
    lexer *lex = (lexer *)malloc(sizeof(lexer));
    lex->code = source;
    lex->position = 0;
    lex->line = 1;
    lex->column = 1;
    initialize_vector( &lex->Tokens , sizeof(Token));

    initialize_arena(&lex->Token_data);

    while(!is_at_end(lex)){

        //for strings
        if(character_match(lex , "\"")){
            size_t string_len = 0;
            int start_line = lex->line;
            int start_col = lex->column;
            int start_pos = lex->position;
            advance(lex);
            Vector string_data;
            initialize_vector(&string_data , 1);
            while(1){
                //if it ended
                if(is_at_end(lex)){
                    disp_error(lex->code , start_line , start_col , start_pos , 3 , "String starts here but is never closed ,(did you forget a closing quote?)");
                }
                //for escape sequences
                else if(character_match(lex , "\\")){
                    advance(lex);
                    if(is_at_end(lex)){
                        disp_error(lex->code , lex->line , lex->column - 1 , lex->position - 1 , 3, "file ends right after a backslash, the file may be cut off");
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
                                disp_error(lex->code , start_line , lex->column - i - 1 , lex->position ,3 ,  "Octal escape value = %d , is out of range, (a char can hold 0 to 255, so the max is \\377)" , sum);
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
                                disp_error(lex->code , start_line , lex->column - 1 , lex->position , 3 , "\\x must be followed by at least one hex digit (0-9, a-f) , %c is not a valid hexadecimal digit" , peek(lex));
                            }
                            while(is_hexadecimal_digit(peek(lex)) && i < 2){
                                if(leading_0){
                                    if(character_match(lex , "0")){
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
                            disp_error(lex->code , start_line , lex->column - 1 , lex->position , 3 , "unknown escape sequence \\%c" , peek(lex));
                            break;
                    }
                    push_vector(&string_data , &c);
                    string_len++;
                    advance(lex);
                }
                //string end
                else if(character_match(lex , "\"")){
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
                else if(character_match(lex , "\n")){
                    disp_error(lex->code , start_line , start_col , start_pos , 3 , "a string can't span lines, if this one was meant to continue, close it with \" or use \\n for a line break");
                }
                //normal chars
                else{
                    char c = peek(lex);
                    push_vector(&string_data , &c);
                    string_len++;
                    advance(lex);
                }
            }
            continue;
        }

        // for whitespaces
        if(character_match(lex , " \t\r\n\f\v")){
            advance(lex);
            continue;
        }

        // for single instance character 
        if(character_match(lex , "(){}[];,~?:#")){
            Token temp;
            switch (peek(lex)){
            case '(':
                temp = make_token_pre_defined_tokens(TOK_OPEN_ROUND , lex);
                push_vector(&lex->Tokens , &temp);
                break;
            case ')':
                temp = make_token_pre_defined_tokens(TOK_CLOSE_ROUND , lex);
                push_vector(&lex->Tokens , &temp);
                break;
            case '{':
                temp = make_token_pre_defined_tokens(TOK_OPEN_CURLY , lex);
                push_vector(&lex->Tokens , &temp);
                break;
            case '}':
                temp = make_token_pre_defined_tokens(TOK_CLOSE_CURLY , lex);
                push_vector(&lex->Tokens , &temp);
                break;
            case '[':
                temp = make_token_pre_defined_tokens(TOK_OPEN_SQUARE , lex);
                push_vector(&lex->Tokens , &temp);
                break;
            case ']':
                temp = make_token_pre_defined_tokens(TOK_CLOSE_SQUARE , lex);
                push_vector(&lex->Tokens , &temp);
                break;
            case ';':
                temp = make_token_pre_defined_tokens(TOK_SEMICOLON , lex);
                push_vector(&lex->Tokens , &temp);
                break;
            case ',':
                temp = make_token_pre_defined_tokens(TOK_COMMA , lex);
                push_vector(&lex->Tokens , &temp);
                break;
            case '~':
                temp = make_token_pre_defined_tokens(TOK_TILDE , lex);
                push_vector(&lex->Tokens , &temp);
                break;
            case '?':
                temp = make_token_pre_defined_tokens(TOK_QUESTION , lex);
                push_vector(&lex->Tokens , &temp);
                break;
            case ':':
                temp = make_token_pre_defined_tokens(TOK_COLON, lex);
                push_vector(&lex->Tokens , &temp);
                break;
            case '#':
                temp = make_token_pre_defined_tokens(TOK_HASH , lex);
                push_vector(&lex->Tokens , &temp);
                break;
            default:
                break;
            }
            advance(lex);
            continue;
        }

        //for single line comments
        if(character_match(lex , "/") && peek_next(lex) == '/'){
            while(peek(lex) != '\n' && !is_at_end(lex)){
                advance(lex);
            }
            continue;
        }

        //for multi line comments
        if(character_match(lex , "/")  && peek_next(lex) == '*'){
            int start_line = lex->line;
            int start_col = lex->column; 
            int start_pos = lex->position;
            advance(lex);
            advance(lex);
            while((peek(lex) != '*' || peek_next(lex) != '/') && !is_at_end(lex)){
                advance(lex);
            }
            if(is_at_end(lex)){
                disp_error(lex->code ,start_line , start_col , start_pos , 3 , "Multi line comment started but never close (did you forget a missing \"*\\\"?)" );
            }
            advance(lex);
            advance(lex);
            continue;
        }

        // for characters which are present in multiple tokens
        if(character_match(lex , "+-*/%=!<>&|^.")){
            Token temp;
            switch(peek(lex)){
                case '+':
                    if(peek_next(lex) == '+'){
                        temp = make_token_pre_defined_tokens(TOK_PLUS_PLUS , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else if(peek_next(lex) == '='){
                        temp = make_token_pre_defined_tokens(TOK_PLUS_EQUALS , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else{
                        temp = make_token_pre_defined_tokens(TOK_PLUS , lex);
                        push_vector(&lex->Tokens , &temp);
                    }
                    break;
                case '-':
                    if(peek_next(lex) == '-'){
                        temp = make_token_pre_defined_tokens(TOK_MINUS_MINUS , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else if(peek_next(lex) == '='){
                        temp = make_token_pre_defined_tokens(TOK_MINUS_EQUALS , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else if(peek_next(lex) == '>'){
                        temp = make_token_pre_defined_tokens(TOK_ARROW , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else{
                        temp = make_token_pre_defined_tokens(TOK_MINUS , lex);
                        push_vector(&lex->Tokens , &temp);
                    }
                    break;
                case '*':
                    if(peek_next(lex) == '='){
                        temp = make_token_pre_defined_tokens(TOK_STAR_EQUALS , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else{
                        temp = make_token_pre_defined_tokens(TOK_STAR , lex);
                        push_vector(&lex->Tokens , &temp);
                    }
                    break;
                case '/':
                    if(peek_next(lex) == '='){
                        temp = make_token_pre_defined_tokens(TOK_SLASH_EQUALS , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else{
                        temp = make_token_pre_defined_tokens(TOK_SLASH , lex);
                        push_vector(&lex->Tokens , &temp);
                    }
                    break;
                case '%':
                    if(peek_next(lex) == '='){
                        temp = make_token_pre_defined_tokens(TOK_PERCENT_EQUALS , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else{
                        temp = make_token_pre_defined_tokens(TOK_PERCENT , lex);
                        push_vector(&lex->Tokens , &temp);
                    }
                    break;
                case '=':
                    if(peek_next(lex) == '='){
                        temp = make_token_pre_defined_tokens(TOK_EQUALS , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else{
                        temp = make_token_pre_defined_tokens(TOK_ASSIGN , lex);
                        push_vector(&lex->Tokens , &temp);
                    }
                    break;
                case '!':
                    if(peek_next(lex) == '='){
                        temp = make_token_pre_defined_tokens(TOK_NOT_EQUALS , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else{
                        temp = make_token_pre_defined_tokens(TOK_NOT , lex);
                        push_vector(&lex->Tokens , &temp);
                    }
                    break;
                case '<':
                    if(peek_next(lex) == '='){
                        temp = make_token_pre_defined_tokens(TOK_LESS_EQUAL , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else if(peek_next(lex) == '<'){
                        if(lex->code[lex->position + 2] == '='){
                            temp = make_token_pre_defined_tokens(TOK_LSHIFT_EQUALS , lex);
                            push_vector(&lex->Tokens , &temp);
                            advance(lex);
                            advance(lex);
                        }
                        else{
                            temp = make_token_pre_defined_tokens(TOK_LEFT_SHIFT , lex);
                            push_vector(&lex->Tokens , &temp);
                            advance(lex);
                        }
                    }
                    else{
                        temp = make_token_pre_defined_tokens(TOK_LESS , lex);
                        push_vector(&lex->Tokens , &temp);
                    }
                    break;
                case '>':
                    if(peek_next(lex) == '='){
                        temp = make_token_pre_defined_tokens(TOK_GREATER_EQUAL , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else if(peek_next(lex) == '>'){
                        if(lex->code[lex->position + 2] == '='){
                            temp = make_token_pre_defined_tokens(TOK_RSHIFT_EQUALS , lex);
                            push_vector(&lex->Tokens , &temp);
                            advance(lex);
                            advance(lex);
                        }
                        else{
                            temp = make_token_pre_defined_tokens(TOK_RIGHT_SHIFT , lex);
                            push_vector(&lex->Tokens , &temp);
                            advance(lex);
                        }
                    }
                    else{
                        temp = make_token_pre_defined_tokens(TOK_GREATER , lex);
                        push_vector(&lex->Tokens , &temp);
                    }
                    break;
                case '&':
                    if(peek_next(lex) == '&'){
                        temp = make_token_pre_defined_tokens(TOK_AND_AND , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else if(peek_next(lex) == '='){
                        temp = make_token_pre_defined_tokens(TOK_AND_EQUALS , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else{
                        temp = make_token_pre_defined_tokens(TOK_AMPERSAND , lex);
                        push_vector(&lex->Tokens , &temp);
                    }
                    break;
                case '|':
                    if(peek_next(lex) == '|'){
                        temp = make_token_pre_defined_tokens(TOK_OR_OR , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else if(peek_next(lex) == '='){
                        temp = make_token_pre_defined_tokens(TOK_OR_EQUALS , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else{
                        temp = make_token_pre_defined_tokens(TOK_PIPE , lex);
                        push_vector(&lex->Tokens , &temp);
                    }
                    break;
                case '^':
                    if(peek_next(lex) == '='){
                        temp = make_token_pre_defined_tokens(TOK_XOR_EQUALS , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                    }
                    else{
                        temp = make_token_pre_defined_tokens(TOK_CARET , lex);
                        push_vector(&lex->Tokens , &temp);
                    }
                    break;
                case '.':
                    if(peek_next(lex) == '.'  && lex->code[lex->position + 2] == '.'){
                        temp = make_token_pre_defined_tokens(TOK_ELLIPSIS , lex);
                        push_vector(&lex->Tokens , &temp);
                        advance(lex);
                        advance(lex);
                    }
                    else{
                        temp = make_token_pre_defined_tokens(TOK_DOT , lex);
                        push_vector(&lex->Tokens , &temp);
                    }
                    break;
                default:
                    break;
            }
            advance(lex);
            continue;
        }
        
    }
}
