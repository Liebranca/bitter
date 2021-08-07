#ifndef __LYMATH_H__
#define __LYMATH_H__

#include "zjc_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct LY_UNSIG_STR4 {

    

};

//   ---     ---     ---     ---     ---

typedef struct LY_UNSIG_STR16 {

    union {

        uchar  x;
        uchar  y;

    };  ushort v;

} ustr16;

//   ---     ---     ---     ---     ---

void SCUSTR16(ustr16* us, ustr16 dim, float x);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __LYMATH_H__