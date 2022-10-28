#ifndef __24_JOY_H__
#define __24_JOY_H__

// ---   *   ---   *   ---
// deps

  #include <vector>
  #include <linux/joystick.h>

  #include "bitter/kvrnel/Style.hpp"

// ---   *   ---   *   ---
// defs

namespace PSX {

  cx8 TRIANGLE = 0x0;
  cx8 CIRCLE   = 0x1;
  cx8 CROSS    = 0x2;
  cx8 SQUARE   = 0x3;

  cx8 R1       = 0x7;
  cx8 R2       = 0x5;
  cx8 R3       = 0xB;

  cx8 L1       = 0x6;
  cx8 L2       = 0x4;
  cx8 L3       = 0xA;

  cx8 SELECT   = 0x8;
  cx8 START    = 0x9;

  cx8 VIEW_X   = 0x0;
  cx8 VIEW_Y   = 0x1;

  cx8 MOVE_X   = 0x3;
  cx8 MOVE_Y   = 0x2;

};

// ---   *   ---   *   ---
// info

class Joystick {

public:

  VERSION   "v0.00.1b";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---
// helpers

private:

  cx16 AXIS_MAX=(1<<15)-1;
  typedef struct js_event Event;

// ---   *   ---   *   ---
// attrs

  int   m_fd;

  int   m_bt_cnt;
  int   m_ax_cnt;

  std::vector<float> m_ax;
  uint64_t           m_bt;

  Event m_ev;

// ---   *   ---   *   ---
// internals

  int next_event(void);

  void ax_event(void);
  void bt_event(void);

// ---   *   ---   *   ---
// iface

public:

  Joystick(int id);
  ~Joystick(void);

  void get_input(void);

  inline bool miss(void) {
    return m_fd<0;

  };

  inline bool button(const uint8_t n) {
    return (m_bt>>n)&1;

  };

  inline float axis(const uint8_t n) {
    return m_ax[n];

  };

};

// ---   *   ---   *   ---

#endif // __24_JOY_H__
