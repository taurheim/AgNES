#pragma once
#include <list>
#include "token.h"
#include "astNode.h"
#include "util/tokenUtil.h"

class Parser {
    public:
        explicit Parser(std::list<Token> & tokens);
        ASTNode * parse();
    private:
        std::list<Token> & tokens;
        std::list<Token>::iterator tokenIterator;

        ASTNode * root;

        Token t;
        void next();
        void expect(const TokenType & type);
        TokenType lookahead(int amount);

        void reject(std::string const & str);

        // Non-terminals
        ASTNode * program(); // { <function> | <declaration> }
        ASTNode * declaration();
        ASTNode * variableDeclaration();
        ASTNode * parameterList(); // <parameter> | parameterList "," <parameter>
        ASTNode * parameter(); // <typeSpecifier> <identifier>
        ASTNode * function(); // <typeSpecifier> <identifier> "(" [<parameter-list>] ")" <functionBody>
        ASTNode * functionBody(); // "{" {<statement>} "}"
        ASTNode * statement(); // if, else, etc.
        ASTNode * assignment(); // <identifier> [ "[" <expression> "]" ] "=" <expression>
        ASTNode * expression(); // FROM THING (adding, subtracting, etc.)
        ASTNode * binaryExpression();

        // Terminals
        ASTNode * type(); // <type>
        ASTNode * identifier();
        ASTNode * op();
        ASTNode * number(); // Number token
};
