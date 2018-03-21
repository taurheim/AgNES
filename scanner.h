#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "token.h"

class Scanner {

    private:
        std::ifstream & input;
        int line;
        int pos;
        char ch;

    public:
        Scanner(std::ifstream & input);
        void scan();

        void getChar();
        Token getNextToken();
        Token identifyKeyword();
        Token identifyChar();
        Token identifyNumber();

};