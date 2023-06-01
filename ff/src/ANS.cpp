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
// cstruc

void ANS::nit(std::string fpath) {

  auto     m   = Bin::orc(fpath);

  uint64_t i   = 0;
  uint16_t cnt = m[i];i+=2;

  m_dict.nit(cnt);

  for(uint64_t j=0;j<cnt;j++) {

    Anim anim;

    std::string tag    = "";
    uint8_t     tag_sz = m[i++];

    for(uint8_t c=0;c<tag_sz;c++) {
      tag+=m[i++];

    };

    anim.beg=*((uint16_t*) &m[i]);i+=2;
    anim.end=*((uint16_t*) &m[i]);i+=4;

    m_anims.push_back(anim);
    m_dict.push(tag,j);

  };

};

// ---   *   ---   *   ---
// ^used for blanks

void ANS::enit(void) {

  m_dict.nit(1);

  Anim anim={

    .beg=0,
    .end=1

  };

  std::string tag  = "^";
  uint64_t    idex = 0;

  m_anims.push_back(anim);
  m_dict.push(tag,idex);

};

// ---   *   ---   *   ---
