#include "codeGenerator.h"
#include <iomanip>
#include <sstream>
#include <cctype>

CodeGenerator::CodeGenerator(std::list<TAC> intermediateCode, SymbolTable * symbolTable) :
 intermediateCode (intermediateCode),
 symbolTable (symbolTable) {
    currentGlobalOffset = globalStart;
    jumpedToMain = false;
}

std::string CodeGenerator::generate() {
    // Make it easy to tell when our code starts
    code << "; Code begins now" << nl;
    code << "NOP" << nl << "NOP" << nl << "NOP" << nl;
    // First set up our stack
    initializeStack();

    // Now run through the code line by line
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
            generateBeginFunc(tac);
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
        case IR_ENDFUNC: {
            generateEndFunc(tac);
            break;
        }
        case IR_NES_WAITFORFRAME: {
            code << ";;;;;; NESWAITFORFRAME ;;;;;;" << nl;
            code << "INC 99" << nl;
            code << "wait_for_frame:" << nl;
            code << "LDA 99" << nl;
            code << "BNE wait_for_frame" << nl;
            break;
        }
        case IR_NES_SETSPRITEX: {
            code << ";;;;;; NESSETSPRITEX ;;;;;;" << nl;
            int offset = globalToAddressMap[tac.first];
            loadGlobalIntoA(offset);
            code << "STA $0203" << nl;
            break;
        }
        case IR_NES_SETSPRITEY: {
            code << ";;;;;; NESSETSPRITEY ;;;;;;" << nl;
            int offset = globalToAddressMap[tac.first];
            loadGlobalIntoA(offset);
            code << "STA $0200" << nl;
            break;
        }
    }
}

void CodeGenerator::initializeStack() {
    // The stack should start with one "fake" item on it - the old FP, to which the current FP points
    code << "; Initialize the stack" << nl;
    code << "LDA #$FF" << nl;
    code << "STA " << cSP << nl;
    code << "STA " << cFP << nl;
}

void CodeGenerator::popStackToA() {
    // X <- cSP
    code << "LDX " << cSP << nl;

    // A <- M[cSP]
    code << "LDA $0,X" << nl;

    // cSP++
    code << "INC " << cSP << nl;
}

void CodeGenerator::pushAToStack() {
    // cSP--
    code << "DEC " << cSP << nl;
    
    // X <- cSP
    code << "LDX " << cSP << nl;

    // M[$0 + $cSP] <- A
    // This effectively means we put the new item at the stack pointer in memory
    code << "STA $0,X" << nl;
}

void CodeGenerator::generateAssign(TAC tac) {
    code << "; assign " << tac.first <<" := " << tac.second << nl;
    CodeVar left = lookup(tac.first);
    CodeVar right = lookup(tac.second);
    if (left.type == CVT_GLOBAL) {
        if (right.type == CVT_LOCAL) {
            // GLOBAL = LOCAL
            code << "; global = local" << nl;
            loadLocalIntoA(right.value);
            storeAIntoGlobal(left.value);
        }
        else {
            // GLOBAL = CONST
            code << "; global = const" << nl;
            loadConstIntoA(right.value);
            storeAIntoGlobal(left.value);
        }
    }
    else if (left.type == CVT_LOCAL) {
        if (right.type == CVT_GLOBAL) {
            // LOCAL = GLOBAL
            code << "; local := global" << nl;
            loadGlobalIntoA(right.value);
            storeAIntoLocal(left.value);
        }
        else if (right.type == CVT_LOCAL) {
            // LOCAL = LOCAL
            code << "; local := local" << nl;
            loadLocalIntoA(right.value);
            storeAIntoLocal(left.value);
        }
        else if (right.type == CVT_PARAM) {
            code << "; local := param" << nl;
            loadParamIntoA(right.value);
            storeAIntoLocal(left.value);
        }
        else {
            // LOCAL = CONST
            code << "; local := const" << nl;
            loadConstIntoA(right.value);
            storeAIntoLocal(left.value);
        }
    }
}

