#pragma once
#include <string>

enum TokenType {
    T_ASSIGN,
    T_PLUS,
    T_MINUS,
    T_SEMI,
    T_LPAREN,
    T_RPAREN,
    T_LCURLY,
    T_RCURLY,
    T_RETURN,
    T_IDENT,
    T_COMMA,
    T_EOF_TOKEN,
    T_LSQUARE,
    T_RSQUARE,
    T_INT,
    T_INTVALUE,
    T_CHAR,
    T_CHARVALUE,
    T_WHILE,
    T_IF,
    T_ELSE,
    T_FOR,
    T_VOID,
    T_LESSTHAN,
    T_GREATERTHAN,
    T_NOTEQUAL,
    T_LESSEQUAL,
    T_GREATEREQUAL,
    T_AND,
    T_OR,
};

struct Token {
    TokenType type;
    std::string stringVal;
    int intVal;
    char charVal;

    Token(){};
    Token(TokenType type) :
    type (type) {
    }
};
