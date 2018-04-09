#include <iostream>
#include "irGenerator.h"
#include "util/irUtil.h"
#include "varType.h"

IRGenerator::IRGenerator(ASTNode * annotatedRoot) {
    root = annotatedRoot;
    registerCount = 0;
    labelCount = 0;
}

std::list<TAC> IRGenerator::generate() {
    // Go to main by default

    // Generate the rest of the code
    genTAC(root);
    printIRCode(intermediateCode);
    return intermediateCode;
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
        case AST_FUNCTION:
            {
                genTACFunction(node);
                break;
            }
        case AST_DECLARATION:
            {
                genTACDeclaration(node);
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

void IRGenerator::genTACDeclaration(ASTNode * declaration) {
    TypeNode * typeNode = (TypeNode *)declaration->children[0];
    auto child = declaration->children.begin() + 1;
    while (child != declaration->children.end()) {
        IdentifierNode * id = (IdentifierNode *)((*child)->children[0]);
        intermediateCode.push_back({ IR_DECLARATION, id->name, varTypeToString[typeNode->varType] });
        child++;
    }
}

void IRGenerator::genTACFunction(ASTNode * func) {
    IdentifierNode * functionName = (IdentifierNode*) func->children[1];
    intermediateCode.push_back({ IR_NEWLABEL, functionName->name});

    // TODO this doesn't support variables with different sizes
    intermediateCode.push_back({ IR_BEGINFUNC, functionName->name });
    auto beginFuncRef = std::prev(intermediateCode.end());
    int startingRegisterCount = registerCount;

    ASTNode * functionBody = func->children[3];
    for(auto & statement : functionBody->children) {
        genTACStatement(statement);
    }

    // How many locals are there
    std::string localsCount = std::to_string((registerCount - startingRegisterCount));
    intermediateCode.push_back({ IR_ENDFUNC, localsCount });
    // Now that we know how many labels are used
    (*beginFuncRef).second = localsCount;
}

void IRGenerator::genTACStatement(ASTNode * node) {
    StatementNode * statementNode = (StatementNode *)node;
    switch(statementNode->type) {
        case STMT_NES_WAITFORFRAME: {
            intermediateCode.push_back({ IR_NES_WAITFORFRAME });
            break;
        }
        case STMT_NES_SETSPRITEX: {
            IdentifierNode * globalIdentifier = (IdentifierNode*) node->children[0];
            std::string varName = globalIdentifier->name;

            intermediateCode.push_back({ IR_NES_SETSPRITEX, varName });
            break;
        }
        case STMT_NES_SETSPRITEY: {
            IdentifierNode * globalIdentifier = (IdentifierNode*) node->children[0];
            std::string varName = globalIdentifier->name;

            intermediateCode.push_back({ IR_NES_SETSPRITEY, varName });
            break;
        }
        case STMT_RETURN: {
            std::string returnValueRegister = genTACExpression(node->children[0]);
            intermediateCode.push_back({ IR_RETURN, returnValueRegister });
            break;
        }
        case STMT_FUNCTIONCALL: {
            IdentifierNode * functionIdentifier = (IdentifierNode*) node->children[0];
            std::string functionName = functionIdentifier->name;

            // Push params to the stack
            int parameterCount = 0;
            auto i = node->children.begin() + 1;
            while (i != node->children.end()) {
                ASTNode * argument = *i;
                std::string paramReg = genTACExpression(argument);
                intermediateCode.push_back({ IR_PUSHPARAM, paramReg });
                parameterCount++;
                i++;
            }

            // Call the function
            intermediateCode.push_back({ IR_CALL, functionName});

            // Pop the params off the stack
            intermediateCode.push_back({ IR_POPPARAMS, std::to_string(parameterCount)});
            break;
        }
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
        case (STMT_IF): {
            ASTNode * condition = node->children[0];
            ASTNode * ifBlock = node->children[1];
            ASTNode * elseBlock = nullptr;
            if (node->children.size() > 2) {
                // Future: handle more than just the else block
                elseBlock = node->children[2];
            }
            std::string falseLabel = genLabel();
            std::string outLabel = genLabel();
            std::string conditionRegister = genTACExpression(condition);
            intermediateCode.push_back({ IR_IFFALSEGOTO, conditionRegister, falseLabel });
            genTACStatement(ifBlock);
            intermediateCode.push_back({ IR_GOTO, outLabel });
            intermediateCode.push_back({ IR_NEWLABEL, falseLabel });
            if (elseBlock != nullptr) {
                genTACStatement(elseBlock);
            }
            intermediateCode.push_back({ IR_NEWLABEL, outLabel });
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
    return "$t" + std::to_string(registerCount++);
}

std::string IRGenerator::genLabel() {
    // if (labelCounts.find(nodeType) == labelCounts.end()) {
    //     labelCounts[nodeType] = 0;
    // }
    // return "_" + nodeType + std::to_string(labelCounts[nodeType]++);
    return "LABEL" + std::to_string(labelCount++);
}