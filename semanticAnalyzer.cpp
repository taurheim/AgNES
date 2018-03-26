#include "semanticAnalyzer.h"
#include "varType.h"

SemanticAnalyzer::SemanticAnalyzer(ASTNode * root)
    : root (root) {
    symbolTable = new SymbolTable();
}

void SemanticAnalyzer::analyze() {
    parseNode(root);
}

void SemanticAnalyzer::parseNode(ASTNode * node) {
    switch (node->nodeType) {
        case AST_DECLARATION:
            {
                VarType varType;
                for (auto i = node->children.begin(); i != node->children.end(); i++) {
                    // Is TypeNode
                    if (i == node->children.begin()) {
                        varType = ((TypeNode *)*i)->varType;
                    } else { // Is AST_VARDECLARATION 
                        IdentifierNode * idNode = (IdentifierNode *)(*i)->children[0];
                        std::string varName = idNode->name;
                        symbolTable->addVariable(varType, varName);
                    }

                }
                
                break;
            }
        default:
            for (auto & n : node->children) {
                parseNode(n);
            }
    }
}