#ifndef __CH_KEY_H__
#define __CH_KEY_H__

#include "KVRNEL/zjc_CommonTypes.h"
#include "SDL2/SDL.h"

#ifdef __cplusplus
extern"C" {

#endif

/*uint CH_KEYB_KCODES[]={

  SDLK_LALT,SDLK_LCTRL,SDLK_LSHIFT,
  SDLK_RALT,SDLK_RCTRL,SDLK_RSHIFT,
  

  SDLK_CAPSLOCK,SDLK_INSERT,SDLK_SCROLLLOCK

};*/

typedef struct {

  uint lv_keys;             // flip on held
  uint tg_keys;             // flip on up

} KEYB;

//   ---     ---     ---     ---     ---
// #:0x1;>

// set key state
void STWINKEY(uint* k,uint input);

// clear key state
void CLWINKEY(uint* k,uint input);

// get key state
int GTWINKEY(uint* k,uint value);

//   ---     ---     ---     ---     ---
// #:0x2;>


#ifdef __cplusplus
}
#endif

#endif                      // __CH_KEY_
