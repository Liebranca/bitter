#ifndef __SIN_CANVAS_H__
#define __SIN_CANVAS_H__

#include "KVRNEL/zjc_CommonTypes.h"
#include "KVRNEL/lymath.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

void NTCANVAS(void              );          // kyknit
void DLCANVAS(void              );          // kykdel

void BEGPSH  (void              );          // frbeg for char pushing
void PSHCHR  (float* t, ustr8* d);          // pushchar to screen
void ENDPSH  (ustr8* d          );          // frend for char pushing

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SIN_CANVAS_H__