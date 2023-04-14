#ifndef __24_CLOCK_H__
#define __24_CLOCK_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Char_Sprite.hpp"

// ---   *   ---   *   ---
// info

class Clock {

private:

  VERSION   "v2.00.4";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---
// attrs

  long    m_beg;
  long    m_end;

  long    m_len;
  long    m_delta;

  float   m_scale;

  float   m_fBy;
  long    m_uBy;

  Char_Sprite m_gf;

// ---   *   ---   *   ---
// iface

public:

  // ctrash
  Clock(void) {};

  inline Clock(float fps) {
    this->nit(fps);

  };

  void nit(float fps);
  void tick(int busy);

  inline void draw(void);

// ---   *   ---   *   ---
// getters

  inline float fBy(void) {
    return m_fBy;

  };

  inline long uBy(void) {
    return m_uBy;

  };

};

// ---   *   ---   *   ---

#endif // __24_CLOCK_H__
