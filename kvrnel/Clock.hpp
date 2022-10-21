#ifndef __24_CLOCK_H__
#define __24_CLOCK_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Sprite.hpp"

// ---   *   ---   *   ---
// info

class Clock {

private:

  VERSION   "v2.00.1";
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

  Sprite  m_gf;

// ---   *   ---   *   ---

public:

  Clock(float fps);
  void tick(int busy);

  inline void draw(void);

};

// ---   *   ---   *   ---

#endif // __24_CLOCK_H__
