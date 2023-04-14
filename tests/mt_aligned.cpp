// tests multi-threaded use
// of std::vector for both
// read and write

// ---   *   ---   *   ---
// deps

  #include <thread>
  #include "kvrnel/Style.hpp"

// ---   *   ---   *   ---
// ROM

  struct cline_align Data {

    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t d;

    uint64_t e;
    uint64_t f;
    uint64_t g;
    uint64_t h;

    uint64_t i;

  };

  CLINE_ALIGN_CHK(Data);

  cx16 V_SPLIT  = 2;

// ---   *   ---   *   ---

int main(void) {

  std::vector<uint32_t> ve(Data_V_OFFSET);

  printf(

    "ST %u\n"
    "VE %u\n",

    sizeof(Data),
    ve.size()

  );

  return 0;

};

// ---   *   ---   *   ---
