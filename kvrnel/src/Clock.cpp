// ---   *   ---   *   ---
// CLOCK
// Tick,tack
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include <cmath>

  #include <time.h>
  #include <unistd.h>

  #include "kvrnel/Clock.hpp"

// ---   *   ---   *   ---
// constructor

void Clock::nit(float fps) {

  glm::uvec4 pos;

  m_beg=clock();
  m_end=m_beg;

  float frame_time=
    (1/fps)
  * CLOCKS_PER_SEC
  ;

  m_len   = std::round(frame_time);

  m_delta = 0;
  m_scale = 1.0f;

  m_fBy   = 0.0;
  m_uBy   = 0;

  pos.x   = 0;
  pos.y   = -1;

  m_gf    = Char_Sprite(

    L"\x01A9\x01AA\x01AB\x01AC"
    L"\x01AD\x01AE\x01AF\x01B0",

    pos

  );

};

// ---   *   ---   *   ---
// shorthand

inline float get_ftime(long len) {

  return
    ((float) len)
  / CLOCKS_PER_SEC
  ;

};

// ---   *   ---   *   ---
// updates time-interval multipliers
// sleeps if needed

void Clock::tick(int busy) {

  m_beg   = m_end;
  m_beg   = clock();

  m_delta = m_beg-m_end;

  long  len   = m_len << ((! busy) * 2);
  float ftime = 0.0f;

  if(m_delta<len) {

    long sub=(len-m_delta);
    ftime=get_ftime(sub);

    usleep(sub);
    m_delta=0;

  } else {
    ftime=get_ftime(len);

  };

  m_end=m_beg;

  m_fBy=ftime * m_scale;
  m_uBy=std::round(m_fBy);

};

// ---   *   ---   *   ---
// bleeding edge graphics ;>

inline void Clock::draw(void) {
  m_gf.play();

};

// ---   *   ---   *   ---
