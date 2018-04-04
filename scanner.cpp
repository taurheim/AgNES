#include <cctype>
#include <string.h>
#include <stdexcept>
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
    std::cout << ch;
}

Token Scanner::getNextToken() {
    Token sym;
    while (std::isspace(ch) && !input.eof()) getChar(); // skip to next non-whitespace character
    if (std::isalpha(ch)) {
        // Identifier
        sym = identifyKeyword();
    } else if (std::isdigit(ch)) {
        // Number
        sym = identifyNumber();  
    } else if (ch == '\'') {
        // Char
        sym = identifyChar();
    } else if (input.eof()) {
        sym = { T_EOF_TOKEN };
    } else {
        sym = identifySpecialToken();
    }
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
        identToken.type = TokenType::T_IDENT;
        identToken.stringVal = word;
        return identToken;
    } else {
        return { search->second };
    }
}

Token Scanner::identifySpecialToken() {
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
    numberToken.type = TokenType::T_INTVALUE;
    numberToken.intVal = 0;

    // Go through all the digits to get the full number
    while (std::isdigit(ch)) {
        numberToken.intVal = (numberToken.intVal * 10) + (ch - '0');
        getChar();
    }

    return numberToken;
}

Token Scanner::identifyChar() {
    if(ch != '\'') {
        std::cerr << "Char must begin with \"'\"";
        exit(1);
    }

    getChar();
    Token charToken;
    charToken.type = TokenType::T_CHARVALUE;
    charToken.charVal = ch;
    getChar();

    if(ch != '\'') {
        std::cerr << "Char can only be of length 1, expected \"'\"";
        exit(1);
    }
    getChar();

    return charToken;
}

std::list<Token> Scanner::scan() {
    getChar();

    std::list<Token> tokenList;
    while (true) {
        Token t = getNextToken();
        tokenList.push_back(t);

        if (t.type == T_EOF_TOKEN) break;
    }

    if (debug) {
        std::cout << "Scanner output: \n\t";
        for (auto const & i : tokenList) {
            std::cout << tokenToStr(i) << " ";
        }
        std::cout << std::endl;
    }

    return tokenList;
}