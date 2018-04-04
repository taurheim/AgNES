#pragma once
#include "astNode.h"
#include <string>
#include <list>
#include <map>

enum IR_OPERATION {
    IR_ADD,
    IR_SUBTRACT,
    IR_ASSIGN,
    IR_NEWLABEL,
    IR_GOTO,
    IR_IFTRUEGOTO,
    IR_IFFALSEGOTO,
};

static std::map<std::string, IR_OPERATION> opStringToEnum = {
    {"+", IR_ADD},
    {"-", IR_SUBTRACT},
};

struct TAC {
    IR_OPERATION op;
    std::string first;
    std::string second; 
    std::string third;
};

class IRGenerator {
    public:
        IRGenerator(ASTNode * annotatedRoot);
        void generate();
    private:
        ASTNode * root;
        int registerCount;
        std::list<TAC> intermediateCode;
        //std::map<std::string, int> labelCounts;
        int labelCount;

        // Generate Three-Address-Code representation for the node and its children
        void genTAC(ASTNode * node);

        // Generate TAC for specific types of nodes
        std::string genTACExpression(ASTNode * expr);
        void genTACStatement(ASTNode * statement);

        // Generate new temporary address
        std::string genAddress();

        // Generate new label
        std::string genLabel();
};
