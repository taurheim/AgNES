#include <iostream>
#include "irGenerator.h"
#include "util/irUtil.h"

IRGenerator::IRGenerator(ASTNode * annotatedRoot) {
    root = annotatedRoot;
    registerCount = 0;
    labelCount = 0;
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
        case AST_STATEMENT:
            {
                genTACStatement(node);
                break;
            }
        default:
            {
                for(auto child : node->children) {
                    genTAC(child);
                }
            }
    }
}

void IRGenerator::genTACStatement(ASTNode * node) {
    StatementNode * statementNode = (StatementNode *)node;
    switch(statementNode->type) {
        case (STMT_WHILE): {
            std::string labelBefore = genLabel();
            std::string labelAfter = genLabel();
            intermediateCode.push_back({ IR_NEWLABEL, labelBefore});
            std::string condition = genTACExpression(node->children[0]);
            intermediateCode.push_back({ IR_IFFALSEGOTO, condition, labelAfter });
            genTACStatement(node->children[1]);
            intermediateCode.push_back({ IR_GOTO, labelBefore });
            intermediateCode.push_back({ IR_NEWLABEL, labelAfter });
            break;
        }
        case (STMT_BLOCK): {
            for (auto & child : statementNode->children) {
                genTACStatement(child);
            }
            break;
        }
        case (STMT_ASSIGN): {
            ASTNode * assignmentNode = node->children[0];
            IdentifierNode * assignTo = (IdentifierNode*) assignmentNode->children[0];
            std::string tempRegister = genTACExpression(assignmentNode->children[1]);
            // TODO scope
            intermediateCode.push_back({ IR_ASSIGN, assignTo->name, tempRegister});
            break;
        }
        default: {
            return;
        }
    }
}

std::string IRGenerator::genTACExpression(ASTNode * expr) {
    TAC code = TAC();
    if (expr->children.size() == 1) {
        ASTNode * onlyChild = expr->children[0];
        if(onlyChild->nodeType == AST_NUMBER) {
            // Case 1: cgen(k) where k is a constant
            code.op = IR_ASSIGN;
            code.first = genAddress();
            code.second = std::to_string(((NumberNode *)onlyChild)->value);
            intermediateCode.push_back(code);
        } else if (onlyChild->nodeType == AST_IDENTIFIER) {
            // Case 2: cgen(id) where id is an identifier
            code.op = IR_ASSIGN;
            code.first = genAddress();
            code.second = ((IdentifierNode *)onlyChild)->name;
            intermediateCode.push_back(code);
        }
    } else {
        ASTNode * secondChild = expr->children[1];
        //Check if binary op
        if (secondChild->nodeType == AST_OPERATOR) {
            code.second = genTACExpression(expr->children[0]);
            code.third = genTACExpression(expr->children[2]);
            OpNode * opNode = (OpNode *)expr->children[1];
            code.op = opStringToEnum[opNode->operation];
            code.first = genAddress();
            intermediateCode.push_back(code);
        }
        //Could be unary op, function, etc..
    }
    return code.first;
}

std::string IRGenerator::genAddress() {
    return "_t" + std::to_string(registerCount++);
}

std::string IRGenerator::genLabel() {
    // if (labelCounts.find(nodeType) == labelCounts.end()) {
    //     labelCounts[nodeType] = 0;
    // }
    // return "_" + nodeType + std::to_string(labelCounts[nodeType]++);
    return "LABEL" + std::to_string(labelCount++);
}