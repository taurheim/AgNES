#pragma once
#include <map>
#include <string>
#include "token.h"

static std::map<char, TokenType> charToToken = {
    {'=', TokenType::EQUALS},
    {'+', TokenType::PLUS},
    {'-', TokenType::MINUS},
    {';', TokenType::SEMI},
    {'(', TokenType::LPAREN},
    {')', TokenType::RPAREN},
    {'{', TokenType::LCURLY},
    {'}', TokenType::RCURLY},
    {',', TokenType::COMMA},
};

static std::map<std::string, TokenType> stringToToken = {
    {"int", TokenType::INT},
    {"return", TokenType::RETURN},
    {"char", TokenType::CHAR},
};