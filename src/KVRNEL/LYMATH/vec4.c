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

#include <stdio.h>
#include <math.h>

//   ---     ---     ---     ---     ---

const char ffr_scale=5;
const float ffr=1.0f/((float) (1<<ffr_scale));

#define mulc(a,b) (((a)*(b))>>ffr_scale)
#define divc(a,b) ((((a)<<ffr_scale)/(b)))
#define pow2c(x) (mulc((x),(x)))
#define sqrtc(x) ((sqrt((x)*ffr))*(1<<ffr_scale))

#define ftoc(x) ((x)*(1<<ffr_scale))

//   ---     ---     ---     ---     ---
// move

void addv(vec4* a, vec4* b) {

  for(int i=0;i<4;i++) {
    a->mem[i]+=b->mem[i];

  };

};

void subv(vec4* a, vec4* b) {

  for(int i=0;i<4;i++) {
    a->mem[i]-=b->mem[i];

  };

};

//   ---     ---     ---     ---     ---
// scale

void mulv(vec4* a, char b) {

  for(int i=0;i<4;i++) {
    a->mem[i]=mulc(a->mem[i],b);

  };
};

void divv(vec4* a, char b) {

  if(!b) {
    return;

  };for(int i=0;i<4;i++) {
    a->mem[i]=divc(a->mem[i],b);

  };
};

//   ---     ---     ---     ---     ---
// length of vector

int lenv(vec4* a) {

  return sqrtc(
    pow2c(a->x)+pow2c(a->y)
   +pow2c(a->z)+pow2c(a->w)

  );

};

//   ---     ---     ---     ---     ---
// get a to b direction, return direction

vec4 gtvecto(vec4* a,vec4* b) {
  vec4 vecto=*a;
  subv(&vecto,b);

  return vecto;

};

// discard direction and return distance instead

int distv(vec4* a,vec4* b) {
  vec4 vecto=gtvecto(a,b);
  return lenv(&vecto);

};

//   ---     ---     ---     ---     ---

vec4 crossv(vec4* a,vec4* b) {

  vec4 c={0};

  c.x=mulc(a->y,b->z)-mulc(a->z,b->y);
  c.y=mulc(a->z,b->x)-mulc(a->x,b->z);
  c.z=mulc(a->x,b->y)-mulc(a->y,b->x);

  c.w=ftoc(1);

  return c;

};

//   ---     ---     ---     ---     ---

char* prv(vec4* v) {
  static char buff[0x100];
  sprintf(

    buff,
    "[%f,%f,%f,%f]",

    v->x*ffr,v->y*ffr,
    v->z*ffr,v->w*ffr

  );return buff;

};

//   ---     ---     ---     ---     ---