#include <iostream>

#include "kvrnel/src/Evil.cpp"
#include "kvrnel/src/Bytes.cpp"
#include "kvrnel/src/Bin.cpp"
#include "kvrnel/src/Tab.cpp"

#include <ctime>

// ---   *   ---   *   ---

uint64_t rui(void) {

  return

    ((uint64_t) rand())
//  / ((uint64_t) RAND_MAX)
  ;

};

// ---   *   ---   *   ---

int main(void) {

  srand((uint64_t) time(NULL));

  std::vector<uint64_t> keys;
  std::vector<uint64_t> values;

  Tab<uint64_t,TAB::Symbol> tab(256);

  for(uint64_t i=0;i<0x4000;i++) {

    uint64_t x=rui()&0xFF;

    TAB::Symbol sym;
    TAB::Lookup lkp=tab.has(x);

// ---   *   ---   *   ---

    if(!lkp.key_match) {

      sym=(TAB::Symbol) {

        .idex=i,
        .value=x,

        .freq=1

      };

      keys.push_back(x);
      tab.push(lkp,x,sym);

// ---   *   ---   *   ---

    } else {
      sym=tab.get(lkp);
      sym.freq++;

      tab.set(lkp,sym);

    };

// ---   *   ---   *   ---

  };

  tab.sort(keys,values);

  Bin b("./tab",Bin::NEW);

  b.write(
    values.data(),
    values.size()*sizeof(uint64_t)

  );

  return 0;

};

// ---   *   ---   *   ---
