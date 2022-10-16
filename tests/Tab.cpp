#include <iostream>
#include <ctime>

#include "ff/PAL.hpp"

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

  { PAL pal("./pal8",Bin::NEW,256);

    for(uint64_t i=0;i<0x8000;i++) {

      uint64_t x=rui()&0xFF;
      pal.cpush(x);

    };

    pal.write();

  };

  PAL pal("./pal8");
  printf("%04X\n",pal.get(0));

  return 0;

};

// ---   *   ---   *   ---
