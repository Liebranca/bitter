#ifndef __24_SPRITE_H__
#define __24_SPRITE_H__

// ---   *   ---   *   ---
// deps

  #include <cwchar>
  #include "Vec.hpp"

// ---   *   ---   *   ---

class Sprite {

private:

  Vec<short,2>    m_co;

  short           m_len;
  short           m_frame;

  const wchar_t*  m_anim;

// ---   *   ---   *   ---

public:

  // compiler trash
  Sprite() {};

  // constructor
  Sprite(
    const wchar_t* anim,
    Vec<short,2> co

  );

  // display next frame
  void play(char speed=1);

  // debug print
  void prich(void);

};

// ---   *   ---   *   ---

#endif // __24_SPRITE_H__
