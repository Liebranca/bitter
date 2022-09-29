#ifndef __24_JOJ_H__
#define __24_JOJ_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"

// ---   *   ---   *   ---
// first field is YUV+A color
// second is ORM+E
// third is NVEC

typedef struct {

  union {

    char chroma_u;

    char occlu;
    char sign;

  };

  union {

    char chroma_v;

    char rough;
    char x;

  };

  union {

    char luma;

    char metal;
    char y;

  };

  union {
    char alpha;

    char edge;
    char z;

  };

} JOJ_PIXEL;

// ---   *   ---   *   ---
// check struct is tight

CASSERT(

  sizeof(JOJ_PIXEL)==sizeof(float),
  "Bad JOJ_PIXEL size!"

);

// ---   *   ---   *   ---
// methods

class JOJ {

private:

  VERSION   "v2.00.1";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---

  // compiler trash
  JOJ() {};

  // file into memory
  JOJ(const char* fpath);

  // memory into file
  JOJ(float* pixels,int encoding);

};

// ---   *   ---   *   ---

#endif // __24_JOJ_H__
