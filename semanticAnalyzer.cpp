#include <iostream>
#include "semanticAnalyzer.h"
#include "varType.h"
#include "util/symbolTableUtil.h"

SemanticAnalyzer::SemanticAnalyzer(ASTNode * root)
    : root (root) {
    symbolTable = new SymbolTable();
}

SymbolTable * SemanticAnalyzer::analyze() {
    
    parseNode(root);

    printSymbolTable(symbolTable);

    return symbolTable;
}

void SemanticAnalyzer::parseNode(ASTNode * node) {
    switch (node->nodeType) {
        case AST_DECLARATION:
        case AST_FUNCTION:
            {
                VarType varType;
                std::string functionName = "";
                for (auto i = node->children.begin(); i != node->children.end(); i++) {
                    ASTNode * childNode = *i;
                    // Is TypeNode
                    if (i == node->children.begin()) {
                        varType = ((TypeNode *)*i)->varType;
                    } else if(node->nodeType == AST_DECLARATION) {
                        // Is AST_VARDECLARATION 
                        IdentifierNode * idNode = (IdentifierNode *)(*i)->children[0];
                        std::string varName = idNode->name;
                        symbolTable->addVariable(varType, varName);
                    } else if (node->nodeType == AST_FUNCTION && i == node->children.begin() + 1) {
                        // Second child of AST_FUNCTION is an Identifier
                        IdentifierNode * idNode = (IdentifierNode *)(*i);
                        functionName = idNode->name;
                        symbolTable->addFunction(varType, functionName);
                    } else if (node->nodeType == AST_FUNCTION && childNode->nodeType == AST_PARAMETERLIST) {
                        // Open the scope for the body of the function
                        symbolTable->openScope(functionName);
                        parseNode(childNode);
                    } else {
                        parseNode(childNode);
                    }
                }
                break;
            }
        case AST_PARAMETERLIST:
            {
                for (auto i = node->children.begin(); i != node->children.end(); i++) {
                    ASTNode * childNode = *i; // Parameter
                    TypeNode * paramType = (TypeNode*) childNode->children[0];
                    IdentifierNode * paramIdentifier = (IdentifierNode*) childNode->children[1];

                    symbolTable->addParam(paramType->varType, paramIdentifier->name);
                }
                break;
            }
        case AST_FUNCTIONBODY:
            {
                for (auto & n : node->children) {
                    parseNode(n);
                }
                symbolTable->closeScope();
                break;
            }
        case AST_IDENTIFIER:
            {
                IdentifierNode * idNode= (IdentifierNode*) node;
                STEntry * entry = symbolTable->lookup(idNode->name);
                if(entry == nullptr) {
                    std::cout << "Identifier \"" << idNode->name << "\" is undefined." << std::endl;
                }
                break;
            }
        default:
            for (auto & n : node->children) {
                parseNode(n);
            }
    }
}
