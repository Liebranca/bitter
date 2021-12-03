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

#include "KVRNEL/MEM/sh_map.h"

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

//   ---     ---     ---     ---     ---
// #:0x0;>

static uint* shbout=NULL;

static int shbout_mx=0;
static int shbout_my=0;

static int shbout_x=0;
static int shbout_y=0;

static int shbout_flg=0x00;

void pshout(
  uchar* str,
  uchar col_id,
  char reset) {

  uint ox=shbout_x;
  uint oy=shbout_y;

  int pos=shbout_x+(shbout_y*shbout_mx);
  do {
    if(!*str) { break; }

    shbout[pos]=(*str)|(col_id<<8);
    pos++;shbout_x++;

    if(shbout_x>=shbout_mx) {
      shbout_x=0;
      shbout_y++;

    };

    if(shbout_y>=shbout_my) {
      shbout_x=0;
      shbout_y=0;
      pos=0;break;

    };

  } while(*str++);

  shbout_flg|=0x01;
  if(reset) {
    shbout_x=ox;
    shbout_y=oy;

  };

};

void oncont(int sig) {
  return;

};

int main(int argc,char** argv) {

  NTCHMNG("SINx8",0);
  NTSIN(2);

  float sc[2];
  uint  ws[2];
  GTCHRSZ(sc);
  GTSCRSZ(ws);

  uint CharCount=ws[0]*ws[1];
  shbout_mx=ws[0];
  shbout_my=ws[1];

  uint counter=0;

  shbout=(uint*) malloc(CharCount*sizeof(uint));
  memset(shbout,0,CharCount*sizeof(uint));

  float t[4]={sc[0],-1,sc[1],1};

  uchar buff[64]={0,0};

  BEGPSH();

  void** mh=ntmap_origin("./shb7wt",4);
  char* bf=(char*) (*mh);

  char* mh_key=encodemh(mh);

  signal(SIGCONT,oncont);

  int fr=0;

  while(GTCHMNGRUN()) {

    //(shbout_flg&0x01)!=0
    int evilstate=FRBEGCHMNG(0);
    if(evilstate) {
      break;

    };shbout_flg&=~0x01;

    if(!fr) {
      fr|=1;int pid=fork();

      if(!pid) {

        char* ex_argv[3]={
          "../bin/x64/pf.exe",
          mh_key,
          NULL

        };execv(ex_argv[0],ex_argv);exit(-1);

      } else {
        int wst;
        wait(&wst);

        //pause();

      }
    };

    //buff[0]=0xA9+(counter&0x7);

    pshout(bf,4,1);
    PSHCHR(shbout,shbout_flg&0x01);

    counter++;

    FRENDCHMNG();
    SLEEPCHMNG();

  };

  dlmap(mh);

  ENDPSH();
  free(shbout);

  DLSIN();
  DLCHMNG();

  return 0;

};

//   ---     ---     ---     ---     ---
// #:0x1;>

