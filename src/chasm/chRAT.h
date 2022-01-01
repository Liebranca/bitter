#ifndef __CH_RAT_H__
#define __CH_RAT_H__

#include "KVRNEL/zjc_CommonTypes.h"
#include "KVRNEL/lymath.h"

#include "SDL2/SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct CHASM_RODENT {

    ustr8 abs;
    ustr8 rel;

    uchar sens;
    char  state;

} RAT;

//   ---     ---     ---     ---     ---

void RUNWINRAT     (RAT* rat, SDL_MouseMotionEvent* motion);

void RESWINRAT     (RAT* rat, ustr8 dim);    // resets rats position
void STOPWINRAT    (RAT* rat           );    // stop rat motion
int  WALWINRAT     (RAT* rat, ustr8 dim);    // check rat against the wall

void GTWINRATMOTION(RAT* rat, float x  );   // get %based motion from last frame

int  GTWINRATIG     (RAT* rat           );  // these just set and get flags
void USTWINRATIG    (RAT* rat           );
void STWINRATIG     (RAT* rat           );
int  GTWINRATACT    (RAT* rat           );
void USTWINRATACT   (RAT* rat           );
void STWINRATACT    (RAT* rat           );

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __CH_RAT_H__