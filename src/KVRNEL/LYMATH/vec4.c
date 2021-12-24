//**************************
// VEC4                    *
// because 16 happens      *
//                         *
// LIBRE SOFTWARE          *
// Licensed under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************

#include "vec4.h"
#include "AVTOMAT/ffr.h"

#include <stdio.h>
#include <math.h>

char ftoc(char x) {return x*(1<<ffr_scale);};

//   ---     ---     ---     ---     ---

vec4 mkv4(
  char x,
  char y,
  char z,
  char w

) {

  vec4 a={
    ftoc(x),
    ftoc(y),
    ftoc(z),
    ftoc(w)

  };return a;

};

//   ---     ---     ---     ---     ---
// move

vec4 addv(vec4 a, vec4 b) {

  vec4 c={0};

  for(int i=0;i<3;i++) {
    c.mem[i]=a.mem[i]+b.mem[i];

  };return c;

};

vec4 subv(vec4 a, vec4 b) {

  vec4 c={0};

  for(int i=0;i<3;i++) {
    c.mem[i]=a.mem[i]-b.mem[i];

  };return c;

};

//   ---     ---     ---     ---     ---
// scale

vec4 mulv(vec4 a, char b) {

  vec4 c={0};

  for(int i=0;i<3;i++) {
    c.mem[i]=mulc(a.mem[i],b);

  };return c;
};

vec4 divv(vec4 a, char b) {

  vec4 c={0};

  if(!b) {
    return c;

  };

  for(int i=0;i<3;i++) {
    c.mem[i]=divc(a.mem[i],b);

  };return c;
};

//   ---     ---     ---     ---     ---

vec4 negv(vec4 a) {

  vec4 c={-a.x,-a.y,-a.z,-a.w};
  return c;

};

//   ---     ---     ---     ---     ---
// length of vector

char lenv(vec4 a) {
  return sqrtc(pow2c(a.x)+pow2c(a.y)+pow2c(a.z));

};

// distance to vector

int distv(vec4 a,vec4 b) {
  vec4 vecto=subv(a,b);
  return lenv(vecto);

};

//   ---     ---     ---     ---     ---

// brain hurty

char dotv(vec4 a,vec4 b) {
  return mulc(a.x,b.x)+mulc(a.y,b.y)+mulc(a.z,b.z);

};

vec4 normv(vec4 a) {

  char len=lenv(a);
  len+=1*(!len);

  vec4 c={
    divc(a.x,len),
    divc(a.y,len),
    divc(a.z,len),

    ftoc(1)

  };return c;

};

vec4 crossv(vec4 a,vec4 b) {

  vec4 c={

    mulc(a.y,b.z)-mulc(a.z,b.y),
    mulc(a.z,b.x)-mulc(a.x,b.z),
    mulc(a.x,b.y)-mulc(a.y,b.x),

    ftoc(1)

  };return c;

};

//   ---     ---     ---     ---     ---

char* prv(vec4 v) {

  static char buff[0x100];

  sprintf(

    buff,
    "[%02X,%02X,%02X,%02X]",

    v.x&0xFF,v.y&0xFF,
    v.z&0xFF,v.w&0xFF

  );return buff;

};

//   ---     ---     ---     ---     ---