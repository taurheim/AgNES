#pragma once
#include <string>
#include <iostream>
#include <list>
#include "../irGenerator.h"

static void printIRCode(std::list<TAC> code) {
    for (auto & i : code) {
        std::cout << i.result << " := " << i.adr1;
        switch(i.op) {
            case IR_ADD: {
                std::cout << " + " << i.adr2;
                break;
            }
            case IR_SUBTRACT: {
                std::cout << " - " << i.adr2;
                break;
            }
            default: {
                break;
            }
        }
        std::cout << std::endl;
    }
}