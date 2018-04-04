#pragma once
#include <map>
#include <string>
#include "token.h"

static std::map<char, TokenType> charToToken = {
    {'=', TokenType::T_EQUALS},
    {'+', TokenType::T_PLUS},
    {'-', TokenType::T_MINUS},
    {';', TokenType::T_SEMI},
    {'(', TokenType::T_LPAREN},
    {')', TokenType::T_RPAREN},
    {'{', TokenType::T_LCURLY},
    {'}', TokenType::T_RCURLY},
    {',', TokenType::T_COMMA},
};

static std::map<std::string, TokenType> stringToToken = {
    {"int", TokenType::T_INT},
    {"return", TokenType::T_RETURN},
    {"char", TokenType::T_CHAR},
};