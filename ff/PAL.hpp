#ifndef __24_PAL_H__
#define __24_PAL_H__

// ---   *   ---   *   ---
// deps

  #include <vector>

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Bin.hpp"
  #include "kvrnel/Tab.hpp"

// ---   *   ---   *   ---
// info

class PAL: public Bin {

public:

  VERSION   "v0.00.1b";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---
// filebeg

private:

  typedef struct {

    char     sig[8];
    uint64_t elem_cnt;

  } Header;

// ---   *   ---   *   ---
// attrs

  Header m_hed;
  Symtab m_tab;

  std::vector<TAB::Symbol*> m_values;

// ---   *   ---   *   ---
// virtual constants

  vicstr m_fsig(void) {
    return "%PAL\0\0\0\0";

  };

  vic64 m_header_sz(void) {
    return sizeof(Header);

  };

  VIC void* get_header(void) {
    return &m_hed;

  };

// ---   *   ---   *   ---
// internals

  void encode(void);
  void decode(void);

  void from_buff(uint8_t* buff_p);

// ---   *   ---   *   ---
// iface

public:

  PAL(std::string fpath,char mode=Bin::READ);
  void write(void);

};

// ---   *   ---   *   ---

#endif // __24_PAL_H__
