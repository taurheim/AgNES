#include <iterator>
#include <iostream>
#include "parser.h"
#include "util/astUtil.h"
#include "util/tokenUtil.h"

Parser::Parser(std::list<Token> & tokens) : tokens (tokens) {
    tokenIterator = tokens.begin();
    t = *tokenIterator;
}

void Parser::parse() {
    root = program();

    printAST(root);
}

void Parser::next() {
    if (t.type == EOF_TOKEN) {
        reject("Unexpected EOF");
        return;
    }
    std::advance(tokenIterator, 1);
    t = *tokenIterator;

    std::cout << tokenToStr(t) << std::endl;
}

ASTNode * Parser::program() {
    /*
        {<function> | <declaration>}

        Lookahead:
        <type> <identifier>
            ";" -> declaration
            "(" -> function
    */

    ASTNode * node = new ASTNode(AST_PROGRAM);

    while (t.type != EOF_TOKEN) {
        TokenType decider = lookahead(2);

        ASTNode * childNode;

        if(decider == SEMI) {
            childNode = declaration();
            expect(SEMI);
        } else if (decider == LPAREN) {
            childNode = function();
        } else {
            reject("program(): Expected LPAREN or SEMI");
        }

        node->children.push_back(childNode);
    }

    return node;
}

ASTNode * Parser::function() {
    /*
        <type> <identifier> "(" [<parameterList>] ")" "{" {<statement>} "}"
    */
    ASTNode * node = new ASTNode(AST_FUNCTION);

    node->children.push_back(type());
    node->children.push_back(identifier());

    expect(LPAREN);

    // Check for params
    if (t.type != RPAREN) {
        node->children.push_back(parameterList());
    }

    expect(RPAREN);
    expect(LCURLY);

    // 0 or more statements
    while (t.type != RCURLY) {
        node->children.push_back(statement());
    }
    
    expect(RCURLY);

    return node;
}

ASTNode * Parser::parameterList() {
    /*
        <parameter> {"," <parameter> }
    */
    ASTNode * node = new ASTNode(AST_PARAMETERLIST);

    node->children.push_back(parameter());

    while (t.type == COMMA) {
        node->children.push_back(parameter());
    }

    return node;
}

ASTNode * Parser::parameter() {
    /*
        <typeSpecifier> <identifier>
    */
    ASTNode * node = new ASTNode(AST_PARAMETER);

    node->children.push_back(type());
    node->children.push_back(identifier());

    return node;
}

ASTNode * Parser::identifier() {
    /*
        Terminal
    */
    if (t.type != TokenType::IDENT) {
        reject("Expected identifier");
    }

    IdentifierNode * node = new IdentifierNode();
    node->name = t.stringVal;

    next();
    return node;
}

ASTNode * Parser::type() {
    /*
        Terminal
    */
    if (t.type != TokenType::INT) {
        reject("Type not recognized");
    }

    TypeNode * node = new TypeNode();
    node->typeString = "int";

    next();
    return node;
}

ASTNode * Parser::declaration() {
    /*
        TODO full coverage
        <type> <identifier>
    */
    ASTNode * node = new ASTNode(AST_DECLARATION);
    node->children.push_back(type());
    node->children.push_back(identifier());
    return node;
}

void Parser::reject(std::string const & str) {
    std::cout << "FUCK YOU: " << str << " -- " << tokenToStr(t) << std::endl;
    exit(1);
}

void Parser::expect(const TokenType & type) {
    if (t.type != type) {
        std::string err("Expected: ");
        std::string nodeString;
        auto search = tokenToStrMap.find(type);
        if (search != tokenToStrMap.end()) {
            nodeString = search->second;
        } else {
            nodeString = "NOTFOUND";
        }
        err.append(nodeString);
        reject("Expected: ");
    } else {
        // Consume
        next();
    }
}

ASTNode * Parser::statement() {
    /*
        "if" "(" <expression> ")" <statement> [ "else" <statement> ]
        | "while" "(" <expression> ")" <statement>
        | "for" "(" <assignment> ")" ";" <expression> ";" <assignment> ")" <statement>
        | "return" [<expression>] ";"
        | <assignment> ";"
        | <identifier> "(" [ <expression> {"," <expression> } ] ")" ";"
        | "{" {<statement>} "}"
        | ";"
    */
    StatementNode * node = new StatementNode();

    switch(t.type) {
        case RETURN: {
            node->type = "return";
            next();
            if (t.type != SEMI) {
                node->children.push_back(expression());
            }
            expect(SEMI);
            break;
        }
        case SEMI: {
            node->type = "empty";
            next();
            break;
        }
        case IDENT:
        {
            // Can be either an assignment or a function call
            // Check second character - if it's a "(" then it's a function call
            TokenType decider = lookahead(1);
            if (decider == LPAREN) {
                // Function call
                node->type = "functioncall";
                node->children.push_back(identifier());
                expect(LPAREN);

                // First arg
                if(t.type != RPAREN) {
                    node->children.push_back(expression());
                }

                // 2.. args
                while (t.type != RPAREN) {
                    expect(COMMA);
                    node->children.push_back(expression());
                }

                expect(RPAREN);
                expect(SEMI);
            } else {
                // Assignment
                node->type = "assignment";
                node->children.push_back(assignment());
            }
            break;
        }
        default:
        {
            reject("statement not recognized.");
        }
    }

    return node;
}

ASTNode * Parser::assignment() {
    /*
        <identifier> ["[" <expression> "]"] "=" <expression>
    */
    ASTNode * node = new ASTNode(AST_ASSIGNMENT);

    node->children.push_back(identifier());

    // TODO handle array

    expect(EQUALS);
    node->children.push_back(expression());

    return node;
}

TokenType Parser::lookahead(int amount) {
    Token decider = *std::next(tokenIterator, amount);
    return decider.type;
}

ASTNode * Parser::expression() {
    /*
        "-" <expression>
        | "!" <expression>
        | <identifier> [ "(" [ <expression> { "," <expression> }] ")" | "[" <expression> "]"]
        | "(" <expression> ")"
        | <number>
        | <char>
        | <string>
        | <expression> <binaryOperation> <expression>
    */
    ASTNode * node = new ASTNode(AST_EXPRESSION);

    switch(t.type) {
        case NUM: {
            node->children.push_back(number());
            break;
        }
        default: {
            node->children.push_back(identifier());
            node->children.push_back(op());
            node->children.push_back(expression());
            break;
        }
    }

    return node;
}

ASTNode * Parser::op() {
    /*
        Terminal
        "+" | "-" etc.
    */
    OpNode * node = new OpNode();

    switch(t.type) {
        case PLUS: {
            node->operation = "+";
            break;
        }
        default: {
            reject("Operation not supported");
        }
    }

    next();
    return node;
}

ASTNode * Parser::number() {
    /*
        Terminal
    */
    NumberNode * node = new NumberNode();
    node->value = t.intVal;

    expect(NUM);
    return node;
}
