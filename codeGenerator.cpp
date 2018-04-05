#include "codeGenerator.h"
#include <iomanip>
#include <sstream>

CodeGenerator::CodeGenerator(std::list<TAC> intermediateCode, SymbolTable * symbolTable) :
 intermediateCode (intermediateCode),
 symbolTable (symbolTable) {
     currentGlobalOffset = 0;
     currentLocalOffset = 4;
}

std::string CodeGenerator::generate() {
    //First save our frame-pointer
    generateFramePointer();
    for (auto & tac : intermediateCode) {
        generateCodeFromTAC(tac);
    }
    return code.str();
}

void CodeGenerator::generateCodeFromTAC(TAC tac) {
    switch (tac.op) {
        case IR_DECLARATION: {
            allocateGlobal(tac.first, "type");
            break;
        }
        case IR_NEWLABEL: {
            code << tac.first << ":" << nl;
            break;
        } 
        case IR_BEGINFUNC: {
            beginFunc(tac.first);
            allocateStackSpace(std::stoi(tac.second));
            break;
        }
        case IR_ASSIGN: {
            generateAssign(tac);
            break;
        }
        case IR_ADD: {
            generateAddition(tac);
            break;
        }
        case IR_IFTRUEGOTO: {
            generateBranchOnCondition(tac, true);
            break;
        }
        case IR_IFFALSEGOTO: {
            generateBranchOnCondition(tac, false);
            break;
        }
        case IR_GOTO: {
            generateJump(tac);
            break;
        }
        case IR_PUSHPARAM: {
            generatePushParam(tac);
        }
    }
}

void CodeGenerator::generateAssign(TAC tac) {
    code << "; assign " << tac.first <<" := " << tac.second << nl;
    CodeVar left = lookup(tac.first);
    CodeVar right = lookup(tac.second);
    if (left.type == CVT_GLOBAL) {
        if (right.type == CVT_LOCAL) {
            code << "LDY #" << right.value << nl;
            code << "LDA ($0),Y" << nl;
            code << "STA " << padAddress(left.value) << nl;
        }
        else { //const
            code << "LDA #" << right.value << nl;
            code << "STA " << padAddress(left.value) << nl;
        }
    }
    else if (left.type == CVT_LOCAL) {
        if (right.type == CVT_GLOBAL) {
            code << "LDA " << padAddress(right.value) << nl;
            code << "LDY #" << left.value << nl;
            code << "STA ($0),Y" << nl;
        }
        else { //const
            code << "LDA #" << right.value << nl;
            code << "LDY #" << left.value << nl;
            code << "STA ($0),Y" << nl;
        }
    }
}

void CodeGenerator::generateAddition(TAC tac) {
    code << "; add " << tac.first << " := " << tac.second << " + " << tac.third << nl;
    CodeVar assignTo = lookup(tac.first);
    CodeVar arg1 = lookup(tac.second);
    CodeVar arg2 = lookup(tac.third);
    code << "LDY #" << arg1.value << nl;
    code << "LDA ($0),Y" << nl;
    code << "LDY #" << arg2.value << nl;
    code << "ADC ($0),Y" << nl;
    code << "LDY #" << assignTo.value << nl;
    code << "STA ($0),Y" << nl;
}

void CodeGenerator::generateBranchOnCondition(TAC tac, bool condition) {
    code << "; branch if " << tac.first << " is " << condition << nl;
    CodeVar var = lookup(tac.first);
    code << "LDY #" << var.value << nl;
    code << "LDA ($0), Y" << nl;
    code << "CPA #" << condition << nl;
    code << "BEQ " << tac.second << nl;
}

void CodeGenerator::generateJump(TAC tac) {
    code << "; jump to " << tac.first << nl;
    code << "JMP " << tac.first << nl;
}

void CodeGenerator::generateFramePointer() {
    code << "; create frame pointer " << nl;
    code << "TSX" << nl; //Save SP to X
    code << "STX 0000" << nl; //Save SP to mem at 0000
    globalToAddressMap["$fp"] = currentGlobalOffset;
    currentGlobalOffset += 4;
}

void CodeGenerator::beginFunc(std::string funcName) {
    currentLocalOffset = 4;
    currentParamMap.clear();
    int paramCount = 1;
    code << "; beginfunc " << funcName << nl;
    STEntry * functionScope = symbolTable->lookupScope(funcName);
    STEntry * currentEntry = functionScope->next;
    while (currentEntry != nullptr) {
        currentParamMap[currentEntry->name] = (paramCount++)*(-4);
        code << "; arg " << currentEntry->name << " has stack offset " << currentParamMap[currentEntry->name] << nl;
        currentEntry = currentEntry->next;
    }
}

void CodeGenerator::allocateGlobal(std::string name, std::string type) {
    code << "; global " << name << nl;
    int address = currentGlobalOffset;
    code << "LDA #0" << nl; //Initialize with 0
    code << "STA " << padAddress(address) << nl; //Put in memory
    globalToAddressMap[name] = address;
    currentGlobalOffset += 4; //Will vary depending on type in the future
}

void CodeGenerator::allocateStackSpace(int size) {
    code << "; allocate stack space " << size << nl;
    code << "TSX" << nl;
    code << "TXA" << nl;
    code << "ADC #" << size << nl;
    code << "TAX" << nl;
    code << "TXS" << nl;
}

//Look up variable to see if it already has a space in mem or the stack
//Create local if it does not yet exist
//Return variable type and value
CodeVar CodeGenerator::lookup(std::string varName) {
    if (isNumber(varName)) {
        int value = std::stoi(varName);
        return {CVT_CONST, value};  
    }
    if (globalToAddressMap.find(varName) != globalToAddressMap.end()) {
        return {CVT_GLOBAL, globalToAddressMap[varName]};
    }
    if (localToOffsetMap.find(varName) != localToOffsetMap.end()) {
        return {CVT_LOCAL, localToOffsetMap[varName]};
    }
    localToOffsetMap[varName] = currentLocalOffset;
    currentLocalOffset += 4;
    return {CVT_LOCAL, localToOffsetMap[varName]};
}

std::string CodeGenerator::padAddress(int num)
{
    std::ostringstream ss;
    ss << std::setw(4) << std::setfill('0') << num;
    std::string result = ss.str();
    if (result.length() > 4)
    {
        result.erase(0, result.length() - 4);
    }
    return result;
}

bool CodeGenerator::isNumber(std::string s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}