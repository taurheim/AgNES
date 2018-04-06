#pragma once
#include <string>
#include <iostream>
#include <map>
#include "../astNode.h"

static std::map<ASTNodeType, std::string> nodeTypeToString = {
    {AST_PROGRAM, "PROGRAM"},
    {AST_FUNCTION, "FUNCTION"},
    {AST_DECLARATION, "DECLARATION"},
    {AST_IDENTIFIER, "IDENTIFIER"},
    {AST_NUMBER, "NUMBER"},
    {AST_TYPE, "TYPE"},
    {AST_PARAMETERLIST, "PARAMETERLIST"},
    {AST_PARAMETER, "PARAMETER"},
    {AST_STATEMENT, "STATEMENT"},
    {AST_ASSIGNMENT, "ASSIGNMENT"},
    {AST_EXPRESSION, "EXPRESSION"},
    {AST_OPERATOR, "OPERATOR"},
    {AST_VARDECLARATION, "VARDECLARATION"},
    {AST_FUNCTIONBODY, "FUNCTIONBODY"},
    {AST_CHAR, "CHAR"},
};

static std::map<StatementType, std::string> statementTypeToString = {
    {STMT_WHILE, "WHILE"},
    {STMT_FOR, "FOR"},
    {STMT_IF, "IF"},
    {STMT_RETURN, "RETURN"},
    {STMT_ASSIGN, "ASSIGN"},
    {STMT_FUNCTIONCALL, "FUNCTIONCALL"},
    {STMT_BLOCK, "BLOCK"},
    {STMT_EMPTY, "EMPTY"},
    {STMT_NES_WAITFORFRAME, "NES_WAITFORFRAME"},
    {STMT_NES_SETSPRITEX, "NES_SETSPRITEX"},
    {STMT_NES_SETSPRITEY, "NES_SETSPRITEY"},
};

static void printNode(std::string indent, bool last, ASTNode * node) {
    std::string nodeString;
    switch(node->nodeType) {
        case AST_IDENTIFIER: {
            IdentifierNode * identifier = static_cast<IdentifierNode*>(node);
            nodeString = "Identifier: ";
            nodeString.append(identifier->name);
            break;
        }
        case AST_TYPE: {
            TypeNode * type = static_cast<TypeNode*>(node);
            nodeString = "Type: ";
            auto search = varTypeToString.find(type->varType);
            if (search != varTypeToString.end()) {
                nodeString.append(search->second);
            } else {
                nodeString.append("ASTUTIL::TYPE_NOT_FOUND");
            }
            break;
        }
        case AST_OPERATOR: {
            OpNode * op = static_cast<OpNode*>(node);
            nodeString = "Operator: ";
            nodeString.append(op->operation);
            break;
        }
        case AST_STATEMENT: {
            StatementNode * statement = static_cast<StatementNode*>(node);
            nodeString = "Statement (type: ";
            nodeString.append(statementTypeToString[statement->type]);
            nodeString.append(")");
            break;
        }
        case AST_NUMBER: {
            NumberNode * number = static_cast<NumberNode*>(node);
            nodeString = "Number: ";
            nodeString.append(std::to_string(number->value));
            break;
        }
        case AST_CHAR: {
            CharNode * charNode = static_cast<CharNode*>(node);
            nodeString = "Char: ";
            nodeString.append(std::string(1,charNode->value));
            break;
        }
        default: {
            auto search = nodeTypeToString.find(node->nodeType);
            if (search != nodeTypeToString.end()) {
                nodeString = search->second;
            } else {
                nodeString = "NOTFOUND";
            }
        }
    }

    std::cout << indent;

    if (last) {
        std::cout << "\\-";
        indent.append(" ");
    } else {
        std::cout << "|-";
        indent.append("| ");
    }

    std::cout << nodeString << std::endl;

    for(auto it = node->children.begin(); it != node->children.end(); ++it ){
        bool isLast = std::next(it) == node->children.end();
        printNode(indent, isLast, *it);
    }
}

static void printAST(ASTNode * root) {
    std::cout << "Parser output: \n";
    printNode("\t", true, root);
}