#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "symbol.h"
#include "symbolMaps.h"

class Scanner {

    private:
        std::ifstream & input;
        int line;
        int pos;
        char ch;

    public:

        Scanner(std::ifstream & input);
        void getChar();
        Symbol getSym();
        Symbol identifyKeyword();
        Symbol identifyChar();
        Symbol identifyNumber();
        void scan();

};