#include <iostream>
#include "symbolTable.h"

SymbolTable::SymbolTable()
: currentScopeId (0) {
    intDescriptor = new STTypeDescriptor(VT_INT);
    charDescriptor = new STTypeDescriptor(VT_CHAR);
    boolDescriptor = new STTypeDescriptor(VT_BOOL);

    universe = new STEntry(ST_HEAD, 0);
    currentScope = universe;
};

void SymbolTable::addVariable(VarType varType, std::string varName) {
    addEntry(ST_VAR, varType, varName);
};

void SymbolTable::addFunction(VarType returnType, std::string functionName) {
    addEntry(ST_FUNCTION, returnType, functionName);
};

void SymbolTable::addParam(VarType paramType, std::string paramName) {
    addEntry(ST_PARAM, paramType, paramName);
}

void SymbolTable::addEntry(STEntryType entryType, VarType varType, std::string varName) {
    STEntry * scopeIterator = currentScope;
    while (scopeIterator->next != nullptr) {
        if (scopeIterator->next->name == varName) {
            std::cout << "Object name already exists in scope: " << varName << std::endl;
            return; 
        } else {
            scopeIterator = scopeIterator->next;
        }
    }
    STTypeDescriptor * typeDescriptor = new STTypeDescriptor(varType);
    STEntry * newEntry = new STEntry();
    newEntry->name = varName;
    newEntry->entryType = entryType;
    newEntry->scopeId = currentScopeId;
    newEntry->typeDescriptor = typeDescriptor;
    scopeIterator->next = newEntry;
};

void SymbolTable::openScope() {
    currentScopeId++;
    STEntry * oldScope = currentScope;
    currentScope = new STEntry(ST_HEAD, currentScopeId);
    
    // Fix references
    currentScope->parentScope = oldScope;
    oldScope->childScopes.push_back(currentScope);
}

void SymbolTable::closeScope() {
    currentScope = currentScope->parentScope;
}

STEntry * SymbolTable::lookup(std::string name) {
    STEntry * head = currentScope;
    STEntry * currentEntry;
    while (head != nullptr) {
         currentEntry = head->next;
         while (currentEntry != nullptr) {
             if (currentEntry->name == name) {
                 return currentEntry;
             }
             currentEntry = currentEntry->next;
         }
         if(head == universe) {
             break;
         }

        head = head->parentScope;
    }

    return nullptr;
}

void SymbolTable::print() {
    STEntry * head = currentScope;
    STEntry * currentEntry;
    std::cout << "innerScope" << std::endl;
    while (head != nullptr) {
        std::cout << "| " << std::endl;
        std::cout << "[ Head ]";
        currentEntry = head->next;
        while (currentEntry != nullptr) {
            std::cout << " -> ";
            std::cout << " [ " << currentEntry->name << " ] ";
            currentEntry = currentEntry->next;
         }
         std::cout << " -> guard " << std::endl;
         head = head->parentScope;
    }
}

STEntry * SymbolTable::getRoot() {
    return universe;
}

