#pragma once
#include <string>

enum TokenType {
    EQUALS,
    PLUS,
    MINUS,
    SEMI,
    LPAREN,
    RPAREN,
    LCURLY,
    RCURLY,
    INT,
    RETURN,
    IDENT,
    NUM,
    COMMA,
    EOF_TOKEN,
    CHAR,
    LSQUARE,
    RSQUARE,
};

struct Token {
    TokenType type;
    std::string stringVal;
    int intVal;
    char charVal;
};
