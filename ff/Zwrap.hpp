#ifndef __24_ZWRAP_H__
#define __24_ZWRAP_H__

// ---   *   ---   *   ---
// deps

  #include <memory>
  #include "zlib.h"

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Bin.hpp"

// ---   *   ---   *   ---
// info

class Zwrap {

public:

  VERSION   "v2.00.1";
  AUTHOR    "IBN-3DILA";

  cx8 DEFLATE     = 0x00;
  cx8 INFLATE     = 0x01;

  cx8 INPUT_BIN   = 0x02;
  cx8 OUTPUT_BIN  = 0x04;

// ---   *   ---   *   ---
// helpers

private:

  typedef struct {

    union {

      uint8_t* bytes;
      Bin*     bin;

    };

    uint64_t size;
    uint64_t offset;

// ---   *   ---   *   ---

    inline void set(
      Bin*     src,

      uint64_t size,
      uint64_t offset=0

    );

    inline void set(
      uint8_t* src,

      uint64_t size,
      uint64_t offset=0

    );

  } Target;

// ---   *   ---   *   ---
// attrs

  Target    m_dst;
  Target    m_src;

  z_stream  m_strm;

  uint64_t  m_readsize;
  uint64_t  m_remain;

  uint64_t  m_flush;

  int       m_status;
  int       m_mode;

  std::unique_ptr<uint8_t> m_buff;

// ---   *   ---   *   ---
// consts

  cx8 WINDOW_BITS      = 15;
  cx8 GZIP_ENCODING    = 16;
  cx8 ENABLE_ZLIB_GZIP = 32;

// ---   *   ---   *   ---
// internals

  inline void get_readsize(void);
  std::string get_status(void);

  void dump(void);
  void next_chunk(void);

  uint64_t process(
    uint8_t* dst,
    uint64_t avail

  );

// ---   *   ---   *   ---
// iface

public:

  Zwrap(int mode=0x00);
  ~Zwrap(void);

  int flate(void);

// ---   *   ---   *   ---
// setters

  inline void set_src(
    void*    src,

    uint64_t size=0,
    uint64_t offset=0

  );

  inline void set_dst(
    void*    dst,

    uint64_t size=0,
    uint64_t offset=0

  );

};

// ---   *   ---   *   ---

#endif // __24_ZWRAP_H__
