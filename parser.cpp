#include <iterator>
#include <iostream>
#include "parser.h"
#include "util/astUtil.h"

Parser::Parser(std::list<Token> & tokens) : tokens (tokens) {
    tokenIterator = tokens.begin();
    t = *tokenIterator;
}

void Parser::parse() {
    root = program();

    printAST(root);
}

void Parser::next() {
    std::advance(tokenIterator, 1);
    t = *tokenIterator;
}

ASTNode * Parser::program() {
    /*
        {<function> | <declaration>}

        Lookahead:
        <type> <identifier>
            ";" -> declaration
            "(" -> function
    */

    // Perform lookahead
    auto decider = *std::next(tokenIterator, 2);

    ASTNode * node = new ASTNode();
    ASTNode * childNode;

    if(decider.type == SEMI) {
        childNode = declaration();
    } else if (decider.type == LPAREN) {
        childNode = function();
    } else {
        reject();
    }

    node->children.push_back(childNode);

    return node;
}

ASTNode * Parser::function() {
    ASTNode * node = new ASTNode();

    node->children.push_back(type());
    node->children.push_back(identifier());

    return node;
}

ASTNode * Parser::identifier() {
    if (t.type != TokenType::IDENT) {
        reject();
    }

    IdentifierNode * node = new IdentifierNode();
    node->name = t.stringVal;

    next();
    return node;
}

ASTNode * Parser::type() {
    if (t.type != TokenType::INT) {
        reject();
    }

    TypeNode * node = new TypeNode();
    node->typeString = "int";

    next();
    return node;
}

ASTNode * Parser::declaration() {
    std::cout << "(declaration)";
    return new ASTNode();
}

void Parser::reject() {
    std::cout << "FUCK YOU" << std::endl;
}