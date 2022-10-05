// ---   *   ---   *   ---
// boring details that'd make the header
// too long for the casual reader
// meant to be pasted in!

#ifndef __24_JOJ_PIXEL_H__
#define __24_JOJ_PIXEL_H__

public:

// ---   *   ---   *   ---
// each field in JOJ is a single byte
//
// we use the same structure for each
// type of image, just accessing it's
// fields through a different name
//
// this is just for clarity; no more, no less
//
// for reference:
//
// first    YAUV color
// second   ORM+emit
// third    normal+curv

typedef struct {
  union {
    struct {

// ---   *   ---   *   ---
// arr[0]

      union {
        char luma;
        char occlu;
        char x;

      };

// ---   *   ---   *   ---
// arr[1]

      union {
        char alpha;
        char rough;
        char y;

      };

// ---   *   ---   *   ---
// arr[2]

      union {
        char chroma_u;
        char metal;
        char z;

      };

// ---   *   ---   *   ---
// arr[3]

      union {
        char chroma_v;
        char emit;
        char curv;

      };

// ---   *   ---   *   ---
// ^you can access them as an array:

    };

    char arr[4];

  };

// ---   *   ---   *   ---
// methods

  // for using as an array
  inline char& operator[](size_t idex) {
    return arr[idex];

  };

  // for running through arr
  inline char* as_ptr(void) {
    return &arr[0];

  };

// ---   *   ---   *   ---

  // straight numerical repr
  inline size_t as_key(void) {

    return

      arr[0]

    | (arr[1]<< 8)
    | (arr[2]<<16)
    | (arr[3]<<24)
    ;

  };

  // ^recover values from repr
  inline void from_key(size_t k) {

    arr[0]=k&0xFF;

    arr[1]=(k>>8)&0xFF;
    arr[2]=(k>>16)&0xFF;
    arr[3]=(k>>24)&0xFF;

  };

// ---   *   ---   *   ---

} Pixel;

// ---   *   ---   *   ---

#endif // __24_JOJ_PIXEL_H__
