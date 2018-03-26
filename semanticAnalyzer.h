#pragma once
#include "astNode.h"
#include "symbolTable.h"

class SemanticAnalyzer {

    public:
        SemanticAnalyzer(ASTNode * root);
        void analyze();
    
    private:
        ASTNode * root;
        SymbolTable * symbolTable;

        void parseNode(ASTNode * node);
};