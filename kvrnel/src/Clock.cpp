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
// updates time-interval multipliers
// sleeps if needed

void Clock::tick(int busy) {

  m_beg=m_end;
  m_beg=clock();

  m_delta=m_beg-m_end;

  if(m_delta<m_len) {

    long  sub=m_len-m_delta;

    float frame_time=
      ((float) sub)
    / CLOCKS_PER_SEC
    ;

    m_fBy=(1.0f/frame_time)*m_scale;

    usleep(sub);
    m_delta=0;

  } else {
    m_fBy=m_scale;

  };

  m_uBy=std::round(m_fBy);

};

// ---   *   ---   *   ---
// bleeding edge graphics ;>

inline void Clock::draw(void) {
  m_gf.play();

};

// ---   *   ---   *   ---
