// ---   *   ---   *   ---
// ANS
// Action metadata
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Bin.hpp"
  #include "ff/ANS.hpp"

// ---   *   ---   *   ---

ANS::ANS(std::string fpath) {

  auto     m = Bin::orc(fpath);
  uint64_t i = 0;

  while(i < m.used()) {

    Anim anim;

    uint8_t tag_sz=m[i++];i+=tag_sz;

    anim.beg=*((uint16_t*) &m[i]);i+=2;
    anim.end=*((uint16_t*) &m[i]);i+=4;

    m_anims.push_back(anim);

  };

};

// ---   *   ---   *   ---
