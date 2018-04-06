#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include <streambuf>
#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "semanticAnalyzer.h"
#include "astNode.h"
#include "symbolTable.h"
#include "irGenerator.h"
#include "codeGenerator.h"

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
    ASTNode * astRoot = parser.parse();

    //Now we make a SemanticAnalyzer!
    std::cout << "Running semantic analysis..." << std::endl;
    SemanticAnalyzer analyzer = SemanticAnalyzer(astRoot);
    SymbolTable * symbolTable = analyzer.analyze();

    // Generate intermediate code
    std::cout << "Running IR Generator..." << std::endl;
    IRGenerator irGenerator = IRGenerator(astRoot);
    std::list<TAC> intermediateCode = irGenerator.generate();

    //Generate machine code
    std::cout << "Running Code Generator..." << std::endl;
    CodeGenerator codeGenerator = CodeGenerator(intermediateCode, symbolTable);
    std::string machineCode = codeGenerator.generate();
    std::cout << machineCode;

    // Sandwich our machine code in the middle of the header and footer
    std::cout << "Writing to output.s" << std::endl;
    std::ifstream header("NES_HEADER.s");
    std::string headerStr((std::istreambuf_iterator<char>(header)), std::istreambuf_iterator<char>());
    std::ifstream footer("NES_FOOTER.s");
    std::string footerStr((std::istreambuf_iterator<char>(footer)), std::istreambuf_iterator<char>());
    std::remove("output.s");
    std::ofstream outputFile;
    outputFile.open("output.s");

    outputFile << headerStr;
    outputFile << machineCode;
    outputFile << footerStr;

    header.close();
    footer.close();
    outputFile.close();

    std::cout << "Finished Compilation." << std::endl;
}