#ifndef __24_CHAR_SPRITE_H__
#define __24_CHAR_SPRITE_H__

// ---   *   ---   *   ---
// deps

  #include <cwchar>
  #include "kvrnel/GLM.hpp"

// ---   *   ---   *   ---

class Char_Sprite {

private:

  vec4            m_co;

  short           m_len;
  short           m_frame;

  const wchar_t*  m_anim;

// ---   *   ---   *   ---

public:

  // compiler trash
  Char_Sprite() {};

  // constructor
  Char_Sprite(
    const wchar_t* anim,
    uvec4& co

  );

  // display next frame
  void play(char speed=1);

  // debug print
  void prich(void);

};

// ---   *   ---   *   ---

#endif // __24_CHAR_SPRITE_H__
