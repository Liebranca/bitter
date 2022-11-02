#ifndef __24_ID_H__
#define __24_ID_H__

// ---   *   ---   *   ---
// deps

  #include "Style.hpp"

// ---   *   ---   *   ---
// info

class ID {

  VERSION   "v2.00.2";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---

  cx8 KEYW   = 28;
  cx8 SIGW   = 4;

  cx8 KLIMIT = KEYW-1;
  cx8 SLIMIT = SIGW-1;

//   ---     ---     ---     ---     ---
// base struct to identify otherwise wild voids

private:

  // block type
  std::string m_sig;

  // block name
  std::string m_key;

// ---   *   ---   *   ---

public:

  // compiler trash
  ID() {};

  // constructor
  ID(
    const std::string sig,
    const std::string key

  );

  // ^for generic ones
  ID(
    const std::string sig,
    uint64_t          n

  );

// ---   *   ---   *   ---
// getters

  inline std::string as_str(void) {
    return m_sig+m_key;

  };

  inline int sigil(void) {
    return *((int*) m_sig.c_str());

  };

  inline std::string key(void) {
    return m_key;

  };

};

//   ---     ---     ---     ---     ---

#endif //__24_ID_H__
