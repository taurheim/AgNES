#pragma once

#include <map>
#include "../token.h"
#include <string>
#include <sstream>

static std::map<TokenType, std::string> tokenToStrMap = {
    { T_EQUALS, "EQUALS" },
    { T_PLUS, "PLUS" },
    { T_MINUS, "MINUS" },
    { T_SEMI, "SEMI" },
    { T_LPAREN, "LPAREN" },
    { T_RPAREN, "RPAREN" },
    { T_LCURLY, "LCURLY" },
    { T_RCURLY, "RCURLY" },
    { T_INT, "INT" },
    { T_RETURN, "RETURN" },
    { T_IDENT, "IDENT" },
    { T_INTVALUE, "INTVALUE" },
    { T_COMMA, "COMMA" },
    { T_EOF_TOKEN, "EOF" },
    { T_CHAR, "CHAR"},
    { T_CHARVALUE, "CHARVALUE"},
    { T_LSQUARE, "LSQUARE"},
    { T_RSQUARE, "RSQUARE"},
};

static std::string tokenToStr(Token t) {
    auto search = tokenToStrMap.find(t.type);
    if (search == tokenToStrMap.end()) {
        return "TOKEN_UTIL_MISSING_TOKEN";
    } else {
        std::stringstream outputString;
        outputString << search->second;
        switch(t.type) {
            case T_INTVALUE:
                outputString << "(" << t.intVal << ")";
                break;
            case T_IDENT:
                outputString << "(" << t.stringVal << ")";
                break;
        }

        return outputString.str();
    }
}