void CodeGenerator::generateAddition(TAC tac) {
    code << "; add " << tac.first << " := " << tac.second << " + " << tac.third << nl;
    CodeVar assignTo = lookup(tac.first);
    CodeVar arg1 = lookup(tac.second);
    CodeVar arg2 = lookup(tac.third);

    // For now, use $0 as our adder
    // This is bad and should not be done this way

    // Set result to 0 to start
    loadConstIntoA(0);
    code << "STA $0" << nl;

    // Add arg1
    loadLocalIntoA(arg1.value);
    code << "CLC" << nl;
    code << "ADC $0" << nl;
    code << "STA $0" << nl;

    // Add arg2
    loadLocalIntoA(arg2.value);
    code << "CLC" << nl;
    code << "ADC $0" << nl;

    // Store the output
    storeAIntoLocal(assignTo.value);
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
    pushAToStack();
}

void CodeGenerator::generatePopParams(TAC tac) {
    code << "; Pop " << tac.first << " params" << nl;
    deallocateStackSpace(stoi(tac.first));
}

void CodeGenerator::generateJump(TAC tac) {
    code << "; jump to " << tac.first << nl;
    code << "JMP " << tac.first << nl;
}

void CodeGenerator::generateSubroutineCall(TAC tac) {
    code << "; push FP, call, restore FP" << nl;

    // Push cFP to stack
    code << "LDA " << cFP << nl;
    pushAToStack();
    
    // Set cFP to cSP
    code << "LDA " << cSP << nl;
    code << "STA " << cFP << nl;

    // Call function
    code << "JSR " << tac.first << nl;

    // Restore FP
    popStackToA();
    code << "STA " << cFP << nl;
}

void CodeGenerator::loadLocalIntoA(int offset){
    // Locals are located below the frame pointer in memory
    /*
        [ Locals ]
        [ FP ]
        [ Params ]
    */
    // X <- cFP
    code << "LDX " << cFP << nl;
    
    // Subtracting is the same as adding 255 - num
    // e.g. we want to load cFP - 3
    // cFP = $F9
    // cFP = $F9 - $3 = 0xF6
    // $F9 + $FC = 0xF6
    // $FD = $FF - $3 = 255 - 3 = 252
    int adjustedOffset = 256 - offset;
    code << "LDA " << adjustedOffset << ",X" << nl;
}

void CodeGenerator::loadParamIntoA(int offset) {
    // Params are located above the frame pointer in memory
    /*
        [ Locals ]
        [ FP ]
        [ Params ]
    */
    code << "LDX " << cFP << nl;
    code << "LDA " << offset << ",X" << nl;
}

void CodeGenerator::loadGlobalIntoA(int address) {
    code << "LDA " << padAddress(address) << nl;
}

void CodeGenerator::loadConstIntoA(int value) {
    code << "LDA #" << value << nl;
}

void CodeGenerator::storeAIntoLocal(int offset) {
    // Locals are below the frame pointer in memory
    // Offset can't be 0 - locals start at 1
    code << "LDX " << cFP << nl;
    int adjustedOffset = 256 - offset;
    code << "STA " << adjustedOffset << ",X" << nl;
}

void CodeGenerator::storeAIntoGlobal(int address) {
    code << "STA " << padAddress(address) << nl;
}

void CodeGenerator::generateBeginFunc(TAC tac) {
    std::string funcName = tac.first;
    int paramSize = stoi(tac.second);

    code << "; beginfunc " << funcName << nl;
    
    // Allocate space for the locals
    code << "; Allocating " << paramSize << " for locals" << nl;
    allocateStackSpace(paramSize);

    // Create a map for the local param names
    currentLocalOffset = 1;
    currentParamMap.clear();
    int paramCount = 1;
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

void CodeGenerator::generateEndFunc(TAC tac) {
    code << "; endFunc" << nl;
    int paramSize = stoi(tac.first);

    // Deallocate space for locals
    deallocateStackSpace(paramSize);

    code << "RTS" << nl;
}

void CodeGenerator::allocateGlobal(std::string name, std::string type) {
    code << "; global " << name << nl;
    int address = currentGlobalOffset;
    loadConstIntoA(0);
    storeAIntoGlobal(address);
    globalToAddressMap[name] = address;
    currentGlobalOffset++; //Will vary depending on type in the future
}

void CodeGenerator::allocateStackSpace(int size) {
    if (size == 0) return;
    // SBC: A = A - M - (1 - C)
    loadConstIntoA(256 - size);
    code << "CLC" << nl;
    code << "ADC " << cSP << nl;
    code << "STA " << cSP << nl;
}

void CodeGenerator::deallocateStackSpace(int size) {
    if (size == 0) return;
    loadConstIntoA(size);
    code << "CLC" << nl; //Set stupid carry
    code << "ADC " << cSP << nl;
    code << "STA " << cSP << nl;
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