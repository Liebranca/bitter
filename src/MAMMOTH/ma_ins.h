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

void lmcpy (void         );                 // copy b to a
void lmmov (void         );                 // b to a, then xor b
void lmwap (void         );                 // swap a and b
void lmwed (void         );                 // set typing rules

void lmjmp (void         );                 // jump to label
void lmjif (void         );                 // jump if
void lmeif (void         );                 // jump if not
void lmexit(void         );                 // exit program

void lmadd (void         );                 // add b to a
void lmsub (void         );                 // sub b from a
void lminc (void         );                 // a++
void lmdec (void         );                 // a--

void lmasl (uint* udr    );                 // arithmetics solver

//   ---     ---     ---     ---     ---

void swcpy (void         );                 // flip switches for instruction sw***
void swmov (void         );
void swwap (void         );
void swwed (void         );

void swjmp (void         );
void swjif (void         );
void sweif (void         );
void swexit(void         );

void swadd (void         );
void swsub (void         );
void swinc (void         );
void swdec (void         );

//   ---     ---     ---     ---     ---

void stentry(void        );                 // set program entry point

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __MA_INS_H__