# AgNES

## Scanner
Turns source code into a token stream. Removes white space and comments, and identifies what kind of token each piece of the code is.

## Parser
Turns a token stream into a parse tree. Checks that the sequence of tokens is grammatically correct and can be grouped together according to the specifications of how the language works.

## Semantic Analysis
Turns a parse tree into an abstract syntax tree. Cuts out any unnecessary or redundant information from the parse tree to form a concise representation of what the program means (i.e., the AST).

## Code Generation
Turns that AST into executable machine code.