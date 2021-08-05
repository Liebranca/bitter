#ifndef __LYMATH_H__
#define __LYMATH_H__

#include "zjc_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct LY_UNSIG_STR2 {

    union {

        uchar x;
        uchar y;

    };  uchar v;

} ustr2;

//   ---     ---     ---     ---     ---

void SCUSTR2(ustr2* us, ustr2 dim, float x);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __LYMATH_H__