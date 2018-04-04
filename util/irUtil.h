#pragma once
#include <string>
#include <iostream>
#include <list>
#include "../irGenerator.h"

static void printIRCode(std::list<TAC> code) {
    for (auto & i : code) {
        switch(i.op) {
            case IR_ADD: {
                std::cout << "\t" <<  i.first << " := " << i.second << " + " << i.third;
                break;
            }
            case IR_SUBTRACT: {
                std::cout << "\t" << i.first << " := " << i.second << " - " << i.third;
                break;
            }
            case IR_ASSIGN: {
                std::cout << "\t" << i.first << " := " << i.second;
                break;
            }
            case IR_GOTO: {
                std::cout<< "\t"  << "Goto " << i.first;
                break;
            }
            case IR_IFTRUEGOTO: {
                std::cout << "\t" << "If " << i.first << " goto " << i.second;
                break;
            }
            case IR_IFFALSEGOTO: {
                std::cout << "\t" << "IfNot " << i.first << " goto " << i.second;
                break;
            }
            case IR_NEWLABEL: {
                std::cout << i.first << ":";
                break;
            }
            default: {
                break;
            }
        }
        std::cout << std::endl;
    }
}