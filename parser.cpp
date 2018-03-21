#include <iterator>
#include <iostream>
#include "parser.h"
#include "util/astUtil.h"

Parser::Parser(std::list<Token> & tokens) : tokens (tokens) {
    tokenIterator = tokens.begin();
    t = *tokenIterator;
}

void Parser::parse() {
    tree.root = program();

    printTree(tree.root);
}

void Parser::next() {
    std::advance(tokenIterator, 1);
    t = *tokenIterator;
}

ProgramNode * Parser::program() {
    /*
        {<function> | <declaration>}

        Lookahead:
        <type> <identifier>
            ";" -> declaration
            "(" -> function
    */

    // Perform lookahead
    auto decider = *std::next(tokenIterator, 2);

    ProgramNode * node = new ProgramNode();
    ASTNode * childNode;

    if(decider.type == SEMI) {
        childNode = (ASTNode*) declaration();
    } else if (decider.type == LPAREN) {
        childNode = (ASTNode*) function();
    } else {
        reject();
    }

    node->programBody.push_back(childNode);

    return node;
}

FunctionNode * Parser::function() {
    FunctionNode * node = new FunctionNode();

    node->returnType = type();
    node->functionName = identifier();

    return nullptr;
}

IdentifierNode * Parser::identifier() {
    if (t.type != TokenType::IDENT) {
        reject();
    }

    IdentifierNode * node = new IdentifierNode();
    node->name = t.stringVal;

    next();
    return node;
}

TypeNode * Parser::type() {
    if (t.type != TokenType::INT) {
        reject();
    }

    TypeNode * node = new TypeNode();
    node->type = CType::CTYPE_INT;

    next();
    return node;
}

DeclarationNode * Parser::declaration() {
    std::cout << "(declaration)";
    return nullptr;
}

void Parser::reject() {
    std::cout << "FUCK YOU" << std::endl;
}