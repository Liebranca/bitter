#ifndef __SIN_CANVAS_H__
#define __SIN_CANVAS_H__

#include "KVRNEL/zjc_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

void NTCANVAS(void       );                 // kyknit
void DRCANVAS(uint texloc);                 // draw rect
void DLCANVAS(void       );                 // kykdel

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SIN_CANVAS_H__