//**************************
// __SHB7                  *
// launches the kvr shell  *
//                         *
// LIBRE SOFTWARE          *
// Licenced under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************



#include "__shb7.h"
#include "sh_gbl.h"

#include "KVRNEL/MEM/sh_map.h"
#include "KVRNEL/TYPES/sh_char.h"
#include "KVRNEL/fizzel.h"

#include "CHASM/chMNG.h"

#include "SIN/__sin.h"
#include "SIN/sin_texture.h"
#include "SIN/REND/sin_canvas.h"

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <signal.h>

#include <sys/wait.h>
#include <limits.h>
#include <errno.h>

//   ---     ---     ---     ---     ---
// #:0x0;>

static int shbout_flg=0x00;

//   ---     ---     ---     ---     ---

char* pshout(

  uchar* str,

  uchar col_id,
  char flg

) {

  size_t pos[2];gticpos(pos+0);
  uchar sh_ctl=0x1F;

  sticcol(col_id);

//   ---     ---     ---     ---     ---

  if(strlen(str)==sizeof(SHSIG_MSIN)+1) {
    SHSIG* sig=(SHSIG*) str;

    // chances are a human wouldn't type this
    // it's not impossible, just highly improbable

    if( sig->hed[0]==SHSIG_MSIN[0]
    &&  sig->hed[1]==SHSIG_MSIN[1] ) {
      str+=sizeof(SHSIG)+sig->len;
      //flg|=0x08;

    };

  };ichar* dr_buff=rfdrbuf();

//   ---     ---     ---     ---     ---

  do {

    // skip conditions

    if(!(flg&0x08)) {

      flg|=(
       (*str<0x20 && (dr_buff+2)->ch<=0x20)
       *(0x04*(!(flg&0x02)))

      );if( (flg&0x04) || !*str) {
        break;

//   ---     ---     ---     ---     ---

      // detect peso escapes

      };ushort nc=0x0000;
      if(strlen(str)>2) {

        nc=*((ushort*) str);

        if(nc==0x3A24) {
          sticctl(0x00);str+=2;

        } else if(nc==0x3E3B) {
          sticctl(0x1F);str+=2;

        };
      };

//   ---     ---     ---     ---     ---

      // regular char append
      if(*str!=0x08) {
        wric(*str);

      // backspace
      } else {
        bkic();continue;

      };
    };

  } while(*str++);

//   ---     ---     ---     ---     ---

  shbout_flg|=0x01;
  if(flg&0x01) {
    sticpos(pos+0);

  };if(!(flg&0x02)) {
    prompt_cap();

  };

  return str++;

};

void oncont(int sig) {
  return;

};

//   ---     ---     ---     ---     ---

