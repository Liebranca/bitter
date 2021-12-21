//**************************
// CHMANG                  *
// SDL window manager      *
//                         *
// LIBRE SOFTWARE          *
// Licenced under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************



#include "chMNG.h"
#include "chWIN.h"

#include "KVRNEL/MEM/zjc_clock.h"

#include "SIN/sin_globals.h"
#include "SIN/__sin.h"

//   ---     ---     ---     ---     ---
// #:0x0;>


#define CHASM_MAX_WINS 1
static SDL_GLContext ogl_context;

static CLCK* chmang_clock=NULL;
static WIN* curwin=NULL;

static uchar openwins;
static uint busy=0;

static float ambientColor[4]={
  0.0f,0.0f,32.0f/255.0f,1.0f

};static float ambientMult=1.0f;

//   ---     ---     ---     ---     ---
// #:0x1;>


int GTRUN(void) {
  return GTWINOPEN(curwin);

};

int FRBEG(int update) {

  if(SIN_EVILSTATE) {

    CALOUT(S,
      "SIN exception -- shutting down;"
      "check the logs"

    );return FATAL;

  };

  KFRBEG();

  uint busy_last=busy;
  busy+=(POLWIN(curwin)+update)*(busy<60);

  if(busy && !busy_last) {
    STFRCAP(60);busy+=30;

  } else if(busy && busy_last) {
    busy--;

    if(!busy) {
      STFRCAP(4);

    };
  };

  SWPWINBUF(curwin);

  glBindFramebuffer(GL_FRAMEBUFFER,0);

  glClearColor(
    ambientColor[0] * ambientMult,
    ambientColor[1] * ambientMult,
    ambientColor[2] * ambientMult,
    ambientColor[3] * ambientMult

  );glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  return DONE;

};

float FBY(void) {
  return chmang_clock->fBy;

};

uint UBY(void) {
  return chmang_clock->uBy;

};

void FREND(void) { KFREND();};

void FRSLP(void) {
  if(GTSLEEP()) {SDL_Delay(GTSLTIM());};

};

uint GTWINWID(void) { return SIN_WSIZX;};
uint GTWINHEI(void) { return SIN_WSIZY;};

//   ---     ---     ---     ---     ---
// #:0x2;>


int NTCHMNG(char* title,int fullscreen) {


  // SDL setup boilerplate
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,4);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,4);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,4);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,4);

  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,16);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,6);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
  SDL_GL_CONTEXT_PROFILE_CORE);

  if(SDL_Init(SDL_INIT_VIDEO)
     <0) {


    SDL_Quit();CALOUT(K,"SDL INIT FAILED\n");
    return FATAL;

  };

  openwins=1;

  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0,&DM);

  if(fullscreen) {

    SIN_WSIZX=DM.w;
    SIN_WSIZY=DM.h;

  };

//   ---     ---     ---     ---     ---
// #:0x3;> make window

  curwin=MKWIN(title,SIN_WSIZY,SIN_WSIZX,fullscreen);
  ogl_context=SDL_GL_CreateContext(curwin->window);
  gladLoadGLLoader((
    GLADloadproc)SDL_GL_GetProcAddress

  );SDL_GL_SetSwapInterval(1);

//   ---     ---     ---     ---     ---
// #:0x4;>


  /* joystick stuff can't be tested now
  int numjoys=SDL_NumJoysticks();

  if(numjoys>0)
  {

    main_joy=0;
    for(int i=0;i<numjoys;i++)
    {

      SDL_JoystickEventState(SDL_ENABLE);
      openJoy(curwin,i);curwin->joys[i].sens=2.5f;
    }
  } */

//   ---     ---     ---     ---     ---
// #:0x5;>


  glEnable(GL_ALPHA_TEST);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  chmang_clock=MKCLCK(".*^~",8,1.0f);
  STCLCK(chmang_clock);
  GTCHRGRD(24);

  return DONE;

};

//   ---     ---     ---     ---     ---
// #:0x6;>

int DLCHMNG() {


  SDL_GL_DeleteContext(ogl_context);
  DLMEM(chmang_clock);BKWIN(curwin);

  SDL_Quit();

  return DONE;

};

//   ---     ---     ---     ---     ---
// #:0x7;>


void GTCHMNGKPROC(uchar* mask) {
  *mask=0;/*(curwin->keyb&*mask);*/

};

void GTCHMNGMVKEY(ustr8* moveaxis) {


  moveaxis->x=0;
  moveaxis->y=0;

  /*if(curwin->keyb&(0x2000+0x8000)) {

    moveaxis->x=255;
    if(curwin->keyb&0x8000) {moveaxis
     ->x*=-1;};
  };

  if(curwin->key&(0x1000+0x4000)) {

    moveaxis->y=255;
    if(curwin->keyb&0x4000) {moveaxis
     ->y*=-1;};
  };*/

};

//   ---     ---     ---     ---     ---

uchar* GTIBUF(void) {
  if(strlen(curwin->ibuff)) {
    return curwin->ibuff;

  };return NULL;

};

void CLIBUF(void) {
  curwin->ibuff[0]=0x00;
  curwin->ibuff_i^=curwin->ibuff_i;

};

//   ---     ---     ---     ---     ---
// #:0x8;>

