#include "kvrnel/Mem.hpp"

// ---   *   ---   *   ---

template class Mem<short>;

int main(void) {

  Mem<short> m(66);

  m.write((void*) "$$$$$$$$$$$$$$$$",8);
  m.prich();

  return 0;

};

// ---   *   ---   *   ---
