#include "kvrnel/Mem.hpp"
#include "kvrnel/Bin.hpp"
#include "ff/Zwrap.hpp"

int main(void) {

  Mem<uint8_t> m(256);

  {
    Zwrap z(

      Zwrap::DEFLATE

    | Zwrap::INPUT_BIN
    | Zwrap::OUTPUT_BIN

    );

    Bin in("./ztest_in",Bin::READ);
    Bin out("./ztest_out",Bin::NEW);

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
