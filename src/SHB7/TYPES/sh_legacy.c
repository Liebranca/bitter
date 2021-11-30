//**************************
// SH_LEGACY               *
// kvnslrat boxes && termo *
//                         *
// LIBRE SOFTWARE          *
// Licenced under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************


#include "sh_legacy.h"

#include "KVRNEL/TYPES/zjc_hash.h"
#include "SIN/__sin.h"

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

//   ---     ---     ---     ---     ---
// #:0x0;>


#define SH_INBUFF_WIDTH 64

static PEOP PE_OPS[4];      // container for ops

static HASH* GNAMES;        // globals and
                            // built-ins

static HASH* LNAMES;        // user-defined or
                            // runtime-generated
                            // values

//   ---     ---     ---     ---     ---
// #:0x1;>


typedef struct {            // it's a modern
                            // typewriter

  struct termios t;         // gross termios
                            // stuff

  struct termios restore;   // idem

  uchar kb[SH_INBUFF_WIDTH];// reader for input
                            // buff

  int kbh;                  // chars waiting on
                            // input buff

  float curt[4];            // cursor transform
  float chrt[4];            // (next) char
                            // transform

  ustr8 chrd[4];            // (next) char data

  uint nchars_x;            // x-size of window,
                            // in number of chars

  uint nchars_y;            // y-size of window,
                            // in number of chars

} KVR;static KVR kvr= {0};

//   ---     ---     ---     ---     ---

#define JMPCON_FMOVCUR 0x01
#define JMPCON_FMOVCHR 0x02

void JMPCON(void* buff) {

  ustr8 co=*((ustr8*) buff);

  // clamp to {0, scrsz}
  uint raw_x=clampui(co.x-1, 0, kvr.nchars_x);
  uint raw_y=clampui(co.y-1, 0, kvr.nchars_y);

  // multiply scale by number of chars
  float x=-1+(kvr.curt[0] * raw_x);
  float y= 1-(kvr.curt[2] * raw_y);

//   ---     ---     ---     ---     ---

  // assign values to __cursor__ position
  if(co.z & JMPCON_FMOVCUR) {
    kvr.curt[3] = y;
    kvr.curt[1] = x;

  };

  // assign values to __next_char__ position
  if(co.z & JMPCON_FMOVCHR) {
    kvr.chrt[3] = y;
    kvr.chrt[1] = x;

  };

  // make debug call
  CALOUT(P,
    "\e[67;0HJumped to [%u,%u]",
    raw_x, raw_y

  );
};

//   ---     ---     ---     ---     ---

void GBPE(void* buff) {
  ustr8 data = *((ustr8*) buff);
  CALOUT(P, "\e[67;0HYou gb 0x%08X", data.F);

};

void WTPE(void* buff) {
  CALOUT(P, "\e[67;0HYou wt!");

};

//   ---     ---     ---     ---     ---
// #:0x2;>

void NTPEOP(uint idex,
char* opcode,
uint data,
STARK fun) {

  PEOP* op=PE_OPS+idex;
  op->id=IDNEW("FUN*",opcode);
  op->data.F=data;
  op->fun=fun;

};

PEOP* GTPENOP(void) {
  return PE_OPS+0;

};

//   ---     ---     ---     ---     ---
// #:0x3;>


CHRSPRT MKCHRSPRT(

char* buff,
uint co) {


  CHRSPRT sprite= {0};

  sprite.buff=buff;
  sprite.length=strlen(buff);
  sprite.current=0.0f;

                            // clamp position to
                            // screen
  sprite.co.x=clampui((co&0x00FF),1,kvr.nchars_x);
  sprite.co.y=clampui((co&0xFF00),1,kvr.nchars_y);

  PLCHRSPRT(&sprite,0.0f);  // init draw buffer

  return sprite;

};

void PLCHRSPRT(

CHRSPRT* sprite,
float dirn) {


                            // wrap around end
  if(sprite->current>=sprite->length) {
    sprite->current=0.0f;

  }

  elif(sprite->current<0) { // wrap around start
    sprite->current=sprite->length-1;

  };

  char c=sprite->buff [(uint) (sprite->current)];
  sprite->current+=dirn;

                            // load data into
                            // frame
  sprintf(
    sprite->frame,
    "$:jmp %c %c \x02;>%c",
    sprite->co.x,sprite->co.y,c

  );
};

//   ---     ---     ---     ---     ---
// #:0x4;>


void NTKVR(uint chrsz) {

  tcgetattr(0,&kvr.t);      // gross termios
                            // stuff

  kvr.restore=kvr.t;        // chasm has event
                            // polling, so this
                            // is redundant
                            // but it's nice to
                            // have in case we
                            // drop sdl

  kvr.t.c_lflag&=~ICANON;
  kvr.t.c_lflag&=~ECHO;

  tcsetattr(0,TCSANOW,&kvr.t);

//   ---     ---     ---     ---     ---
// #:0x5;> actual chasm stuff


                            
                            
  float sc[2];              // init cursor/char
  GTCHRSZ(sc,chrsz);        // vars

  // set initial x,y scale & position
  // we just initialize next char and cursor
  // to the same thing

  kvr.chrt[0]=kvr.curt[0]=sc[0];
  kvr.chrt[1]=kvr.curt[1]=-1.0;
  kvr.chrt[2]=kvr.curt[2]=sc[1];
  kvr.chrt[3]=kvr.curt[3]=1.0;

  CLMEM2(
    kvr.chrd,
    xBYTES(4,ustr8)

  );                        // zero-out chardata

  uint ws[2];

  GTSCRSZ(ws,chrsz);        // screen size
  kvr.nchars_x=ws[0];       // calculation,
  kvr.nchars_y=ws[1];       // needed for jumps

  CLMEM2(
    kvr.kb,
    SH_INBUFF_WIDTH

  );                        // blank out the
                            // inbuff

//   ---     ---     ---     ---     ---
// #:0x6;>

  GNAMES=MKHASH(5,"GNAMES");// global names

  // for opcode-to-funptr fetch
  NTPEOP(0x01,"jmp",0x0001,&JMPCON);
  NTPEOP(0x02,"gb",0x0001,&GBPE);
  NTPEOP(0x03,"wt",0x0001,&WTPE);

  // upload opcodes to gnames
  HASHSET(GNAMES,byref(PE_OPS[1].id));
  HASHSET(GNAMES,byref(PE_OPS[2].id));
  HASHSET(GNAMES,byref(PE_OPS[3].id));

};

//   ---     ---     ---     ---     ---
// #:0x7;>


void DLKVR() {

  DLMEM(GNAMES);
  tcsetattr(0,TCSANOW,&kvr.restore);

};

//   ---     ---     ---     ---     ---
// #:0x8;>


float* GTKVRCURT(void) {
  return kvr.curt;

};

float* GTKVRCHRT(void) {
  return kvr.chrt;

};

ustr8* GTKVRCHRD(void) {
  return kvr.chrd;

};

//   ---     ---     ---     ---     ---
// #:0x9;> just like conio


int kbhit(void) {
  ioctl(0,FIONREAD,&kvr.kbh);return kvr.kbh;

};

void getch(void) {

  int remain=kvr.kbh;
  int x=0;

  while(remain) {
    kvr.kb[x]=getchar();x++;remain--;
    if(x==SH_INBUFF_WIDTH) {
      break;

    }
  };
};

//   ---     ---     ---     ---     ---
// #:0xA;>


void GTOP(

PEOP** dst,
char* key) {

  STR_HASHGET(GNAMES,key,*dst,0);

};

//   ---     ---     ---     ---     ---
// #:0xB;>

