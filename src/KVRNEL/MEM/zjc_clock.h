#ifndef __ZJC_CLOCK_H__
#define __ZJC_CLOCK_H__

#include "kvr_mem.h"

// in case we want alt codes for clock draws
// #include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct ZJC_CLOCK {                  // simplistic, hand-made clock

    MEM*    m;                              // mem header

    uint    frbeg;                          // marks beggining of clock frame
    uint    frend;                          // marks end of clock frame

    uint    frdlt;                          // delta between (end-beg)
    uint    frtim;                          // minimum time allowed per frame
    uint    sltim;                          // non-zero if the clock must sleep
    float   sctim;                          // timescale

    float   fBy;                            // delta mult
    uint    uBy;                            // delta mult

    int     frcap;                          // per-second framecap
    int     frcnt;                          // frame-count

    int     flags;                          // 0x01: on/off
                                            // 0x02: rested last frame
                                            // 0x04: blank*6

    char    dinfo[12];                      // draw info for clock
                                            // chars 0-8 are the actual chars to show
                                            // 9 is terminator, 10-11 are screen yx pos 

} CLCK;

//   ---     ---     ---     ---     ---

CLCK*  MKCLCK              (char* chars,  \
                            uint frcap,   \
                            float sctim   );// allocs a new clock

void   CLCKWIN             (uint w        );// sleeps for a while

void   CALCFRDLT           (void          );// computes clock framedelta
float  GTSCTIM             (void          );// gets clock timescale
void   STSCTIM             (float sctim   );// sets clock timescale
void   CALCFRTIM           (void          );// computes clock frametime
void   STFRCAP             (int frcap     );// set clock framecap

int    GTSLEEP             (void          );// gets wheter sleep is needed
void   CALCSLTIM           (void          );// computes sleeptime
uint   GTSLTIM             (void          );// gets sleeptime
void   GTFBY               (void          );// return 1.0f time clock framedelta
void   STCLCK              (CLCK* clck    );// set current clock
float  GTFRDLT             (void          );// gets framedelta

void   KFRBEG              (void          );// clock punch in
void   KFREND              (void          );// clock punch out

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ZJC_CLOCK_H__