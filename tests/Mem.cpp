#include "src/kvrnel/types/id.c"
#include "src/kvrnel/mem/mem.c"

#include <stdio.h>

class Blk:public Mem {

public:

  uint64_t& operator[](int64_t offset) {
    offset*=sizeof(uint64_t);
    return Mem::buff<uint64_t&>(offset);

  };

};

int main(void) {

  ID id("$","testy");
  Blk m=Mem::poly<Blk&>(128,&id);

  m[0]=0x02;
  m[1]=0x40;

  m.prich(0);
  Mem::del(&m);

};
