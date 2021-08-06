/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    chMANG                                *
*                                           *
*     -SDL window/input manager             *
*     -runs the loops                       *
*     -gateway to the cool stuff            *
*                                           */
// *   ---     ---     ---     ---     ---  *
/*    LIBRE SOFTWARE                        *
*                                           *
*     Licenced under GNU GPL3               *
*     be a bro and inherit                  *
*                                           */
// *   ---     ---     ---     ---     ---  *
/* CONTRIBUTORS                             *
*     lyeb,                                 *
*                                           */
/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/

#include "chMNG.h"
#include "chWIN.h"

#include "GLEW/glew.h"
#include "../KVRNEL/MEM/zjc_clock.h"

//   ---     ---     ---     ---     ---

#define CHASM_MAX_WINS 1

static SDL_GLContext ogl_context;

static CLCK*         chmang_clock = NULL;
static WIN*          curwin       = NULL;

static uint          w_width      = 640;
static uint          w_height     = 480;

static uchar         openwins;

//   ---     ---     ---     ---     ---

int   GTCHMNGRUN        (void)              { return GTWINOPEN(curwin);                     };
void  FRBEGCHMNG        (void)              { KFRBEG(); POLWIN(curwin);                     };
float FBYCHMNG          (void)              { return chmang_clock->fBy;                     };
uint  UBYCHMNG          (void)              { return chmang_clock->uBy;                     };
void  FRENDCHMNG        (void)              { KFREND();                                     };
void  SLEEPCHMNG        (void)              { if(GTSLEEP()) { SDL_Delay(GTSLTIM()); };      };
uint  GTWINWID          (void)              { return w_width;                               };
uint  GTWINHEI          (void)              { return w_height;                              };

//   ---     ---     ---     ---     ---

int NTCHMNG(char* title, int fullscreen)    {

                                            // SDL setup boilerplate
    SDL_Init                                (SDL_INIT_EVERYTHING                          );

    SDL_GL_SetAttribute                     (SDL_GL_RED_SIZE,              3              );
    SDL_GL_SetAttribute                     (SDL_GL_GREEN_SIZE,            3              );
    SDL_GL_SetAttribute                     (SDL_GL_BLUE_SIZE,             2              );

    SDL_GL_SetAttribute                     (SDL_GL_BUFFER_SIZE,          16              );
    SDL_GL_SetAttribute                     (SDL_GL_DEPTH_SIZE,           16              );
    SDL_GL_SetAttribute                     (SDL_GL_DOUBLEBUFFER,          1              );
    SDL_GL_SetAttribute                     (SDL_GL_ACCELERATED_VISUAL,    1              );

    SDL_GL_SetAttribute                     (SDL_GL_CONTEXT_MAJOR_VERSION, 4              );
    SDL_GL_SetAttribute                     (SDL_GL_CONTEXT_MINOR_VERSION, 0              );

    openwins=1;

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);

    if(fullscreen) {
        w_width  = DM.w;
        w_height = DM.h;

    };

//   ---     ---     ---     ---     ---    // make window

    curwin          = MKWIN                 (title, w_width, w_height                     );
    ogl_context     = SDL_GL_CreateContext  (curwin->window                               );

    if(fullscreen) {
        SDL_SetWindowFullscreen             (curwin->window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    };

//   ---     ---     ---     ---     ---    // init glew

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if (status != GLEW_OK) {
        CALOUT('E', "GLEW failed it's own init; something's wrong...\n");
        DLCHMNG(); return FATAL;

    };

    if (!glewIsSupported("GL_VERSION_4_0")) {
        CALOUT('E', "This application requires OpenGL v4.0\n\b");

    };

    SDL_GL_SetSwapInterval(1);

    /* joystick stuff can't be tested now
    int numjoys = SDL_NumJoysticks();

    if(numjoys > 0)
    {
        main_joy = 0;
        for(int i = 0; i < numjoys; i++)
        {
            SDL_JoystickEventState(SDL_ENABLE);
            openJoy(curwin, i); curwin->joys[i].sens = 2.5f;
        }
    }*/

//   ---     ---     ---     ---     ---
/*
    glEnable                                (GL_DEPTH_TEST                                );
    glEnable                                (GL_CULL_FACE                                 );
    glCullFace                              (GL_BACK                                      );
    glEnable                                (GL_BLEND                                     );
    glBlendFunc                             (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA         );
*/
    chmang_clock = MKCLCK                   ("|/-\\", 8, 1.0f                             );
    STCLCK                                  (chmang_clock                                 );

    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

int DLCHMNG()                               {

    SDL_GL_DeleteContext(ogl_context);
    BKWIN(curwin); DLMEM(chmang_clock);

    SDL_Quit();

    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

void GTCHMNGKPROC(uchar* mask)              { *mask = (curwin->key & *mask);                };

void GTCHMNGMVKEY(ustr2* moveaxis)          {

    moveaxis->x = 0;
    moveaxis->y = 0;

    if(curwin->key & (0x2000 + 0x8000)) {
        moveaxis->x = 255;
        if(curwin->key & 0x8000)            { moveaxis->x *= -1;                            };
    };

    if(curwin->key & (0x1000 + 0x4000)) {
        moveaxis->y = 255;
        if(curwin->key & 0x4000)            { moveaxis->y *= -1;                            };
    };                                                                                      };

//   ---     ---     ---     ---     ---