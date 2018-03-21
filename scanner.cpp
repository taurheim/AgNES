#include <cctype>
#include <string.h>
#include "scanner.h"
#include "tokenMaps.h"
#include "util/tokenUtil.h"

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

Token Scanner::getNextToken() {
    Token sym;
    while (std::isspace(ch)) getChar(); // skip to next non-whitespace character
    if (std::isalpha(ch)) sym = identifyKeyword();
    else if (std::isdigit(ch)) sym = identifyNumber();
    else sym = identifyChar();
    return sym;
}

Token Scanner::identifyKeyword() {
    std::string word;
    while (std::isalpha(ch)) { // create string... TODO: include numeric
        word += ch;
        getChar();
    }
    auto search = stringToToken.find(word);
    if (search == stringToToken.end()) {
        Token identToken;
        identToken.type = TokenType::IDENT;
        identToken.stringVal = word;
        return identToken;
    } else {
        return { search->second };
    }
}

Token Scanner::identifyChar() {
    auto search = charToToken.find(ch);
    if (search == charToToken.end()) {
        std::cerr << "Invalid character!";
        exit(1);
    } else {
        getChar();
        return { search->second };
    }
}

Token Scanner::identifyNumber() {
    Token numberToken;
    numberToken.type = TokenType::NUM;
    numberToken.intVal = 0;

    // Go through all the digits to get the full number
    while (std::isdigit(ch)) {
        numberToken.intVal = (numberToken.intVal * 10) + (ch - '0');
        getChar();
    }

    return numberToken;
}

void Scanner::scan() {
    getChar();
    while (!input.eof()) {
        std::cout << tokenToStr(getNextToken()) << " ";
    }
    std::cout << "\n";
}