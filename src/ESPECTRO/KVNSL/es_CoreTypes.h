#ifndef __ES_CORETYPES_H__
#define __ES_CORETYPES_H__

#include "KVRNEL/zjc_evil.h"
#include "KVRNEL/zjc_CommonTypes.h"
#include "KVRNEL/TYPES/zjc_id.h"
#include "KVRNEL/lymath.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct PESO_OPERATION {             // basic hashable fun ptr with descriptor

    ID    id;                               // key/lookup token
    ustr8 data;                             // some bytes describing the call
    STARK fun;                              // ptr to the actual operation

} PEOP;

//   ---     ---     ---     ---     ---

void  NTPEOP (uint  idex  ,
              char* opcode,
              uint  data  ,
              STARK fun   );                // insert fun ptr into op array

PEOP* GTPENOP(void        );                // return null operator

void  GTOP   (PEOP** dst  ,
              char*  key  );                // fetch op matching key

//   ---     ---     ---     ---     ---

typedef struct PESO_CHARSPRITE {            // animated char sequence

    char* buff;                             // characters to draw each frame
    int   current;                          // current frame
    uint  length;                           // length of buff

} CHRSPRT;

CHRSPRT MKCHRSPRT(char* buff     );         // make char sprite from a buffer

char*   PLCHRSPRT(CHRSPRT* sprite,
                  int dirn       );         // get current frame for animation

//   ---     ---     ---     ---     ---

void NTKVR (uint chrsz  );                  // init console
void DLKVR (            );                  // del & restore

int  kbhit (void        );                  // get n chars waiting on input buff
void getch (void        );                  // read input buff

//   ---     ---     ---     ---     ---

float* GTKVRCURT(void);                     // get cursor transform
float* GTKVRCHRT(void);                     // get next char transform
ustr8* GTKVRCHRD(void);                     // get next char data

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ES_CORETYPES_H__