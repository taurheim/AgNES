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
            case IR_BEGINFUNC: {
                std::cout << "\t" << "BeginFunc " << i.first << " " << i.second;
                break;
            }
            case IR_ENDFUNC: {
                std::cout << "\t" << "EndFunc";
                break;
            }
            case IR_NEWLABEL: {
                std::cout << i.first << ":";
                break;
            }
            case IR_CALL: {
                std::cout << "\t" << "Call " << i.first;
                break;
            }
            case IR_PUSHPARAM: {
                std::cout << "\t" << "PushParam " << i.first;
                break;
            }
            case IR_POPPARAMS: {
                std::cout << "\t" << "PopParams " << i.first;
                break;
            }
            case IR_DECLARATION: {
                std::cout << "\t" << "Declare " << i.first << " as " << i.second;
                break;
            }
            case IR_RETURN: {
                std::cout << "\t" << "Return " << i.first;
                break;
            }
            case IR_NES_WAITFORFRAME: {
                std::cout << "\t" << "NES_WAITFORFRAME";
                break;
            }
            case IR_NES_SETSPRITEX: {
                std::cout << "\t" << "NES_SETSPRITEX " << i.first;
                break;
            }
            case IR_NES_SETSPRITEY: {
                std::cout << "\t" << "NES_SETSPRITEY " << i.first;
                break;
            }
            default: {
                std::cout << "\t" << "<IR NOT SUPPORTED BY irUtil>";
                break;
            }
        }
        std::cout << std::endl;
    }
}