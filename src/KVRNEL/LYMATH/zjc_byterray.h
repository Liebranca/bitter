#ifndef __ZJC_BYTERRAY_H__
#define __ZJC_BYTERRAY_H__

#include "../zjc_evil.h"
#include "../zjc_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct LYMTH_USTR8 {

    union {
        struct {
            uchar x:2;
            uchar y:2;
            uchar z:2;
            uchar w:2;

        };  uchar b;
    };

} ustr8; CASSERT(sizeof(ustr8)==1, "USTR8 SHOULD BE 1 BYTE");

/* 00 00  00 00 
   00 00  00 00 
               
   00 00  00 00 
   00 00  00 00 */
 
//   ---     ---     ---     ---     ---

void add(ustr8* a, ustr8 b);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif //__ZJC_BYTERRAY_H__