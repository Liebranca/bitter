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

typedef struct FwdPixel {
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
  inline void clear(void) {
    arr[0]^=arr[0];
    arr[1]^=arr[1];
    arr[2]^=arr[2];
    arr[3]^=arr[3];

  };

  // ^return pixel is blank
  inline bool is_clear(void) {

    return
       arr[0]==0
    && arr[1]==0
    && (arr[2]==8 || arr[2]==0)
    && (arr[3]==8 || arr[3]==0)
    ;

  };

// ---   *   ---   *   ---

  inline uint32_t as_int(void) {
    return *((uint32_t*) &arr[0]);

  };

  inline void from_int(uint32_t v) {
    *((uint32_t*) &arr[0])=v;

  };

  // straight numerical repr
  inline uint64_t as_key(
    JOJ::SubEncoding& enc

  ) {

    return bitpack<uint8_t>(

      (uint8_t*) &arr[0],

      (uint8_t*) enc.values,
      (int*)  enc.cnt

    );

  };

  // ^recover values from repr
  inline void from_key(
    uint64_t k,
    JOJ::SubEncoding& enc

  ) {

    this->clear();

    bitunpack<uint8_t>(

      k,

      (uint8_t*) &arr[0],

      (uint8_t*) enc.values,
      (int*)  enc.cnt

    );

  };

// ---   *   ---   *   ---
// dirty quick cmp

  inline bool operator==(
    struct FwdPixel& other

  ) {
    return this->as_int()==other.as_int();

  };

  inline bool operator!=(
    struct FwdPixel& other

  ) {return !(*this==other);};


// ---   *   ---   *   ---

} Pixel;

// ---   *   ---   *   ---

#endif // __24_JOJ_PIXEL_H__

