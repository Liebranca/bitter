#ifndef __24_JOJ_H__
#define __24_JOJ_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Bin.hpp"

// ---   *   ---   *   ---
// defs

  cx64 JOJ_PAL_SZ=16;

// ---   *   ---   *   ---
// first field is YUV+alpha color
// second is ORM+emit
// third is normal+curv

typedef struct {

  union {

    struct {
      union {

        char chroma_u;

        char occlu;
        char x;

      };

      union {

        char chroma_v;

        char rough;
        char y;

      };

      union {

        char luma;

        char metal;
        char z;

      };

      union {
        char alpha;

        char emit;
        char curv;

      };

    };

    char arr[4];

  };

  inline char& operator[](size_t idex) {
    return arr[idex];

  };

} JOJ_PIXEL;

// ---   *   ---   *   ---
// check struct is tight

CASSERT(

  sizeof(JOJ_PIXEL)==sizeof(float),
  "Bad JOJ_PIXEL size!"

);

// ---   *   ---   *   ---
// from 0 (lowest) to 7(highest)
// level of precision to use for
// each field of a material

typedef struct {

  char nvec_xz;
  char nvec_y;
  char curv;

  char chroma;
  char luma;
  char alpha;

  char occlu;
  char rough;
  char metal;
  char emit;

} JOJ_ENCODING;

// ---   *   ---   *   ---

typedef struct {

  uint8_t  mip;

  uint8_t  imcnt;
  uint16_t imsz;

  JOJ_ENCODING  encoding;
  JOJ_PIXEL     palette[JOJ_PAL_SZ];

} JOJ_HEADER;

// ---   *   ---   *   ---
// methods

class JOJ: public Bin {

public:

  VERSION   "v2.00.1";
  AUTHOR    "IBN-3DILA";

  CX JOJ_ENCODING ENC_DEFAULT={

    .nvec_xz  = 1,
    .nvec_y   = 0,
    .curv     = 3,

    .chroma   = 6,
    .luma     = 6,
    .alpha    = 2,

    .occlu    = 3,
    .rough    = 3,
    .metal    = 3,
    .emit     = 3

  };

// ---   *   ---   *   ---

private:

  cxstr   m_fsig="%JOJ";
  cx64    m_header_sz=sizeof(JOJ_HEADER);

  float*  m_pixels;

  JOJ_ENCODING m_enc;

// ---   *   ---   *   ---

public:

  // file into memory
  JOJ(std::string fpath);

  // memory into file
  JOJ(

    std::string fpath,

    float* pixels,
    size_t sz,

    JOJ_ENCODING enc=ENC_DEFAULT

  );

  // stores unit vector in joj format
  void encode_nvec(
    float* n,
    JOJ_PIXEL& j

  );

  // ^retrieves unit vector from joj
  void decode_nvec(
    float* n,
    JOJ_PIXEL& j

  );

};

// ---   *   ---   *   ---

#endif // __24_JOJ_H__
