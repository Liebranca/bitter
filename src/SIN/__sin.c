//**************************
// SIN                     *
// bytesized graphics      *
//                         *
// LIBRE SOFTWARE          *
// Licenced under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************



#include "KVRNEL/lymath.h"

#include "sin_globals.h"
#include "sin_texture.h"
#include "sin_shader.h"

#include "REND/sin_canvas.h"
#include "__sin.h"

#include <math.h>

//   ---     ---     ---     ---     ---
// #:0x0;>

static int FONT_MUL = 1;

void NTSIN(int chrsz) {

  FONT_MUL=chrsz;

  NTSINHASH();
  NTSINTEX();
  NTSINSHD();
  NTCANVAS();

};

void DLSIN() {


  DLCANVAS();
  DLSINSHD();
  DLSINTEX();
  DLSINHASH();

};

//   ---     ---     ---     ---     ---
// #:0x1;>


static uint SIN_CHRBLK[2]= {16,16};

void GTCHRGRD(uint chrsz) {


  uint base_sz[2]= {chrsz,chrsz};
  uint z [2]= {SIN_WSIZX,SIN_WSIZY};

  for(uint x=0;x<2;x++) {
    while((base_sz[x]%z[x])>chrsz) {

      base_sz[x]++;if(base_sz[x]==128) {break;}

    };SIN_CHRBLK[x]=base_sz[x];

  };
};

//   ---     ---     ---     ---     ---
// #:0x2;>


void GTCHRSZ(float* dst) {

  dst[0]=(
    ((float)SIN_CHRBLK[0])
    /((float)SIN_WSIZX))*FONT_MUL;

  dst[1]=(
    ((float)SIN_CHRBLK[1])
   /((float)SIN_WSIZY))*FONT_MUL;

};

void GTSCRSZ(uint* dst) {


  uint z[2]= {SIN_WSIZX,SIN_WSIZY};

  for(uint x=0;x<2;x++) {

    uint i=0;
    uint chrsiz=SIN_CHRBLK[x]*FONT_MUL;

    while(z[x]>chrsiz) {

      z[x]-=chrsiz;i++;

    };if(z[x]>=(chrsiz/2)) {
      i++;

    };dst[x]=i*2;

  };
};

//   ---     ---     ---     ---     ---
// #:0x3;>

