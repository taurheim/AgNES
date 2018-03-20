#include "scanner.h"

Scanner::Scanner(std::ifstream & input) 
    : input (input) {
    line = 0;
    pos = 0;
    ch = 0;
};

void Scanner::getChar() {
    input.get(ch);
    //std::cout << ch;
}

Symbol Scanner::getSym() {
    Symbol sym;
    while (std::isspace(ch)) getChar(); // skip to next non-whitespace character
    if (std::isalpha(ch)) sym = identifyKeyword();
    else if (std::isdigit(ch)) sym = identifyNumber();
    else sym = identifyChar();
    return sym;
}

Symbol Scanner::identifyKeyword() {
    std::string word;
    while (std::isalpha(ch)) { // create string... TODO: include numeric
        word += ch;
        getChar();
    }
    auto search = stringToSymbol.find(word);
    if (search == stringToSymbol.end()) {
        return Symbol::IDENT;
    } else {
        return search->second;
    }
}

Symbol Scanner::identifyChar() {
    auto search = charToSymbol.find(ch);
    if (search == charToSymbol.end()) {
        std::cerr << "Invalid character!";
        exit(1);
    } else {
        getChar();
        return search->second;
    }
}

Symbol Scanner::identifyNumber() {
    int num;
    while (std::isdigit(ch)) {
        num = num * 10 + (ch - '0');
        getChar();
    }
    return Symbol::NUM;
}

void Scanner::scan() {
    getChar();
    while (!input.eof()) std::cout << getSym() << "\n";
    std::cout << "\n";
}