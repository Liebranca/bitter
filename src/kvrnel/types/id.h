#ifndef __ZJC_ID_H__
#define __ZJC_ID_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include "../zjc_CommonTypes.h"

#include <stdint.h>

#define cx8 static constexpr const uint8_t

#define VERSION static constexpr const char* VERSION=
#define AUTHOR static constexpr const char* AUTHOR=

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

  const char* as_str(void) {
    return m_full;

  };

  const uint32_t sigil(void) {
    return *((uint32_t*) m_sig);

  };

  const char* key(void) {
    return m_key;

  };

};

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif //__ZJC_ID_H__
