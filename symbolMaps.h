#pragma once
#include <map>
#include <string>
#include "symbol.h"

static std::map<char, Symbol> charToSymbol = {
    {'=', Symbol::EQUALS},
    {'+', Symbol::PLUS},
    {'-', Symbol::MINUS},
    {';', Symbol::SEMI},
    {'(', Symbol::LPAREN},
    {')', Symbol::RPAREN},
    {'{', Symbol::LCURLY},
    {'}', Symbol::RCURLY}
};

static std::map<std::string, Symbol> stringToSymbol = {
    {"int", Symbol::INT},
    {"return", Symbol::RETURN}
};