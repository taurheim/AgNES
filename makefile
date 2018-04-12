CC=gcc
CFLAGS=-I
make: main.cpp scanner.cpp parser.cpp semanticAnalyzer.cpp symbolTable.cpp irGenerator.cpp codeGenerator.cpp
	g++ -w -g -std=c++11 main.cpp scanner.cpp parser.cpp semanticAnalyzer.cpp symbolTable.cpp irGenerator.cpp codeGenerator.cpp -o build/AgNES