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
  inline char& operator[](uint64_t idex) {
    return arr[idex];

  };

  // for running through arr
  inline char* as_ptr(void) {
    return &arr[0];

  };

  // blank out the pixel
  void clear(void) {
    arr[0]^=arr[0];
    arr[1]^=arr[1];
    arr[2]^=arr[2];
    arr[3]^=arr[3];

  };

// ---   *   ---   *   ---

  // straight numerical repr
  inline uint64_t as_key(
    JOJ::SubEncoding& enc

  ) {

    return bitpack<char>(

      &arr[0],

      (char*) enc.values,
      (int*)  enc.cnt

    );

  };

  // ^recover values from repr
  inline void from_key(
    uint64_t k,
    JOJ::SubEncoding& enc

  ) {

    this->clear();

    bitunpack<char>(

      k,

      &arr[0],

      (char*) enc.values,
      (int*)  enc.cnt

    );

  };

// ---   *   ---   *   ---

} Pixel;

// ---   *   ---   *   ---

#endif // __24_JOJ_PIXEL_H__

