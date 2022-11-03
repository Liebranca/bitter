#ifndef __24_SQUARE_H__
#define __24_SQUARE_H__

// ---   *   ---   *   ---
// deps

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

#endif // __24_SQUARE_H__
