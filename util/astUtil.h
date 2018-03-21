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
};

static void printNode(std::string indent, bool last, ASTNode * node) {
    std::string nodeString;
    switch(node->nodeType) {
        case AST_IDENTIFIER:
            {
                IdentifierNode * identifier = static_cast<IdentifierNode*>(node);
                nodeString = "Identifier: ";
                nodeString.append(identifier->name);
                break;
            }
        case AST_TYPE:
            {
                TypeNode * type = static_cast<TypeNode*>(node);
                nodeString = "Type: ";
                nodeString.append(type->typeString);
                break;
            }
        default:
            {
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