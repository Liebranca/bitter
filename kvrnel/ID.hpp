#ifndef __24_ID_H__
#define __24_ID_H__

// ---   *   ---   *   ---
// deps

  #include "Style.hpp"

// ---   *   ---   *   ---
// info

class ID {

  VERSION   "v2.00.1";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---

  cx8 KEYW=28;
  cx8 SIGW=4;

  cx8 KLIMIT=KEYW-1;
  cx8 SLIMIT=SIGW-1;

//   ---     ---     ---     ---     ---
// base struct to identify otherwise wild voids

private:

  union {

    // sigil describes the block
    // key is used for hashing
    struct {
      char m_sig[SIGW];
      char m_key[KEYW];

    };

    // sigil + key
    char m_full[SIGW+KEYW];

  };

// ---   *   ---   *   ---

public:

  ID(

    const char* sig,
    const char* key

  );

// ---   *   ---   *   ---
// getters

  inline const char* as_str(void) {
    return m_full;

  };

  inline const int sigil(void) {
    return *((int*) m_sig);

  };

  inline const char* key(void) {
    return m_key;

  };

};

//   ---     ---     ---     ---     ---

#endif //__24_ID_H__
