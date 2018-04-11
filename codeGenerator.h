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
    CVT_PARAM,
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
        const std::string nl = "\n ";
        
        // MEMORY MAP
        const std::string cSP = "$0400";
        const std::string cFP = "$0401";
        const int globalStart = 1026; // $0402

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
        void generateBeginFunc(TAC tac);
        void generateCodeFromTAC(TAC tac);
        void generateAssign(TAC tac);
        void generateAddition(TAC tac);
        void generateBranchOnCondition(TAC tac, bool condition);
        void generateJump(TAC tac);
        void generatePushParam(TAC tac);
        void generatePopParams(TAC tac);
        void generateSubroutineCall(TAC tac);
        void generateEndFunc(TAC tac);

        // Accessing locals, params, and globals
        void loadLocalIntoA(int offset);
        void loadParamIntoA(int offset);
        void loadGlobalIntoA(int address);
        void loadConstIntoA(int value);

        // Storing locals, and globals
        void storeAIntoLocal(int offset);
        void storeAIntoGlobal(int address);

        void allocateGlobal(std::string name, std::string type);
        
        /*
            We implemented our own stack. These methods allow us to use it.
        */
        void initializeStack();
        void popStackToA();
        void pushAToStack();
        void allocateStackSpace(int size);
        void deallocateStackSpace(int size);

        //Helpers
        CodeVar lookup(std::string varName);
        int addGlobal(std::string globalVar); //Returns a memory address for a global var
        std::string padAddress(int num);
        bool isNumber(std::string str);
};