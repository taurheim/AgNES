#include <iostream>
#include <cstdio>
#include <fstream>
#include "scanner.h"

int main() {
  std::ifstream file ("input.c");
  if (file.is_open()) {
    Scanner scanner = Scanner(file);
    scanner.scan();
  } else {
    std::cout << "Could not open file \n";
  }
}