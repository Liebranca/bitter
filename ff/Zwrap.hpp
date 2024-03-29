#ifndef __24_ZWRAP_H__
#define __24_ZWRAP_H__

// ---   *   ---   *   ---
// deps

  #include "zlib.h"

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Mem.hpp"
  #include "kvrnel/Bin.hpp"

// ---   *   ---   *   ---
// info

class Zwrap {

public:

  VERSION   "v2.00.3";
  AUTHOR    "IBN-3DILA";

  cx8 DEFLATE     = 0x01;
  cx8 INFLATE     = 0x02;

  cx8 INPUT_BIN   = 0x04;
  cx8 OUTPUT_BIN  = 0x08;

  struct Error {

    CX Evil::Errcode ZLIB={

      .type=AR_FATAL,

      .code=__COUNTER__,
      .mess="ZLIB went PLOP!"

    };

  };

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

    void set(
      Bin*     src,

      uint64_t size,
      uint64_t offset=0

    );

    void set(
      uint8_t* src,

      uint64_t size,
      uint64_t offset=0

    );

  } Target;

// ---   *   ---   *   ---
// attrs

  Target       m_dst;
  Target       m_src;

  z_stream     m_strm;

  uint64_t     m_readsize;
  uint64_t     m_remain;

  uint64_t     m_flush;

  int          m_status;
  int          m_mode;

  Mem<uint8_t> m_in_buff;
  Mem<uint8_t> m_out_buff;

// ---   *   ---   *   ---
// consts

  cx8 WINDOW_BITS      = 15;
  cx8 GZIP_ENCODING    = 16;
  cx8 ENABLE_ZLIB_GZIP = 32;

// ---   *   ---   *   ---
// internals

  // caps next read to remaining bytes
  inline void get_readsize(void) {

    m_readsize=(m_readsize > m_remain)
      ? m_remain
      : m_readsize
      ;

  };

  std::string get_status(void);

  void dump(void);
  void next_chunk(void);

  uint64_t process(
    uint8_t* dst,
    uint64_t avail

  );

  // exit condition for flate loop
  bool flating(void) {

    bool out=(m_mode & Zwrap::DEFLATE)
      ?   m_strm.avail_in
      : ! m_strm.avail_out
      ;

    // i curse the zlib docs to
    // eternal damnation
    if(m_status==Z_OK && ! m_strm.avail_out) {
      out=1;

    };

    return out;

  };

// ---   *   ---   *   ---
// iface

public:

  Zwrap(int mode=0x00);
  ~Zwrap(void);

  void flate(void);

// ---   *   ---   *   ---
// setters

  void set_src(
    void*    src,

    uint64_t size=0,
    uint64_t offset=0

  );

  void set_dst(
    void*    dst,

    uint64_t size=0,
    uint64_t offset=0

  );

};

// ---   *   ---   *   ---

#endif // __24_ZWRAP_H__
