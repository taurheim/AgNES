#pragma once
#include <string>

enum TokenType {
    T_EQUALS,
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
};

struct Token {
    TokenType type;
    std::string stringVal;
    int intVal;
    char charVal;
};
