#ifndef __SH_LEGACY_H__
#define __SH_LEGACY_H__

#include "KVRNEL/zjc_evil.h"
#include "KVRNEL/zjc_CommonTypes.h"
#include "KVRNEL/TYPES/zjc_id.h"
#include "KVRNEL/lymath.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---
// #:0x0;>

typedef struct {            // basic hashable fun
                            // ptr with
                            // descriptor

  ID id;                    // key/lookup token

  ustr8 data;               // some bytes
                            // describing the
                            // call

  STARK fun;                // ptr to the actual
                            // operation

} PEOP;

//   ---     ---     ---     ---     ---
// #:0x1;>


void NTPEOP(uint idex,
char* opcode,
uint data,
STARK fun);                 // insert fun ptr
                            // into op array

PEOP* GTPENOP(void);        // return null
                            // operator

void GTOP(PEOP** dst,
char* key);                 // fetch op matching
                            // key

//   ---     ---     ---     ---     ---
// #:0x2;>

typedef struct {            // animated char
                            // sequence

  char* buff;               // characters to draw
                            // each frame

  char frame[16];           // draw buffer
  float current;            // current frame
  uint length;              // length of buff

  ustr8 co;                 // position in
                            // chargrid

} CHRSPRT;

//   ---     ---     ---     ---     ---
// #:0x3;>


CHRSPRT MKCHRSPRT(

char* buff,
uint co);                   // make char sprite
                            // from a buffer

void PLCHRSPRT(

CHRSPRT* sprite,
float dirn);                // get next frame for
                            // animation

//   ---     ---     ---     ---     ---
// #:0x4;>


#define SH_SPRITE_SLOTS 8

typedef struct {            // basic str container

  float t[4];               // transform; scale &
                            // startof in screen
                            // coords

  uint chrsz;               // base charsize
                            // multiplier

                            // fixed-size array
                            // for animated char
                            // sequences

  CHRSPRT sprites[SH_SPRITE_SLOTS];

} CHRBUFF;

//   ---     ---     ---     ---     ---
// #:0x5;>


void NTKVR(uint chrsz);     // init console
void DLKVR();               // del & restore

int kbhit(void);            // get n chars
                            // waiting on input
                            // buff

void getch(void);           // read input buff

//   ---     ---     ---     ---     ---
// #:0x6;>


float* GTKVRCURT(void);     // get cursor
                            // transform

float* GTKVRCHRT(void);     // get next char
                            // transform

ustr8* GTKVRCHRD(void);     // get next char data

//   ---     ---     ---     ---     ---
// #:0x7;>


#ifdef __cplusplus
}
#endif

#endif // __SH_LEGACY_H__