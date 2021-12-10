//**************************
// CHWIN                   *
// sdl2 windows stuff      *
//                         *
// LIBRE SOFTWARE          *
// Licenced under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************



#include "chWIN.h"

//   ---     ---     ---     ---     ---
// #:0x0;>


#define chWIN_FLAGS_OPEN 0x01
#define chWIN_FLAGS_FOCUS 0x02
#define chWIN_FLAGS_ISTOP 0x04

void STWINFLAG(
  WIN* win,
  int flag) {

  win->flags|=flag;

};

void USTWINFLAG(
  WIN* win,
  int flag) {

  win->flags&=~flag;

};

//   ---     ---     ---     ---     ---
// #:0x1;>


WIN* MKWIN(
  char* title,
  int height,
  int width,
  int fullscreen) {


  WIN* win;ID id=IDNEW("WIN*",title);
  MEMGET(WIN,win,0,&id);

//   ---     ---     ---     ---     ---
// #:0x2;>


  win->window=SDL_CreateWindow(

    title,

    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,

    width,
    height,

    SDL_WINDOW_OPENGL|SDL_WINDOW_BORDERLESS
    |(SDL_WINDOW_FULLSCREEN_DESKTOP*fullscreen)

  );win->flags|=chWIN_FLAGS_OPEN;

//   ---     ---     ---     ---     ---
// #:0x3;>

  CLMEM2(win->ibuff,CHW_IBUFF_SZ);
  win->ibuff_i^=win->ibuff_i;

  win->size.x=width;
  win->size.y=height;

  win->hsize.x=width/2;
  win->hsize.y=height/2;

  win->flags=0;

  /*
  whandle.mouse.state=0;
  whandle.mouse.abs.x=whandle.hsize.x;
  whandle.mouse.abs.y=whandle.hsize.y;

  whandle.mouse.rel.x=0.0f;
  whandle.mouse.rel.y=0.0f;
  whandle.mouse.sens=0.65f;
  */

  STWINFLAG(win,chWIN_FLAGS_OPEN);

  return win;

};

//   ---     ---     ---     ---     ---
// #:0x4;>


int BKWIN(WIN* win) {

  SDL_DestroyWindow(win->window);
  DLMEM(&(win->m));

  return DONE;

};

void SWPWINBUF(WIN* win) {
  SDL_GL_SwapWindow(win->window);

};

void SHWWINCUR(void) {SDL_ShowCursor(SDL_DISABLE);};
void HIDWINCUR(void) {SDL_ShowCursor(SDL_ENABLE);};

int GTWINOPEN(WIN* win) {
  return win->flags&chWIN_FLAGS_OPEN;

};

//   ---     ---     ---     ---     ---
// #:0x5;>


int POLWIN(WIN* win) {


  int k;
  SDL_Event event;

  SDL_Window* focused=SDL_GetMouseFocus();
  Uint32 wFlags=SDL_GetWindowFlags(win->window);

  int wrap_cond=focused!=win->window;

  /* legacy stuff I might port later

  if(wFlags & SDL_WINDOW_FULLSCREEN_DESKTOP) { wrap_cond = cursorToWall(whandle); }*/

  /*if((wrap_cond)
  &&!(wFlags&SDL_WINDOW_MINIMIZED)
  &&(whandle->isOnTop)) {mouseWrap(whandle);}
  */

  int busy=0;

//   ---     ---     ---     ---     ---
// #:0x6;>


  while(SDL_PollEvent(&event)) {

    busy++;

    switch(event.type) {


      case SDL_QUIT:

      USTWINFLAG(win,chWIN_FLAGS_OPEN);
      break;

//   ---     ---     ---     ---     ---
// #:0x7;>


      case SDL_WINDOWEVENT:

      if(event.window.windowID
      ==SDL_GetWindowID(win->window)) {


        switch(event.window.event) {

          case SDL_WINDOWEVENT_FOCUS_GAINED:

          SHWWINCUR();
          STWINFLAG(win,chWIN_FLAGS_FOCUS);
          break;

          case SDL_WINDOWEVENT_FOCUS_LOST:

          SHWWINCUR();
          USTWINFLAG(win,chWIN_FLAGS_FOCUS);
          break;

          default:
          break;
        };
      };

//   ---     ---     ---     ---     ---
// #:0x8;>


      /*
      case SDL_JOYBUTTONDOWN:

      {

        for(int i=0;i<CHASM_NUM_JOYS;i++)
        {

          if(whandle->joys[i].plugged)
          {

            if(
              event.jbutton.which=
              =whandle->joys[i].instanceID)
            {chJOY_SETBUTTON(
            &whandle->joys[i],
              event.jbutton.button);}
          }
        }

        break;
      }

      case SDL_JOYAXISMOTION:

      {

        for(int i=0;i<CHASM_NUM_JOYS;i++)
        {

          if(whandle->joys[i].plugged)
          {

            if(event.jaxis.which
              ==whandle->joys[i].instanceID)
            {chJOY_SETAXES(
              &whandle->joys[i],event.jaxis);}
          }
        }

        break;
      }

      */

//   ---     ---     ---     ---     ---

      case SDL_TEXTINPUT:

      win->ibuff_i&=(CHW_IBUFF_SZ-1);

      strcpy(
        win->ibuff+win->ibuff_i,
        event.text.text

      );win->ibuff_i+=strlen(event.text.text);

      win->ibuff_i&=(CHW_IBUFF_SZ-1);
      win->ibuff[win->ibuff_i]=0x00;

      break;

//   ---     ---     ---     ---     ---

      case SDL_KEYDOWN:

      k=event.key.keysym.sym;
      char c=0x00;

      switch(k) {

        case SDLK_ESCAPE:
        USTWINFLAG(win,chWIN_FLAGS_OPEN);
        c=0x1B;break;

        case SDLK_RETURN:
        c=0x0A;goto KUP_INSERT;

        case SDLK_BACKSPACE:
        c=0x08;goto KUP_INSERT;

        KUP_INSERT:
        win->ibuff_i&=(CHW_IBUFF_SZ-1);

        win->ibuff[win->ibuff_i]=c;
        win->ibuff_i++;

        win->ibuff_i&=(CHW_IBUFF_SZ-1);
        win->ibuff[win->ibuff_i]=0x00;
        break;

        default:
        break;

      }

      /*for(uint i=0;i<8;i++) {

        if(k==CH_KEYB_KCODES[i]) {
          STWINKEY(((uint*) &win->keys)+1,i);

        };
      };*/break;

//   ---     ---     ---     ---     ---

      case SDL_KEYUP:

      /*for(uint i=0;i<8;i++) {

        if(k==CH_KEYB_KCODES[i]) {
          CLWINKEY(((uint*) &win->keys)+1,i);

        };
      };*/break;

//   ---     ---     ---     ---     ---
// #:0x9;>


      /*
      case SDL_MOUSEMOTION:

      {


        if(!(win->flags&chWIN_FLAGS_OPEN)) {

          if(
          !chRAT_ISIGGY(&whandle->mouse))
            {chRAT_RUN(
          &whandle->mouse,&event.motion);};
          else {chRAT_UNIGGY(&whandle->mouse);};

        };break;
        */

        default:
        break;

      };
    };

    return busy*4;
  };

//   ---     ---     ---     ---     ---
// #:0xA;>

