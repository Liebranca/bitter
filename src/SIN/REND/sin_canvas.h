#ifndef __SIN_CANVAS_H__
#define __SIN_CANVAS_H__

#include "KVRNEL/zjc_CommonTypes.h"
#include "KVRNEL/lymath.h"

#ifdef __cplusplus
extern"C" {

#endif

//   ---     ---     ---     ---     ---
// #:0x0;>


void NTCANVAS();            // kyknit
void DLCANVAS(void);        // kykdel

void BEGPSH(void);          // frbeg for char
                            // pushing

// pushchar to screen
void PSHCHR(float* t,ustr8* d);

// frend for char pushing
void ENDPSH(float* t,ustr8* d);

//   ---     ---     ---     ---     ---
// #:0x1;>


#ifdef __cplusplus
}
#endif

#endif // __SIN_CANVAS_H__