int main(int argc,char** argv) {

  effms();

  char pwd[PATH_MAX+1];     // get bin folder
  char* binf;

  {
    binf = realpath(argv[0],pwd);
    int last=strlen(binf)-1;
    do {
      last--;

    } while(binf[last]!='/');
    binf[last+1]=0x00;

    for(int x=0;x<strlen(binf);x++) {
      if(binf[x]=='\\') {
        binf[x]='/';

      };
    };

    char* lach=binf+(strlen(binf)-1);
    if(*lach!='/') {
      *(lach+1)='/';
      *(lach+2)=0x00;

    };
  };

//   ---     ---     ---     ---     ---

  int spwn_pid;
  void** mh;
  {

    // set outfile as an abspath

    // why? because it doubles as shared memory
    // we don't just need to know where it is
    // every child process needs to know as well

    char fpath[PATH_MAX+1];

    strcpy(fpath,binf);
    strcat(fpath,"shb7wt"); // cat bin+outfile

    // now make an mmapd file ;>
    mh=ntmap_origin(fpath,1);

    // mh key is needed solely as a brute-force way of
    // telling spwn about the shmem
    char* mh_key=encodemh(mh);

//   ---     ---     ---     ---     ---

    // we need to handle pauses
    signal(SIGCONT,oncont);

    // now make spwn
    spwn_pid=fork();
    if(!spwn_pid) {
      char* ex_argv[3];
      char ex_fpath[PATH_MAX+1];
      ex_argv[0]=ex_fpath;

      strcpy(ex_argv[0],binf);
      strcat(ex_argv[0],"spwn");

      ex_argv[1]=mh_key;
      ex_argv[2]=NULL;

      if(!access(ex_argv[0], X_OK)) {

        dup2(mapfd(mh),STDOUT_FILENO);

        execv(ex_argv[0],ex_argv);
        exit(-1);

      };CALOUT(E,"spwn not found\n");
      dlmap(mh);

      return -1;

    };
  };

//   ---     ---     ---     ---     --- window init

  // create the window manager
  // that in turn spawns a window!

  NTCHMNG("SHB7x64",0);

  // *then* init the renderer
  // this is simply so there is an OpenGL context
  // before the system is even able to try and draw

  // honestly? you could flip them around
  // i init in this order because it makes sense to me

  NTSIN(2);

  // get charsize as a value between {0.0f,1.0f}
  // this value is relative to screen size

  // why? basis: the screen is a grid of characters
  // you want to know the percentage of the screen
  // taken up by each one of those

  float sc[2];GTCHRSZ(sc);

  // related to charsize, this gives you how many
  // characters fit in the screen on a given direction
  // x gives you the number of columns
  // y is number of rows

  uint  ws[2];GTSCRSZ(ws);

  // now multiply those, and you know how many
  // characters total a single screen can have

  uint CharCount=ws[0]*ws[1];

  // first block of shmem is for console draw
  // second is IPC flags
  // third is IPC input (shb7 <- child process)

  int* shbuf;
  char* c_out;{

    ichar* shbout=(ichar*) *mh;
    sticsc(shbout,ws[0],ws[1]);

    shbuf=(int*) (shbout+SHM_STEP);
    c_out=(char*) (shbuf+SHM_STEP);

  };BEGPSH();

//   ---     ---     ---     ---     ---

  // dummy run to drain init garbage on input buff

  for(int i=0;i<5;i++) {

    if(FRBEG(0)) {
      break;

    };

    FREND();
    FRSLP();

  };CLIBUF();

//   ---     ---     ---     ---     ---

  pshout("$:",0x04,0);      // draw prompt
  PSHCHR(rfdrbuf(),1);

  shbout_flg&=~0x02;        // lock

  size_t cursor_pos[2];
  gticpos(cursor_pos);

//   ---     ---     ---     ---     --- loop head

  // the window manager will know when to exit
  while(GTRUN()) {

    // frame start is very important!

    // based on the value passed to frbeg, the manager
    // can speed up or slow down the program

    // this translates to longer or shorter sleep time
    // so mind this arg -- it controls everything

    // for now we only care if out has been written to

    int evilstate=FRBEG(
      (( (shbout_flg&0x01)
       && !(shbuf[0]&0x4) )!=0)*4

    );if(evilstate) {
      break;

    };shbout_flg&=~0x01;

//   ---     ---     ---     ---     --- exec

    // IPC BUFFER == SHBUF

    if( (!(shbuf[0]&0x02))
    &&  (!(shbuf[0]&0x04)) ) {

      uchar* ibuff=GTIBUF();
      if(ibuff) {

//   ---     ---     ---     ---     ---

        // process command on ret

        if(ibuff[strlen(ibuff)-1]==0x0A) {
          shbuf[1]=gticposr();
          shbuf[0]|=0x02;
          shbuf[0]|=0x04;

          kill(spwn_pid,SIGCONT);

          gticpos(cursor_pos);

          cursor_pos[0]=0;
          cursor_pos[1]=0;
          sticpos(cursor_pos);

//   ---     ---     ---     ---     ---

        // else append

        } else {
          pshout(ibuff,0x05,0);

        };
      };CLIBUF();

//   ---     ---     ---     ---     ---

    } else

    // if !lock, pass commands to spwn

    if( (!(shbuf[0]&0x02))
    &&  ( (shbuf[0]&0x04)) ) {

      cursor_pos[0]=0;
      cursor_pos[1]=2;
      sticpos(cursor_pos);

      if(*c_out) {
        char* ic=c_out;
        while(*(ic=pshout(ic,0x05,0b10))) {;}
        memset(c_out,0,SHM_STEP);

      };

      cursor_pos[0]=0;
      cursor_pos[1]=0;
      sticpos(cursor_pos);

      pshout("$:",0x04,0b00);
      shbuf[0]&=~0x04;

//   ---     ---     ---     ---     ---

    // locked, draw clock
    } else {
      char cl[2]={0xA9+(shbuf[2]&0b111),0x00};
      pshout(cl,0x06,0b01);shbuf[2]++;

    };

//   ---     ---     ---     ---     --- loop tail

    // draw the screen buffer, it's done in one go!

    // how many chars you have is irrelevant, so
    // large prints are insanely cheap

    // do note that the buffer is only updated
    // when shbout is written to, and so if you
    // don't register a write then the previous
    // frame is drawn instead, repeating right
    // until you set the flag

    PSHCHR(rfdrbuf(),shbout_flg&0x01);

    // these two must always go at the very bottom!
    // order matters: one gets current time, the
    // other calculates frame delta

    FREND();
    FRSLP();

  };

//   ---     ---     ---     ---     ---

  shbuf[0]|=0x01;
  kill(spwn_pid,SIGCONT);
  int wst;wait(&wst);

  ENDPSH();

  DLSIN();
  DLCHMNG();
  dlmap(mh);

  return 0;

};

//   ---     ---     ---     ---     ---
// #:0x1;>

