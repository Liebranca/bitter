#ifndef __24_SQUARE_H__
#define __24_SQUARE_H__

// ---   *   ---   *   ---
// deps

  #include <memory>

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Bytes.hpp"

// ---   *   ---   *   ---
// get index of point in rect from 2D coords

inline uint64_t sq_idex(

  uint16_t x,
  uint16_t y,

  uint64_t side

) {return x+(y*side);};

// ---   *   ---   *   ---
// ^get 2D coords from index

inline uint32_t rsq_idex(
  uint64_t i,
  uint64_t side

) {

  uint16_t n=bsf(side);

  uint16_t y=i>>n;
  uint16_t x=i-(y<<n);

  return x | (uint32_t(y)<<16);

};

// ---   *   ---   *   ---

std::unique_ptr<void*> sq_rows(
  void*    buff,

  uint64_t sz,
  uint64_t step

) {

  std::unique_ptr<void*> out(
    new void*[sz]

  );

  void**   out_p = out.get();
  uint8_t* row   = (uint8_t*) buff;

  for(uint64_t i=0;i<sz;i++) {
    out_p[i]=row;
    row+=sz*step;

  };

  return out;

};

// ---   *   ---   *   ---

#endif // __24_SQUARE_H__
