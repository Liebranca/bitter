#include "src/kvrnel/types/id.c"
#include "src/kvrnel/mem/mem.c"

#include <stdio.h>

class Empty {};

template class Mem<Empty,uint8_t>;
typedef Mem<Empty,uint8_t> Blk;

int main(void) {

  ID id("$","testy");
  Blk* m=Blk::nit(65,&id);
  Blk& rm=*m;

  rm[0]=0x48;
  rm[1]=0x4C;
  rm[2]=0x4F;
  rm[3]=0x57;

  rm[4]=0x52;
  rm[5]=0x4C;
  rm[6]=0x44;
  rm[7]=0x21;

  m->prich(0);

};
