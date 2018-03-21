#pragma once
#include <list>
#include "token.h"
#include "astNode.h"
#include "util/tokenUtil.h"

struct AST {
    ProgramNode * root;
};

class Parser {
    public:
        explicit Parser(std::list<Token> & tokens);
        void parse();
    private:
        std::list<Token> & tokens;
        std::list<Token>::iterator tokenIterator;

        AST tree;

        Token t;
        void next();

        void reject();

        // Non-terminals
        ProgramNode * program(); // { <function> | <declaration> }
        DeclarationNode * declaration();
        void * variableDeclaration();
        void * parameterList(); // <parameter> | parameterList "," <parameter>
        ParameterNode * parameter(); // <typeSpecifier> <identifier>
        FunctionNode * function(); // <typeSpecifier> <identifier> "(" [<parameter-list>] ")" "{" {<statement>} "}"
        void * statement(); // if, else, etc.
        void * assignment(); // <identifier> [ "[" <expression> "]" ] "=" <expression>
        ExpressionNode * expression(); // FROM THING (adding, subtracting, etc.)

        // Terminals
        TypeNode * type(); // <type>
        IdentifierNode * identifier();
        void number(); // Number token
        void binaryOperation();
        void logicalOperation();
        void compareOperation();
};
