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
static float PALETTE[64];

void STPAL(

  uint* pal,
  int off,
  int sz) {

  for(int x=off*4,y=0;x<sz*4;x+=4,y++) {
    PALETTE[x+0]=pal_utof(pal[y],24); // R
    PALETTE[x+1]=pal_utof(pal[y],16); // G
    PALETTE[x+2]=pal_utof(pal[y], 8); // B
    PALETTE[x+3]=pal_utof(pal[y], 0); // A

  };
};

float* GTPAL(int n) {
  return (PALETTE+(n*4));

};

void NTSIN(int chrsz) {

  FONT_MUL=chrsz;

  uint def_pal[16]={
    0x000020FF,             // bg def
    0x000040FF,             // bg high a
    0x800000FF,             // bg high b
    0x400040FF,             // bg high c

    0x40AE40FF,             // fg def
    0x00A0A0FF,             // string/comment
    0xB0B000FF,             // caret/bracehi
    0xD09820FF,             // numbers
    0xB0A040FF,             // operators
    0x8020A0FF,             // keys
    0x0080B0FF,             // ins/type
    0xA01020FF,             // directive

    0x202060FF,             // shade darker
    0x404080FF,             // shade dark
    0x6060A0FF,             // shade light
    0x8080C0FF,             // shade lighter

  };STPAL(def_pal,0,16);

  //NTSINHASH();
  //NTSINTEX();
  //NTSINSHD();
  NTCANVAS();

};

void DLSIN() {


  DLCANVAS();
  //DLSINSHD();
  //DLSINTEX();
  //DLSINHASH();

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

  };dst[0]+=1;
};

//   ---     ---     ---     ---     ---
// #:0x3;>

