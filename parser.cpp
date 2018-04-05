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
    std::cout << "Parsing..." << std::endl;
    root = program();
    std::cout << std::endl;
    printAST(root);
    return root;
}

void Parser::next() {
    if (t.type == T_EOF_TOKEN) {
        reject("Unexpected EOF");
        return;
    }
    std::advance(tokenIterator, 1);
    t = *tokenIterator;

    std::cout << tokenToStr(t) << " ";
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

    while (t.type != T_EOF_TOKEN) {
        TokenType decider = lookahead(2);

        ASTNode * childNode;

        if(decider == T_LPAREN) {
            childNode = function();
        } else {
            childNode = declaration();
            expect(T_SEMI);
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
    /*
        "{" { <statement> } "}
    */
    ASTNode * node = new ASTNode(AST_FUNCTIONBODY);
    expect(T_LCURLY);

    // 0 or more statements
    while (t.type != T_RCURLY) {
        node->children.push_back(statement());
    }
    
    expect(T_RCURLY);

    return node;
}

ASTNode * Parser::parameterList() {
    /*
        "(" [<parameter> {"," <parameter> }] ")"
    */
    ASTNode * node = new ASTNode(AST_PARAMETERLIST);

    expect(T_LPAREN);

    if(t.type != T_RPAREN) {
        // We have one or more parameters
        node->children.push_back(parameter());

        // Consume the rest of the params
        while(t.type == T_COMMA) {
            expect(T_COMMA);
            node->children.push_back(parameter());
        }
    }

    
    expect(T_RPAREN);

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
    if (t.type != TokenType::T_IDENT) {
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
    TypeNode * node = new TypeNode();
    switch(t.type) {
        case T_INT: {
            node->varType = VT_INT;
            break;
        }
        case T_CHAR: {
            node->varType = VT_CHAR;
            break;
        }
        default: {
            reject("Type not recognized");
        }
    }

    next();
    return node;
}

ASTNode * Parser::declaration() {
    /*
        This could be extended to include forward declarations of functions. 
        For now, we only support forward declarations of variables.
        <type> <variableDeclaration> { "," variableDeclaration }
    */
    ASTNode * node = new ASTNode(AST_DECLARATION);
    node->children.push_back(type());
    node->children.push_back(variableDeclaration());

    while (t.type == T_COMMA) {
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

    // Check if its an array
    if(t.type == T_LSQUARE) {
        expect(T_LSQUARE);
        node->children.push_back(number());
        expect(T_RSQUARE);
    }

    return node;
}

void Parser::reject(std::string const & str) {
    std::cout << "\n[FUCK YOU] " << str << " { Got: " << tokenToStr(t) << " }" << std::endl;

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
        case T_WHILE: {
            node->type = STMT_WHILE;
            next();
            expect(T_LPAREN);
            node->children.push_back(expression());
            expect(T_RPAREN);
            node->children.push_back(statement());
            break;
        }
        case T_IF: {
            node->type = STMT_IF;
            next();
            expect(T_LPAREN);
            node->children.push_back(expression());
            expect(T_RPAREN);
            node->children.push_back(statement());
            if (t.type == T_ELSE) {
                next();
                node->children.push_back(statement());
            }
            break;
        }
        case T_LCURLY: {
            node->type = STMT_BLOCK;
            next();
            while (t.type != T_RCURLY) {
                node->children.push_back(statement());
            }
            expect(T_RCURLY);
            break;
        }
        case T_RETURN: {
            node->type = STMT_RETURN;
            next();
            if (t.type != T_SEMI) {
                node->children.push_back(expression());
            }
            expect(T_SEMI);
            break;
        }
        case T_SEMI: {
            node->type = STMT_EMPTY;
            next();
            break;
        }
        case T_IDENT:
        {
            // Can be either an assignment or a function call
            // Check second character - if it's a "(" then it's a function call
            TokenType decider = lookahead(1);
            if (decider == T_LPAREN) {
                // Function call
                node->type = STMT_FUNCTIONCALL;
                node->children.push_back(identifier());
                expect(T_LPAREN);

                // First arg
                if(t.type != T_RPAREN) {
                    node->children.push_back(expression());
                }

                // 2.. args
                while (t.type != T_RPAREN) {
                    expect(T_COMMA);
                    node->children.push_back(expression());
                }

                expect(T_RPAREN);
                expect(T_SEMI);
            } else {
                // Assignment
                node->type = STMT_ASSIGN;
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
        <identifier> ["[" <arrayIndex> "]"] "=" <expression>
    */
    ASTNode * node = new ASTNode(AST_ASSIGNMENT);

    node->children.push_back(identifier());

    // Check if its an array
    if (t.type == T_LSQUARE) {
        expect(T_LSQUARE);
        node->children.push_back(arrayIndex());
        expect(T_RSQUARE);
    }

    expect(T_ASSIGN);
    node->children.push_back(expression());
    expect(T_SEMI);

    return node;
}

ASTNode * Parser::arrayIndex() {
    /*
        <expression>
    */
    ASTNode * node = new ASTNode(AST_ARRAYINDEX);

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
        | "(" <expression> ")"
        | <number>
        | <character>
        | <string>
        | <identifier> "(" [ <expression> { "," <expression> }] ")" 
        | <identifier>  "[" <expression> "]"
        | <identifier>
        | <expression> <binaryOperation> <expression>
    */
    ASTNode * node = new ASTNode(AST_EXPRESSION);

    switch(t.type) {
        case T_INTVALUE: {
            ASTNode * numberNode = number();
            if (t.type == T_PLUS || t.type == T_MINUS) {
                ASTNode * expressionNode = new ASTNode(AST_EXPRESSION);
                expressionNode->children.push_back(numberNode);
                node->children.push_back(expressionNode);
            } else {
                node->children.push_back(numberNode);
            }
            break;
        }
        case T_CHARVALUE: {
            ASTNode * charNode = character();
            node->children.push_back(charNode);
            break;
        }
        case T_IDENT: {
            ASTNode * identNode = identifier();
            if (t.type == T_PLUS || t.type == T_MINUS) {
                ASTNode * expressionNode = new ASTNode(AST_EXPRESSION);
                expressionNode->children.push_back(identNode);
                node->children.push_back(expressionNode);
            } else {
                node->children.push_back(identNode);
            }
            break;
        }
        case T_LPAREN: {
            expect(T_LPAREN);
            node->children.push_back(expression());
            expect(T_RPAREN);
            break;
        }
        default: {
            reject("Expression expected T_IDENT or T_INTVALUE");
        }
    }

    if(t.type == T_PLUS || t.type == T_MINUS) {
        node->children.push_back(op());
        node->children.push_back(expression());
    } else if (t.type == T_LPAREN) {
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
        case T_PLUS: {
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

    expect(T_INTVALUE);
    return node;
}

ASTNode * Parser::character() {
    /*
        Terminal
    */
   CharNode * node = new CharNode();
   node->value = t.charVal;
   
   expect(T_CHARVALUE);
   return node;
}
