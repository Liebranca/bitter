#include "src/kvrnel/types/id.c"
#include <iostream>

int main(void) {

  ID id("$","testy");
  std::cout << id.gfull() << std::endl;

};
