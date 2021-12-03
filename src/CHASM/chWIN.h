
#ifndef __CH_WIN_H__
#define __CH_WIN_H__

#include <assert.h>

#include "KVRNEL/MEM/kvr_mem.h"
#include "KVRNEL/lymath.h"

#include "chRAT.h"
#include "chKEY.h"

#ifdef __cplusplus
extern"C" {

#endif

//   ---     ---     ---     ---     ---
// #:0x0;>


typedef struct CHASM_WINDOW {
// sdl window stuff

ID id;                      // pol header

char key;                   // cool custom
                            // keyboard shit
RAT rat;                    // cool custom mouse
                            // shit

SDL_Window* window;         // handle to the
                            // window
SDL_GLContext context;      // opengl context for
                            // this window

ustr8 size;                 // screen size
ustr8 hsize;                // guides relative to
                            // screen size

int flags;                  // 0x01 is open
// 0x02 has focus
// 0x04 is on top

} WIN;

//   ---     ---     ---     ---     ---
// #:0x1;>


WIN* MKWIN(
  char* title,int height,
  int width,int fullscreen);// makes new window

int BKWIN(WIN* win);        // breaks the window

int POLWIN(WIN* win);       // polls the windows
                            // for input events

int GTWINOPEN(WIN* win);    // check if the
                            // window is open

void SWPWINBUF(WIN* win);

//   ---     ---     ---     ---     ---
// #:0x2;>


#ifdef __cplusplus
}
#endif

#endif                      // __CH_WIN_
