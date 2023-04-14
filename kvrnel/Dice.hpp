#ifndef __24_DICE_H__
#define __24_DICE_H__

// ---   *   ---   *   ---
// deps

  #include <random>
  #include "kvrnel/Style.hpp"

// ---   *   ---   *   ---
// info

class Dice {

public:

  VERSION   "v0.00.2b";
  AUTHOR    "IBN-3DILA";

  cx64 WALL=0xDEADBEEFLL;

// ---   *   ---   *   ---
// attrs

private:

  uint64_t m_max;
  uint64_t m_min;
  uint64_t m_cap;

  uint64_t m_seed;
  uint64_t m_idex;
  uint64_t m_last;

// ---   *   ---   *   ---
// iface

public:

  static Dice& fetch(uint64_t s);

  // cstruc
  Dice(

    uint64_t cap,

    uint64_t min  = 0,
    uint64_t max  = 100,
    uint64_t seed = 0xDE74

  );

  // ctrash
  Dice(void) {};

  // resets rng
  void reseed(uint64_t x);

  // get 1dS
  uint64_t next(void);

  // ^get NdS
  static uint64_t roll(
    uint64_t n,
    uint64_t s

  );

};

// ---   *   ---   *   ---

#endif // __24_DICE_H__
