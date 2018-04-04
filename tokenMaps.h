#pragma once
#include <map>
#include <string>
#include "token.h"

static std::map<char, TokenType> charToToken = {
    {'=', TokenType::T_ASSIGN},
    {'+', TokenType::T_PLUS},
    {'-', TokenType::T_MINUS},
    {';', TokenType::T_SEMI},
    {'(', TokenType::T_LPAREN},
    {')', TokenType::T_RPAREN},
    {'{', TokenType::T_LCURLY},
    {'}', TokenType::T_RCURLY},
    {',', TokenType::T_COMMA},
    {'<', TokenType::T_LESSTHAN},
    {'>', TokenType::T_GREATERTHAN},
};

static std::map<std::string, TokenType> stringToToken = {
    {"int", TokenType::T_INT},
    {"return", TokenType::T_RETURN},
    {"char", TokenType::T_CHAR},
    {"while", TokenType::T_WHILE},
    {"if", TokenType::T_IF},
    {"else", TokenType::T_ELSE},
    {"void", TokenType::T_VOID}

};