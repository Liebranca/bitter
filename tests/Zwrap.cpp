#include "kvrnel/src/Evil.cpp"
#include "kvrnel/src/ID.cpp"
#include "kvrnel/src/Mem.cpp"
#include "kvrnel/src/Bin.cpp"
#include "ff/src/Zwrap.cpp"

int main(void) {

  ID id("$","testy");
  Mem<uint8_t> m(&id,256);

  { Zwrap z(Zwrap::DEFLATE

    | Zwrap::INPUT_BIN
    | Zwrap::OUTPUT_BIN

    );

    Bin in("./ztest_in",Bin::READ);
    Bin out("./ztest_out",Bin::WRITE|Bin::TRUNC);

    z.set_src(&in);
    z.set_dst(&out);

    z.flate();

  };

  { Zwrap z(Zwrap::INFLATE|Zwrap::INPUT_BIN);

    Bin in("./ztest_out",Bin::READ);

    z.set_src(&in);
    z.set_dst(&m[0],256);

    z.flate();

  };

  m.prich();

  return 0;

};
