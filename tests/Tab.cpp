#include <iostream>

#include "kvrnel/src/Bytes.cpp"
#include "kvrnel/src/Tab.cpp"

// ---   *   ---   *   ---

int main(void) {

  Tab<std::string,short> tab(64);

  std::string key="item_a";

  tab.push(key,0xDD);
  tab.push(key,0xDF);

  short x=tab.pop(key);
  printf("%04X\n",x);

  return 0;

};

// ---   *   ---   *   ---
