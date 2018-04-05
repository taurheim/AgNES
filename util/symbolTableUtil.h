#include <map>
#include <iostream>
#include "../symbolTable.h"
#include "../varType.h"

static std::map<STEntryType, std::string> stEntryTypeToString = {
    {ST_HEAD, "HEAD"},
    {ST_VAR, "VAR"},
    {ST_PARAM, "PARAM"},
    {ST_CONST, "CONST"},
    {ST_TYPE, "TYPE"},
    {ST_FUNCTION, "FUNCTION"},
};

static void printEntry(std::string indent, bool last, STEntry * entry) {
    std::string nodeString = stEntryTypeToString[entry->entryType];

    std::cout << indent;

    if (last) {
        std::cout << "\\-";
        indent.append(" ");
    } else {
        std::cout << "|-";
        indent.append("| ");
    }

    std::cout << nodeString << ":" << entry->scopeId << " " << entry->name;

    // Print all variables at this scope
    STEntry * currentEntry = entry->next;

    while(currentEntry != nullptr) {
        std::cout << " --> (" << stEntryTypeToString[currentEntry->entryType];

        if (currentEntry->typeDescriptor != nullptr) {
            STTypeDescriptor * td = currentEntry->typeDescriptor;
            
            std::cout << ":" << varTypeToString[td->type];
        }
        
        std::cout << " " << currentEntry->name << ")";
        currentEntry = currentEntry->next;
    }

    std::cout << std::endl;

    // Go through all children scopes
    for(auto it = entry->childScopes.begin(); it != entry->childScopes.end(); ++it ){
        bool isLast = std::next(it) == entry->childScopes.end();
        printEntry(indent, isLast, *it);
    }
}

static void printSymbolTable(SymbolTable * st) {
    std::cout << "Symbol Table: \n";
    printEntry("\t", true, st->getRoot());
}