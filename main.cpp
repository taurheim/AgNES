#include <iostream>
#include <cstdio>
#include <fstream>
#include "scanner.h"
#include "token.h"
#include "parser.h"

int main() {
    // First we run Lexical Analysis, to turn the stream of characters into a list of tokens
    // Input: file
    // Output: list of tokens
    std::ifstream file ("input.c");
    std::list<Token> tokens;
    if (file.is_open()) {
        Scanner scanner = Scanner(file);
        tokens = scanner.scan();
    } else {
        std::cout << "Could not open file \n";
        return 1;
    }

    // Next we run Syntax Analysis/Parsing, where we generate a parse tree
    // Input: list of tokens
    // Output: parse tree
    Parser parser = Parser(tokens);
    parser.parse();

    std::cout << "Finished Compilation." << std::endl;
}