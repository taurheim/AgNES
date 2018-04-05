#pragma once
#include "astNode.h"
#include "irGenerator.h"
#include <string>
#include <list>
#include <map>

// 6502 instruction!
struct Instruction {

}

class CodeGenerator {
    public:
        CodeGenerator(std::list<TAC> intermediateCode);
        void generate();
    private:
        std::list<TAC> intermediateCode;
        //Generate registers and addresses and bla
}