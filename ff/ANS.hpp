#ifndef __24_ANS_H__
#define __24_ANS_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"

// ---   *   ---   *   ---
// info

class ANS {

public:

  VERSION   "v0.00.1b";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---
// helpers

  struct Anim {

    uint16_t beg;
    uint16_t end;

    uint32_t flags;

  };

  typedef std::vector<Anim> Anims;

// ---   *   ---   *   ---
// attrs

private:

  Anims m_anims;

// ---   *   ---   *   ---
// iface

public:

  // ctrash
  ANS(void) {};
  ~ANS(void) {};

  // cstruc
  ANS(std::string fpath);

  // getter
  Anims& data(void) {
    return m_anims;

  };

};

// ---   *   ---   *   ---

#endif // __24_ANS_H__
