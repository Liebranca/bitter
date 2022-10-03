#ifndef __SIN_H__
#define __SIN_H__

#ifdef __cplusplus
extern"C" {
#endif

#include "KVRNEL/zjc_CommonTypes.h"

//   ---     ---     ---     ---     ---
// #:0x0;>

float* GTPAL(int n);        // fetch N color

void NTSIN(int chrsz);      // kyknit
void DLSIN(void);           // kykdel


//   ---     ---     ---     ---     ---
// #:0x1;>



void GTCHRGRD(uint chrsz);  // gets dimentions of
                            // chargrid for new
                            // screen size



// calculates percent of screen used by square
void GTCHRSZ(float* dst);

// calculates how many chars fit in screen
void GTSCRSZ(uint* dst);

//   ---     ---     ---     ---     ---
// #:0x2;>



#ifdef __cplusplus
}
#endif

#endif // __SIN_H__
