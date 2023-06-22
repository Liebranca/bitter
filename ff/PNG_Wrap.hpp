#ifndef __24_PNG_WRAP_H__
#define __24_PNG_WRAP_H__

// ---   *   ---   *   ---
// deps

  #include <cstdio>
  #include <png.h>

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Evil.hpp"
  #include "kvrnel/Mem.hpp"

// ---   *   ---   *   ---
// info

class PNG {

public:

  VERSION   "v0.00.3b";
  AUTHOR    "IBN-3DILA";

  struct Error {

    CX Evil::Errcode SIZE={

      .type=AR_ERROR,

      .code=__COUNTER__,
      .mess=
        "Image size must be square "
        "and a power of two"

    };

    CX Evil::Errcode WRITE={

      .type=AR_ERROR,

      .code=__COUNTER__,
      .mess="Size of buffer doesn't match image"

    };

  };

// ---   *   ---   *   ---
// attrs

private:

  std::string m_fpath;

  FILE*       m_fp;

  png_structp m_rd;
  png_structp m_wt;
  png_infop   m_info;

  uint16_t    m_sz;
  uint64_t    m_sz_sq;

  bool        m_mode;

// ---   *   ---   *   ---
// iface

public:

  // open for read
  PNG(std::string fpath);

  // open for write
  PNG(std::string fpath,uint16_t sz);

  // destroy
  ~PNG(void);

  // io
  Mem<uint8_t> read(void);
  Mem<float> read_to_float(void);
  void write(Mem<uint8_t>& src);

  // getters
  inline uint16_t get_sz(void) {
    return m_sz;

  };

};

// ---   *   ---   *   ---

#endif // __24_PNG_WRAP_H__
