#include "codeGenerator.h"
#include <iomanip>
#include <sstream>

CodeGenerator::CodeGenerator(std::list<TAC> intermediateCode, SymbolTable * symbolTable) :
 intermediateCode (intermediateCode),
 symbolTable (symbolTable) {
     currentGlobalOffset = 0;
     currentLocalOffset = 2;
     jumpedToMain = false;
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
            if (!jumpedToMain) {
                code << "JMP main" << nl;
                jumpedToMain = true;
            }
            code.seekp(-1, code.cur);
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
            break;
        }
        case IR_POPPARAMS: {
            generatePopParams(tac);
            break;
        }
        case IR_CALL: {
            generateSubroutineCall(tac);
            break;
        }
        case IR_RETURN: {
            code << "RTS" << nl;
            break;
        }
    }
}

void CodeGenerator::generateAssign(TAC tac) {
    code << "; assign " << tac.first <<" := " << tac.second << nl;
    CodeVar left = lookup(tac.first);
    CodeVar right = lookup(tac.second);
    if (left.type == CVT_GLOBAL) {
        if (right.type == CVT_LOCAL) {
            loadLocalIntoA(right.value);
            code << "STA " << padAddress(left.value) << nl;
        }
        else { //const
            code << "LDA #" << right.value << nl;
            code << "STA " << padAddress(left.value) << nl;
        }
    }
    else if (left.type == CVT_LOCAL) {

        if (right.type == CVT_GLOBAL) {
            code << "; local := global" << nl;
            setupLocalAddress(left.value);
            code << "LDA " << padAddress(right.value) << nl;
            code << "LDY #0" << nl;
            code << "STA ($2),Y" << nl;
        }
        else if (right.type == CVT_LOCAL) {
            code << "; local := local" << nl;
            loadLocalIntoA(right.value);
            code << "TAX" << nl;
            setupLocalAddress(left.value);
            code << "TXA" << nl;
            code << "STA ($2),Y" << nl;
        }
        else if (right.type == CVT_PARAM) {
            code << "; local := param" << nl;
            //Set up the stack memory to get RHS
            code << "LDA $0" << nl;
            code << "CLC" << nl; // Stupid carry
            code << "ADC #" << right.value << nl;
            code << "STA $2" << nl;

            //Actually reference it now
            code << "LDY #0" << nl;
            code << "LDA ($2),Y" << nl;
            code << "TAX" << nl;

            setupLocalAddress(left.value);
            //Move the value from RHS into LHS
            code << "TXA" << nl;
            code << "STA ($2),Y" << nl;
        }
        else { //const
            code << "; local := const" << nl;
            setupLocalAddress(left.value);
            //Actually reference it now
            code << "LDA #" << right.value << nl;
            code << "LDY #0" << nl;
            code << "STA ($2),Y" << nl;
        }
    }
}

void CodeGenerator::generateAddition(TAC tac) {
    code << "; add " << tac.first << " := " << tac.second << " + " << tac.third << nl;
    CodeVar assignTo = lookup(tac.first);
    CodeVar arg1 = lookup(tac.second);
    CodeVar arg2 = lookup(tac.third);

    loadLocalIntoA(arg1.value);
    code << "TAX" << nl;
    setupLocalAddress(arg2.value);
    code << "TXA" << nl;
    code << "LDY #0" << nl;
    code << "CLC" << nl; // Stupid carry
    code << "ADC ($2),Y" << nl;
    //Move A to assignTo
    code << "TAX" << nl;
    setupLocalAddress(assignTo.value);
    code << "TXA" << nl;
    code << "LDY #0" << nl;
    code << "STA ($2),Y" << nl;
}

void CodeGenerator::generateBranchOnCondition(TAC tac, bool condition) {
    code << "; branch if " << tac.first << " is " << condition << nl;
    CodeVar var = lookup(tac.first);
    loadLocalIntoA(var.value);
    code << "CMP #" << condition << nl;
    code << "BEQ " << tac.second << nl;
}

void CodeGenerator::generatePushParam(TAC tac) {
    CodeVar param = lookup(tac.first);
    code << "; push param " << tac.first << nl;
    loadLocalIntoA(param.value);
    code << "PHA" << nl;
}

