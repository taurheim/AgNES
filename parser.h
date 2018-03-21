#pragma once
#include <list>
#include "token.h"
#include "astNode.h"
#include "util/tokenUtil.h"

class Parser {
    public:
        explicit Parser(std::list<Token> & tokens);
        void parse();
    private:
        std::list<Token> & tokens;
        std::list<Token>::iterator tokenIterator;

        ASTNode * root;

        Token t;
        void next();

        void reject();

        // Non-terminals
        ASTNode * program(); // { <function> | <declaration> }
        ASTNode * declaration();
        void * variableDeclaration();
        void * parameterList(); // <parameter> | parameterList "," <parameter>
        ASTNode * parameter(); // <typeSpecifier> <identifier>
        ASTNode * function(); // <typeSpecifier> <identifier> "(" [<parameter-list>] ")" "{" {<statement>} "}"
        void * statement(); // if, else, etc.
        void * assignment(); // <identifier> [ "[" <expression> "]" ] "=" <expression>
        ASTNode * expression(); // FROM THING (adding, subtracting, etc.)

        // Terminals
        ASTNode * type(); // <type>
        ASTNode * identifier();
        void number(); // Number token
        void binaryOperation();
        void logicalOperation();
        void compareOperation();
};
