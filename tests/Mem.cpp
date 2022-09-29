#include "kvrnel/src/ID.cpp"
#include "kvrnel/src/Evil.cpp"
#include "kvrnel/src/Mem.cpp"

#include <cstdio>

// ---   *   ---   *   ---

class xMem: public Mem<short> {

public:

  static xMem& nit(size_t buff_sz,ID* id) {

    return *((xMem*) Mem<short>::nit(
      buff_sz,sizeof(xMem),id

    ));

  };

  size_t flags;
  size_t whats;

};

// ---   *   ---   *   ---

int main(void) {

  ID id("$","bb");
  xMem& m=xMem::nit(64,&id);

  m.write("00000000");

  printf("%016X\n",&m.flags);

  m.prich();
  m.del();

  return 0;

};

// ---   *   ---   *   ---
