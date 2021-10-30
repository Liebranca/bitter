#ifndef __MA_INS_H__
#define __MA_INS_H__

#include "KVRNEL/zjc_CommonTypes.h"
#include "ma_boxes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

void ldins (uint loc     );                 // load instruction and run it
void lmpush(uint loc     );                 // queue instruction
void lmpop (void         );                 // exec next instruction in queue

void lmcpy (void         );                 // copy x to y
void lmmov (void         );                 // x to y, xor x
void lmwap (void         );                 // swap x and y

void lmjmp (void         );                 // jump to label
void lmjif (void         );                 // jump if

void lmexit(void         );                 // exit program

void lmasl (uint* udr    );                 // arithmetics solver

//   ---     ---     ---     ---     ---

void swcpy (void         );                 // flip switches for instruction sw***
void swmov (void         );
void swwap (void         );

void swjmp (void         );
void swjif (void         );

void swexit(void         );

//   ---     ---     ---     ---     ---

void stentry(void        );                 // set program entry point

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __MA_INS_H__