#pragma once

#include <map>
#include "../token.h"
#include <string>
#include <sstream>

static std::map<TokenType, std::string> tokenToStrMap = {
    { EQUALS, "EQUALS" },
    { PLUS, "PLUS" },
    { MINUS, "MINUS" },
    { SEMI, "SEMI" },
    { LPAREN, "LPAREN" },
    { RPAREN, "RPAREN" },
    { LCURLY, "LCURLY" },
    { RCURLY, "RCURLY" },
    { INT, "INT" },
    { RETURN, "RETURN" },
    { IDENT, "IDENT" },
    { NUM, "NUM" },
};

static std::string tokenToStr(Token t) {
    auto search = tokenToStrMap.find(t.type);
    if (search == tokenToStrMap.end()) {
        return "TOKEN_UTIL_MISSING_TOKEN";
    } else {
        std::stringstream outputString;
        outputString << search->second;
        switch(t.type) {
            case NUM:
                outputString << "(" << t.intVal << ")";
                break;
            case IDENT:
                outputString << "(" << t.stringVal << ")";
                break;
        }

        return outputString.str();
    }
}