#ifndef __24_SQUARE_H__
#define __24_SQUARE_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"

// ---   *   ---   *   ---
// get index of point in rect from 2D coords

inline uint64_t sq_idex(

  uint64_t x,
  uint64_t y,

  uint64_t side

) {return x+(y*side);};

// ---   *   ---   *   ---

#endif // __24_SQUARE_H__
