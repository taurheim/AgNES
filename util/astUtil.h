#pragma once
#include <string>
#include <iostream>
#include "../astNode.h"

template<typename T>
void Print(int indent, T * node) {
    std::cout << "Not Implemented" << std::endl;
}

template<>
void Print<ProgramNode>(int indent, ProgramNode * node) {
    std::cout << "Program" << std::endl;
    for(auto const & astNode : node->programBody) {
        FunctionNode * node = static_cast<FunctionNode*>(astNode);
        Print(indent + 1, node);
    }
}

template<>
void Print<FunctionNode>(int indent, FunctionNode * node) {
    std::cout << "Function" << std::endl;
    Print(indent+1, node->functionName);
    Print(indent+1, node->returnType);

    for (auto const & param : node->parameters) {
        Print(indent+1, param);
    }

    for (auto const & statement : node->statements) {
        Print(indent+1, statement);
    }
}

static void PrintLine(int indent, bool last, std::string & str) {
    for(int i = 0; i<indent; i++) {
        std::cout << " ";
    }
}

static void printTree(ProgramNode * program) {
    Print(0, program);
}