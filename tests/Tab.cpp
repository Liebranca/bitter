#include <iostream>

#include "kvrnel/src/Bytes.cpp"
#include "kvrnel/src/Tab.cpp"

// ---   *   ---   *   ---

int main(void) {

  Tab<size_t,short> tab(64);

  size_t key=0;

  tab.push(key,0xDD);

  tab.set(key,0xDF);
  short x=tab.get(key);
  printf("%04X\n",x);

  return 0;

};

// ---   *   ---   *   ---
