#include <iostream>
#include "symbolTable.h"

SymbolTable::SymbolTable()
: currentLevel (0) {
    intDescriptor = new STTypeDescriptor(VT_INT);
    charDescriptor = new STTypeDescriptor(VT_CHAR);
    boolDescriptor = new STTypeDescriptor(VT_BOOL);

    universe = new STEntry(ST_HEAD, 0);
    innerScope = universe;
    currentLevel = 0;
};

void SymbolTable::addVariable(VarType varType, std::string varName) {

    STEntry * scopeIterator = innerScope;
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
    newEntry->entryType = ST_VAR;
    newEntry->level = currentLevel;
    newEntry->typeDescriptor = typeDescriptor;
    scopeIterator->next = newEntry;
};

void SymbolTable::openScope() {
    currentLevel++;
    STEntry * oldScope = innerScope;
    innerScope = new STEntry(ST_HEAD, currentLevel);
    innerScope->outerScope = oldScope;
}

void SymbolTable::closeScope() {
    innerScope = innerScope->outerScope;
}

STEntry * SymbolTable::lookup(std::string name) {
    STEntry * head = innerScope;
    STEntry * currentEntry;
    while (head != nullptr) {
         currentEntry = head->next;
         while (currentEntry != nullptr) {
             if (currentEntry->name == name) {
                 return currentEntry;
             }
             currentEntry = currentEntry->next;
         }
         head = head->outerScope;
    }
}

void SymbolTable::print() {
    STEntry * head = innerScope;
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
         head = head->outerScope;
    }
}