void CodeGenerator::generatePopParams(TAC tac) {
    code << "; pop params one byte at a time..." << nl;
    int size = std::stoi(tac.first);
    while (size > 0) {
        code << "PLA" << nl;
        size--;
    }
}

void CodeGenerator::generateJump(TAC tac) {
    code << "; jump to " << tac.first << nl;
    code << "JMP " << tac.first << nl;
}

void CodeGenerator::generateSubroutineCall(TAC tac) {
    code << "; push FP, call, restore FP" << nl;
    code << "LDA $0" << nl;
    code << "PHA" << nl; //Push FP
    code << "TSX" << nl; //Get stack pointer
    code << "STX $0" << nl; //Set FP to stack pointer
    code << "JSR " << tac.first << nl;
    code << "PLA" << nl; //Pop FP
    code << "STA $0" << nl;  //Set FP to old FP
}

void CodeGenerator::generateFramePointer() {
    code << "; frame pointer setup " << nl;
    code << "TSX" << nl; //Save SP to X
    code << "STX 0000" << nl; //Save SP to mem at 0000
    code << "PHA" << nl << "PHA" << nl; //Fake PC
    code << "LDX #1" << nl; // TEMP FRAME POINTER
    code << "STX 0003" << nl;
    globalToAddressMap["$fp"] = currentGlobalOffset;
    currentGlobalOffset += 4; // Just allocated 4 things
}

void CodeGenerator::setupLocalAddress(int offset){
    //Set up the stack memory to get local
    code << "LDA $0" << nl;
    code << "SEC" << nl; //Set stupid carry
    code << "SBC #" << offset << nl;
    code << "STA $2" << nl;
}

void CodeGenerator::loadLocalIntoA(int offset){
    setupLocalAddress(offset);
    //Actually reference it now
    code << "LDY #0" << nl;
    code << "LDA ($2),Y" << nl;
}

// void CodeGenerator::beginFunc(std::string funcName) {
//     currentLocalOffset = 2;
//     currentParamMap.clear();
//     int paramCount = 2;
//     code << "; beginfunc " << funcName << nl;
//     STEntry * functionScope = symbolTable->lookupScope(funcName);
//     STEntry * currentEntry = functionScope->next;
//     while (currentEntry != nullptr) {
//         std::string name = currentEntry->name;
//         currentParamMap[name] = (paramCount++);
//         code << "; arg " << name << " has stack offset " << currentParamMap[name] << nl;  
//         currentEntry = currentEntry->next;
//     }
// }

void CodeGenerator::beginFunc(std::string funcName) {
    currentLocalOffset = 2;
    currentParamMap.clear();
    int paramCount = 2;
    code << "; beginfunc " << funcName << nl;
    STEntry * functionScope = symbolTable->lookupScope(funcName);
    STEntry * currentEntry = functionScope->next;
    std::list<std::string> paramNames;
    while (currentEntry != nullptr) {
        std::string name = currentEntry->name;
        paramNames.push_front(name); 
        currentEntry = currentEntry->next;
    }
    for (auto & name : paramNames) {
        currentParamMap[name] = (paramCount++);
        code << "; arg " << name << " has stack offset " << currentParamMap[name] << nl; 
    } 
}

void CodeGenerator::allocateGlobal(std::string name, std::string type) {
    code << "; global " << name << nl;
    int address = currentGlobalOffset;
    code << "LDA #0" << nl; //Initialize with 0
    code << "STA " << padAddress(address) << nl; //Put in memory
    globalToAddressMap[name] = address;
    currentGlobalOffset++; //Will vary depending on type in the future
}

void CodeGenerator::allocateStackSpace(int size) {
    code << "; allocate stack space " << size << nl;
    code << "TSX" << nl;
    code << "TXA" << nl;
    code << "SEC" << nl; //Set stupid carry
    code << "SBC #" << size << nl;
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
    if (currentParamMap.find(varName) != currentParamMap.end()) {
        return {CVT_PARAM, currentParamMap[varName]};
    }
    if (globalToAddressMap.find(varName) != globalToAddressMap.end()) {
        return {CVT_GLOBAL, globalToAddressMap[varName]};
    }
    if (localToOffsetMap.find(varName) != localToOffsetMap.end()) {
        return {CVT_LOCAL, localToOffsetMap[varName]};
    }
    localToOffsetMap[varName] = currentLocalOffset;
    currentLocalOffset++;
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