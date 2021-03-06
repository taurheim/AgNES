#pragma once
#include <string>
#include <list>
#include "varType.h"
#include "astNode.h"

enum STEntryType {
    ST_HEAD,
    ST_VAR,
    ST_PARAM,
    ST_CONST,
    ST_TYPE,
    ST_FUNCTION,
};

struct STTypeDescriptor {
    VarType type;
    VarType baseType; //for arrays
    int length; //for arrays

    STTypeDescriptor(VarType type, VarType baseType = VT_NONE, int length = 0)
    : type (type),
    baseType (baseType),
    length (length) {
    }
};

struct STEntry {
    //mandatory
    STEntryType entryType;
    int scopeId;
    //optionals
    STTypeDescriptor * typeDescriptor; // Set if entryType in (VAR, PARAM, CONST, TYPE, FUNCTION)
    STEntry * next = nullptr;
    STEntry * parentScope; // scope directly outside this scope, for ST_HEAD only (aka "desc")
    std::list<STEntry*> childScopes; // scopes directly within this scope, for ST_HEAD only
    std::string name;
    // int intVal;
    // char charVal;
    // bool boolVal;
    STEntry() {};
    STEntry(STEntryType entryType, int scopeId)
    : entryType (entryType),
    scopeId (scopeId) {
    };
}; 

class SymbolTable {

    private:
        void addEntry(STEntryType entryType, VarType varType, std::string varName);

        STEntry * universe;
        STEntry * currentScope;
        int currentScopeId;

        STTypeDescriptor * intDescriptor;
        STTypeDescriptor * charDescriptor;
        STTypeDescriptor * boolDescriptor;

    public:
        SymbolTable();
        void openScope(std::string scopeName);
        void closeScope();
        void print();
        void addVariable(VarType type, std::string varName);
        void addFunction(VarType returnType, std::string functionName);
        void addParam(VarType paramType, std::string paramName);
        STEntry * lookup(std::string name);
        STEntry * lookupScope(std::string functionName);
        STEntry * getRoot();
};