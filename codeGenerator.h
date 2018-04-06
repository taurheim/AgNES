#pragma once
#include "astNode.h"
#include "irGenerator.h"
#include "symbolTable.h"
#include <iostream>
#include <string>
#include <list>
#include <map>
#include <sstream>

enum CodeVarType {
    CVT_LOCAL,
    CVT_GLOBAL,
    CVT_CONST,
};

struct CodeVar {
    CodeVarType type;
    int value;
};

class CodeGenerator {
    public:
        CodeGenerator(std::list<TAC> intermediateCode, SymbolTable * symbolTable);
        std::string generate();
    private:
        const std::string nl = "\n";
        std::list<TAC> intermediateCode;
        std::ostringstream code;
        std::map<std::string, int> globalToAddressMap;
        std::map<std::string, int> localToOffsetMap;
        std::map<std::string, int> currentParamMap;
        SymbolTable * symbolTable;

        bool jumpedToMain; // Ensure we start running at main and not at functions defined above main
        int currentGlobalOffset;
        int currentLocalOffset;

        //Generate code!!!
        void generateCodeFromTAC(TAC tac);
        void generateFramePointer();
        void generateAssign(TAC tac);
        void generateAddition(TAC tac);
        void generateBranchOnCondition(TAC tac, bool condition);
        void generateJump(TAC tac);
        void generatePushParam(TAC tac);
        void generatePopParams(TAC tac);
        void generateSubroutineCall(TAC tac);
        void beginFunc(std::string funcName);
        void allocateGlobal(std::string name, std::string type);
        void allocateStackSpace(int size);
        


        //Helpers
        CodeVar lookup(std::string varName);
        int addGlobal(std::string globalVar); //Returns a memory address for a global var
        std::string padAddress(int num);
        bool isNumber(std::string str);
};

// enum OpCode {
//     OP_TSX, //Transfer SP to X
//     OP_STX, //Store X in _
//     OP_LDA, //Load A with _
//     OP_LDY, //Load Y with _
//     OP_STA, //Store A in _
//     OP_CMP, //Compare A with _
//     OP_BEQ, //Branch if equal to _
//     OP_ADC, //Add A to _ with carry
//     OP_JMP, //Jump to _
//     OP_JSR, //Jump to subroutine _ (saves address and stuff)
//     OP_RTS, //Return from subroutine
// };

// enum Mode {
//     MODE_IMPLIED,
//     MODE_IMMEDIATE,
//     MODE_ABSOLUTE,
//     MODE_INDIRECT_Y,
// };

// // 6502 instruction!
// struct Instruction {
//     OpCode op;
//     int arg = 0;
//     Mode mode = MODE_IMPLIED;
// };