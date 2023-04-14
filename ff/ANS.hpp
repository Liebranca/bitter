#ifndef __24_ANS_H__
#define __24_ANS_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Tab.hpp"

// ---   *   ---   *   ---
// info

class ANS {

public:

  VERSION   "v0.00.2b";
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
  Itab  m_dict;

// ---   *   ---   *   ---
// iface

public:

  // ctrash
  ANS(void) {};
  ~ANS(void) {};

  // cstruc
  void nit(std::string fpath);

  // selfex
  inline Anim& get(uint64_t i) {
    return m_anims[i];

  };

  // ^by name
  inline Anim& get(std::string name) {
    auto idex=m_dict.get(name);
    return get(idex);

  };

  inline uint64_t iof(std::string name) {
    auto idex=m_dict.get(name);
    return idex;

  };

};

// ---   *   ---   *   ---

#endif // __24_ANS_H__
