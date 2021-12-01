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

#include "KVRNEL/MEM/zjc_clock.h"
#include "CHASM/chMNG.h"

#include "SIN/__sin.h"
#include "SIN/sin_texture.h"
#include "SIN/REND/sin_canvas.h"

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

//   ---     ---     ---     ---     ---
// #:0x0;>

static uint* shbout=NULL;

static int shbout_mx=0;
static int shbout_my=0;

static int shbout_x=0;
static int shbout_y=0;

static int shbout_flg=0x00;

void pshout(
  char* str,
  char col_id,
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

int main(int argc,char** argv) {

  NTCHMNG("SINx8",1);
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

  char buff[64]={0,0};

  BEGPSH();
  while(GTCHMNGRUN()) {

    int evilstate=FRBEGCHMNG();
    if(evilstate) {
      break;

    };

    buff[0]=0xA9+counter;

    pshout(buff,13,1);
    PSHCHR(shbout,shbout_flg&0x01);
    shbout_flg&=~0x01;

    counter++;
    if(counter>7) {
      counter^=counter;

    };

    FRENDCHMNG();
    SLEEPCHMNG();

  };

  ENDPSH();
  free(shbout);

  DLSIN();
  DLCHMNG();

  return 0;

};

//   ---     ---     ---     ---     ---
// #:0x1;>

