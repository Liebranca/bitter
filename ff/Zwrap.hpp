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

  enum {INFLATE,DEFLATE};

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

  uint64_t  m_readsize;
  uint64_t  m_remain;

  uint64_t  m_flush;
  uint64_t  m_mode;

  z_stream  m_strm;
  int       m_status;

  std::unique_ptr<uint8_t> m_buff;

// ---   *   ---   *   ---
// consts

  cx8 WINDOW_BITS      = 15;
  cx8 GZIP_ENCODING    = 16;
  cx8 ENABLE_ZLIB_GZIP = 32;

// ---   *   ---   *   ---
// internals

  inline uint64_t get_readsize(void);
  std::string get_status(void);

  void dump_i(uint8_t* dst,uint8_t* src);
  void next_chunk(uint8_t* src);

// ---   *   ---   *   ---
// iface

public:

  Zwrap(bool mode);
  ~Zwrap(void);

  template <typename dst_t,typename src_t>
  int inflate<dst_t,src_t>(void);

// ---   *   ---   *   ---
// setters

  template <typename T>
  inline void set_src(
    T*       src,

    uint64_t size,
    uint64_t offset=0

  );

  template <typename T>
  inline void set_dst(
    T*       dst,

    uint64_t size,
    uint64_t offset=0

  );

};

// ---   *   ---   *   ---

#endif // __24_ZWRAP_H__
