#ifndef __24_PNG_WRAP_H__
#define __24_PNG_WRAP_H__

// ---   *   ---   *   ---
// deps

  #include <cstdio>
  #include <png.h>

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Mem.hpp"

// ---   *   ---   *   ---
// info

class PNG {

public:

  VERSION   "v0.00.1b";
  AUTHOR    "IBN-3DILA";

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
  void write(Mem<uint8_t>& src);

};

// ---   *   ---   *   ---

#endif // __24_PNG_WRAP_H__
