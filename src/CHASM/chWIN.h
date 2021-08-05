#ifndef __CH_WIN_H__
#define __CH_WIN_H__

#include <assert.h>

#include "../KVRNEL/MEM/kvr_mem.h"
#include "../KVRNEL/lymath.h"

#include "chRAT.h"
#include "chKEY.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct CHASM_WINDOW {               // sdl window stuff

    MEM           mem;                      // mem header

    char          key;                      // cool custom keyboard shit
    RAT           rat;                      // cool custom mouse shit

    SDL_Window*   window;                   // handle to the window
    SDL_GLContext context;                  // opengl context for this window

    ustr2         size;                     // screen size
    ustr2         hsize;                    // guides relative to screen size

    int           flags;                    // 0x01 is open
                                            // 0x02 has focus
                                            // 0x04 is on top

} WIN;

//   ---     ---     ---     ---     ---

WIN* MKWIN(char* title,             \
           int height, int width    );      // makes new window




//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __CH_WIN_H__