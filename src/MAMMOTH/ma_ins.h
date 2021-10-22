#ifndef __MA_INS_H__
#define __MA_INS_H__

#include "KVRNEL/zjc_CommonTypes.h"
#include "ma_boxes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

void lmpush(uint loc     );                 // queue instruction
void lmpop (void         );                 // exec next instruction in queue
void lmcpy (void         );                 // copy x to y
void lmmov (void         );                 // x to y, xor x
void lmwap (void         );                 // swap x and y

void lmasl (uint* udr    );                 // arithmetics solver

//   ---     ---     ---     ---     ---

void swcpy (void         );                 // flip switches for instruction sw***
void swmov (void         );
void swwap (void         );

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __MA_INS_H__