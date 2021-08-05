#ifndef __CH_RAT_H__
#define __CH_RAT_H__

#include "../KVRNEL/zjc_CommonTypes.h"
#include "../KVRNEL/lymath.h"

#include "SDL/SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct CHASM_RODENT {

    ustr2 abs;
    ustr2 rel;

    char  sens;
    char  state;

} RAT;

//   ---     ---     ---     ---     ---

void  RUNWINRAT     (RAT* rat, SDL_MouseMotionEvent* motion);

void  RESWINRAT     (RAT* rat, ustr2 dim);  // resets rats position
void  STOPWINRAT    (RAT* rat           );  // stop rat motion
int   WALWINRAT     (RAT* rat, ustr2 dim);  // check rat against the wall

ustr2 GTWINRATMOTION(RAT* rat, ustr2 dim,
                     float x            );  // get %based motion from last frame

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