// ---   *   ---   *   ---
// SPRITE
// Retro motion
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include <cstring>
  #include <iostream>

  #include "kvrnel/Sprite.hpp"

// ---   *   ---   *   ---
// constructor

Sprite::Sprite(
  const wchar_t* anim,
  Vec<short,2> co

) {

  m_anim=anim;
  m_len=wcslen(anim);
  m_frame=0;
  m_co=co;

};

// ---   *   ---   *   ---
// display next frame

void Sprite::play(char speed) {

  // position the cursor
  std::wcout

  << "\e["
  << m_co[1]+1
  << ';'
  << m_co[0]+1

  << 'H'

  // out this frame
  << m_anim[m_frame]

  ;

// ---   *   ---   *   ---
// advance and cap

  if(speed<0) {

    m_frame=(m_frame+speed >= 0)
      ? m_frame+speed
      : m_len-1
      ;

  } else {

    m_frame=(m_frame+speed < m_len)
      ? m_frame+speed
      : 0
      ;

  };

};

// ---   *   ---   *   ---
// debug print

void Sprite::prich(void) {

  std::wcout << m_anim << std::endl;

};

// ---   *   ---   *   ---

