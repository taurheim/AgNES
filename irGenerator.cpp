#include <iostream>
#include "irGenerator.h"
#include "util/irUtil.h"

IRGenerator::IRGenerator(ASTNode * annotatedRoot) {
    root = annotatedRoot;
    tempIdCount = 0;
}

void IRGenerator::generate() {
    genTAC(root);
    printIRCode(intermediateCode);
}

void IRGenerator::genTAC(ASTNode * node) {
    switch(node->nodeType) {
        case AST_EXPRESSION:
            {
                genTACExpression(node);
                break;
            }
        default:
            {
            }
    }

    for(auto child : node->children) {
        genTAC(child);
    }
}

std::string IRGenerator::genTACExpression(ASTNode * expr) {
    TAC code = TAC();
    if (expr->children.size() == 1) {
        ASTNode * onlyChild = expr->children[0];
        if(onlyChild->nodeType == AST_NUMBER) {
            // Case 1: cgen(k) where k is a constant
            code.adr1 = std::to_string(((NumberNode *)onlyChild)->value);
            code.op = IR_ASSIGN;
            code.result = genAddress();
            intermediateCode.push_back(code);
        } else if (onlyChild->nodeType == AST_IDENTIFIER) {
            // Case 2: cgen(id) where id is an identifier
            code.adr1 = ((IdentifierNode *)onlyChild)->name;
            code.op = IR_ASSIGN;
            code.result = genAddress();
            intermediateCode.push_back(code);
        }
    } else {
        ASTNode * secondChild = expr->children[1];
        //Check if binary op
        if (secondChild->nodeType == AST_OPERATOR) {
            code.adr1 = genTACExpression(expr->children[0]);
            code.adr2 = genTACExpression(expr->children[2]);
            OpNode * opNode = (OpNode *)expr->children[1];
            code.op = opStringToEnum[opNode->operation];
            code.result = genAddress();
            intermediateCode.push_back(code);
        }
        //Could be unary op, function, etc...
    }
    return code.result;
}

std::string IRGenerator::genAddress() {
    return "_t" + std::to_string(tempIdCount++);
}
