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

  cx8 KEYW=20;
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
      char sig[SIGW];
      char key[KEYW];

    };

    // sigil + key
    char full[SIGW+KEYW];

  };

// ---   *   ---   *   ---

public:

  ID(

    const char* sig,
    const char* key

  );

  const char* gfull(void) {return this->full;};

};

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif //__ZJC_ID_H__
