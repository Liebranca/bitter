#include "kvrnel/src/Evil.cpp"
#include "kvrnel/src/ID.cpp"
#include "kvrnel/src/Mem.cpp"
#include "kvrnel/src/Bin.cpp"
#include "ff/src/Zwrap.cpp"

int main(void) {

  ID id("$","testy");

  Mem<uint8_t,256> in(&id);
  Mem<uint8_t,256> out(&id);

  in.write("HLOWRLD!");

  { Zwrap z(0x00);

    z.set_src(&in[0],256);
    z.set_dst(&out[0],256);

    z.flate();

  };

  out.prich();
  in.cl();

  { Zwrap z(Zwrap::INFLATE);

    z.set_src(&out[0],256);
    z.set_dst(&in[0],256);

    z.flate();

  };

  in.prich();

  return 0;

};
