#ifndef __CH_KEY_H__
#define __CH_KEY_H__

#include "zjc_CommonTypes.h"
#include "SDL/SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

static int CH_KEYB_KCODES[8] =              { SDLK_LEFT, SDLK_RIGHT, SDLK_DOWN, SDLK_UP,    \
                                              SDLK_q,    SDLK_w,     SDLK_e,    SDLK_ESCAPE };

//   ---     ---     ---     ---     ---

void STWINKEY(uchar* k, uchar input);       // set key state
void CLWINKEY(uchar* k, uchar input);       // clear key state
int  GTWINKEY(uchar* k, uchar value);       // get key state

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __CH_KEY_H__