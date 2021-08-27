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

#include "KVRNEL/MEM/zjc_clock.h"
#include "SIN/sin_globals.h"

//   ---     ---     ---     ---     ---

#define CHASM_MAX_WINS 1

static SDL_GLContext ogl_context;

static CLCK*         chmang_clock    = NULL;
static WIN*          curwin          = NULL;

static uchar         openwins;

static float         ambientColor[4] = { 22.0f/255.0f, 30.0f/255.0f, 35.0f/255.0f, 1.0f };
static float         ambientMult     = 1.0f;

//   ---     ---     ---     ---     ---

int   GTCHMNGRUN        (void)              { return GTWINOPEN(curwin);                     };

int   FRBEGCHMNG        (void)              { KFRBEG(); POLWIN(curwin);

    if(SIN_EVILSTATE) {
        CALOUT(S, "SIN exception -- shutting down; check the logs");
        return FATAL;

    };

    SWPWINBUF(curwin);

    glBindFramebuffer                       (GL_FRAMEBUFFER, 0                              );

    glClearColor                            ( ambientColor[0] * ambientMult, \
                                              ambientColor[1] * ambientMult, \
                                              ambientColor[2] * ambientMult, \
                                              ambientColor[3] * ambientMult  );

    glClear                                 (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return DONE;                                                                            };

float FBYCHMNG          (void)              { return chmang_clock->fBy;                     };
uint  UBYCHMNG          (void)              { return chmang_clock->uBy;                     };
void  FRENDCHMNG        (void)              { KFREND();                                     };
void  SLEEPCHMNG        (void)              { if(GTSLEEP()) { SDL_Delay(GTSLTIM()); };      };
uint  GTWINWID          (void)              { return SIN_WSIZX;                             };
uint  GTWINHEI          (void)              { return SIN_WSIZY;                             };

//   ---     ---     ---     ---     ---

int NTCHMNG(char* title, int fullscreen)    {

                                            // SDL setup boilerplate
    SDL_GL_SetAttribute                     (SDL_GL_RED_SIZE,              8              );
    SDL_GL_SetAttribute                     (SDL_GL_GREEN_SIZE,            8              );
    SDL_GL_SetAttribute                     (SDL_GL_BLUE_SIZE,             8              );
    SDL_GL_SetAttribute                     (SDL_GL_ALPHA_SIZE,            8              );

    SDL_GL_SetAttribute                     (SDL_GL_BUFFER_SIZE,          16              );
    SDL_GL_SetAttribute                     (SDL_GL_DEPTH_SIZE,           16              );
    SDL_GL_SetAttribute                     (SDL_GL_DOUBLEBUFFER,          1              );
    SDL_GL_SetAttribute                     (SDL_GL_ACCELERATED_VISUAL,    1              );

    SDL_GL_SetAttribute                     (SDL_GL_CONTEXT_MAJOR_VERSION, 4              );
    SDL_GL_SetAttribute                     (SDL_GL_CONTEXT_MINOR_VERSION, 6              );

    SDL_GL_SetAttribute                     (SDL_GL_CONTEXT_PROFILE_MASK                  ,
                                             SDL_GL_CONTEXT_PROFILE_CORE                  );

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {

        SDL_Quit(); CALOUT(K, "SDL INIT FAILED\n");
        return FATAL;

    };

    openwins=1;

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);

    if(fullscreen) {
        SIN_WSIZX = DM.w;
        SIN_WSIZY = DM.h;
    
    };

//   ---     ---     ---     ---     ---    // make window

    curwin          = MKWIN                 (title, SIN_WSIZY, SIN_WSIZX                  );
    ogl_context     = SDL_GL_CreateContext  (curwin->window                               );
    gladLoadGLLoader                        ((GLADloadproc)SDL_GL_GetProcAddress          );

    //SDL_MaximizeWindow(curwin->window);
    //SDL_SetWindowOpacity(curwin->window, 0.0f);
    SDL_GL_SetSwapInterval(1);

//   ---     ---     ---     ---     ---

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

    glEnable                                (GL_ALPHA_TEST                                );
    glEnable                                (GL_DEPTH_TEST                                );
    glEnable                                (GL_CULL_FACE                                 );
    glCullFace                              (GL_BACK                                      );
    glEnable                                (GL_BLEND                                     );
    glBlendFunc                             (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA         );

    chmang_clock = MKCLCK                   (".*^~", 8, 1.0f                              );
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

void GTCHMNGMVKEY(ustr8* moveaxis)          {

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