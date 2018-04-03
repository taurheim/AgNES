#include <iterator>
#include <iostream>
#include "parser.h"
#include "util/astUtil.h"
#include "util/tokenUtil.h"

Parser::Parser(std::list<Token> & tokens) : tokens (tokens) {
    tokenIterator = tokens.begin();
    t = *tokenIterator;
}

ASTNode * Parser::parse() {
    root = program();
    printAST(root);
    return root;
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

        if(decider == LPAREN) {
            childNode = function();
        } else {
            childNode = declaration();
            expect(SEMI);
        }

        node->children.push_back(childNode);
    }

    return node;
}

ASTNode * Parser::function() {
    /*
        <type> <identifier> <parameterList> <functionBody>
    */
    ASTNode * node = new ASTNode(AST_FUNCTION);

    node->children.push_back(type());
    node->children.push_back(identifier());
    node->children.push_back(parameterList());
    node->children.push_back(functionBody());

    return node;
}

ASTNode * Parser::functionBody() {
    ASTNode * node = new ASTNode(AST_FUNCTIONBODY);
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
        "(" [<parameter> {"," <parameter> }] ")"
    */
    ASTNode * node = new ASTNode(AST_PARAMETERLIST);

    expect(LPAREN);

    if(t.type != RPAREN) {
        // We have one or more parameters
        node->children.push_back(parameter());

        // Consume the rest of the params
        while(t.type == COMMA) {
            expect(COMMA);
            node->children.push_back(parameter());
        }
    }

    
    expect(RPAREN);

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
    node->varType = VT_INT;

    next();
    return node;
}

ASTNode * Parser::declaration() {
    /*
        TODO full coverage
        <type> <variableDeclaration> { "," variableDeclaration }
    */
    ASTNode * node = new ASTNode(AST_DECLARATION);
    node->children.push_back(type());
    node->children.push_back(variableDeclaration());

    while (t.type == COMMA) {
        next();
        node->children.push_back(variableDeclaration());
    }
    return node;
}

ASTNode * Parser::variableDeclaration() {
    /*
        <identifier> [ "[" <number> "]" ]
    */
    ASTNode * node = new ASTNode(AST_VARDECLARATION);

    node->children.push_back(identifier());
    
    // TODO array

    return node;
}

void Parser::reject(std::string const & str) {
    std::cout << "[FUCK YOU] " << str << " { Got: " << tokenToStr(t) << " }" << std::endl;

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
        reject(err);
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
    expect(SEMI);

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
        | "(" <expression> ")"
        | <number>
        | <char>
        | <string>
        | <identifier> "(" [ <expression> { "," <expression> }] ")" 
        | <identifier>  "[" <expression> "]"
        | <identifier>
        | <expression> <binaryOperation> <expression>
    */
    ASTNode * node = new ASTNode(AST_EXPRESSION);

    switch(t.type) {
        case NUM: {
            ASTNode * numberNode = number();
            if (t.type == PLUS || t.type == MINUS) {
                ASTNode * expressionNode = new ASTNode(AST_EXPRESSION);
                expressionNode->children.push_back(numberNode);
                node->children.push_back(expressionNode);
            } else {
                node->children.push_back(numberNode);
            }
            break;
        }
        case IDENT: {
            ASTNode * identNode = identifier();
            if (t.type == PLUS || t.type == MINUS) {
                ASTNode * expressionNode = new ASTNode(AST_EXPRESSION);
                expressionNode->children.push_back(identNode);
                node->children.push_back(expressionNode);
            } else {
                node->children.push_back(identNode);
            }
            break;
        }
        case LPAREN: {
            expect(LPAREN);
            node->children.push_back(expression());
            expect(RPAREN);
            break;
        }
        default: {
            reject("Expression expected IDENT or NUM");
        }
    }

    if(t.type == PLUS || t.type == MINUS) {
        node->children.push_back(op());
        node->children.push_back(expression());
    } else if (t.type == LPAREN) {
        // Function call
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
