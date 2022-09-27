#include "src/kvrnel/types/id.c"
#include "src/kvrnel/mem/mem.c"

#include <stdio.h>

class Blk:public Mem {

public:

  uint64_t* buff(int64_t offset) {
    return Mem::buff<uint64_t>(offset);

  };

};

int main(void) {

  ID id("$","testy");
  Blk* m=Mem::poly<Blk>(64,&id);

  m->prich(0);

  Mem::del(m);

};
