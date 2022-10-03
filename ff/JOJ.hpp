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
// first field is YAUV color
// second is ORM+emit
// third is normal+curv

typedef struct {

  union {

    struct {
      union {

        char luma;

        char occlu;
        char x;

      };

      union {

        char alpha;

        char rough;
        char y;

      };

      union {

        char chroma_u;

        char metal;
        char z;

      };

      union {

        char chroma_v;

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

  char normal[9];
  char color[9];
  char shade[3];

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

    .normal={

      // xz
      Frac::STEP_2BIT,
      Frac::SIZE_3BIT,
      Frac::SIGNED,

      // y
      Frac::STEP_2BIT,
      Frac::SIZE_3BIT,
      Frac::SIGNED,

      // curv
      Frac::STEP_3BIT,
      Frac::SIZE_4BIT,
      Frac::UNSIGNED

    },

    .color={

      // alpha && luma
      Frac::STEP_3BIT,
      Frac::SIZE_3BIT,
      Frac::UNSIGNED,

      // chroma_u
      Frac::STEP_4BIT,
      Frac::SIZE_4BIT,
      Frac::SIGNED,

      // chroma_v
      Frac::STEP_4BIT,
      Frac::SIZE_4BIT,
      Frac::SIGNED

    },

    .shade={

      // occlu,rough,metal,emit
      Frac::STEP_3BIT,
      Frac::STEP_4BIT,
      Frac::UNSIGNED

    }

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
    JOJ_PIXEL& j,

    bool mode=Frac::ENCODE

  );

  // ^color
  void encode_color(
    float* n,
    JOJ_PIXEL& j,

    bool mode=Frac::ENCODE

  );

};

// ---   *   ---   *   ---

#endif // __24_JOJ_H__
