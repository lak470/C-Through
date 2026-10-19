#pragma once
typedef enum{
    TOK_EOF,
 
    // literals and names
    TOK_INT_LIT,
    TOK_FLOAT_LIT,
    TOK_CHAR_LIT,
    TOK_STRING,
    TOK_IDENTIFIER,
 
    // brackets
    TOK_OPEN_ROUND,
    TOK_CLOSE_ROUND,
    TOK_OPEN_CURLY,
    TOK_CLOSE_CURLY,
    TOK_OPEN_SQUARE,
    TOK_CLOSE_SQUARE,
 
    // comparison
    TOK_ASSIGN,          // =
    TOK_EQUALS,          // ==
    TOK_NOT_EQUALS,      // !=
    TOK_LESS,
    TOK_LESS_EQUAL,
    TOK_GREATER,
    TOK_GREATER_EQUAL,
 
    // logical
    TOK_AND_AND,         // &&
    TOK_OR_OR,           // ||
    TOK_BANG,            // !
 
    // symbols
    TOK_HASH,
    TOK_COLON,
    TOK_SEMICOLON,
    TOK_QUESTION,
    TOK_COMMA,
    TOK_DOT,
    TOK_ARROW,           // ->
    TOK_ELLIPSIS,        // ...
 
    // arithmetic
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_PERCENT,
    TOK_PLUS_PLUS,
    TOK_MINUS_MINUS,
 
    // bitwise
    TOK_AMPERSAND,       // &
    TOK_PIPE,            // |
    TOK_CARET,           // ^
    TOK_TILDE,           // ~
    TOK_LEFT_SHIFT,      // <<
    TOK_RIGHT_SHIFT,     // >>
 
    // compound assignment
    TOK_PLUS_EQUALS,
    TOK_MINUS_EQUALS,
    TOK_STAR_EQUALS,
    TOK_SLASH_EQUALS,
    TOK_PERCENT_EQUALS,
    TOK_AND_EQUALS,
    TOK_OR_EQUALS,
    TOK_XOR_EQUALS,
    TOK_LSHIFT_EQUALS,
    TOK_RSHIFT_EQUALS,
 
    // keywords: types
    TOK_INT,
    TOK_CHAR,
    TOK_SHORT,
    TOK_LONG,
    TOK_FLOAT,
    TOK_DOUBLE,
    TOK_VOID,
    TOK_SIGNED,
    TOK_UNSIGNED,
 
    // keywords: compound types
    TOK_STRUCT,
    TOK_UNION,
    TOK_ENUM,
    TOK_TYPEDEF,
 
    // keywords: control flow
    TOK_IF,
    TOK_ELSE,
    TOK_FOR,
    TOK_WHILE,
    TOK_DO,
    TOK_SWITCH,
    TOK_CASE,
    TOK_DEFAULT,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_GOTO,
    TOK_RETURN,
 
    // keywords: storage and qualifiers
    TOK_STATIC,
    TOK_EXTERN,
    TOK_VOLATILE,
    TOK_REGISTER,
    TOK_AUTO,
    TOK_CONST,
 
    // keywords: other
    TOK_SIZEOF,
}Token_type;