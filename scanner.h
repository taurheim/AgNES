#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include "token.h"

class Scanner {

    private:
        std::ifstream & input;
        int line;
        int pos;
        char ch;
        bool debug = true;

    public:
        Scanner(std::ifstream & input);
        std::list<Token> scan();

        void getChar();
        Token getNextToken();
        Token identifyKeyword();
        Token identifyChar();
        Token identifyNumber();

};