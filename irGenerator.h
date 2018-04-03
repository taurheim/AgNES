#pragma once
#include "astNode.h"
#include <string>
#include <list>
#include <map>

enum IR_OPERATION {
    IR_ADD,
    IR_SUBTRACT,
    IR_ASSIGN
};

static std::map<std::string, IR_OPERATION> opStringToEnum = {
    {"+", IR_ADD},
    {"-", IR_SUBTRACT},
};

struct TAC {
    std::string adr1;
    std::string adr2;
    std::string result;
    IR_OPERATION op;
};

class IRGenerator {
    public:
        IRGenerator(ASTNode * annotatedRoot);
        void generate();
    private:
        ASTNode * root;
        int tempIdCount;
        std::list<TAC> intermediateCode;

        // Generate Three-Address-Code representation for the node and its children
        void genTAC(ASTNode * node);

        // Generate TAC for specific types of nodes
        std::string genTACExpression(ASTNode * expr);

        // Generate new temporary address
        std::string genAddress();
};
