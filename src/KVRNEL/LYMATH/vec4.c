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
#include <math.h>

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

void mulv(vec4* a, float b) {

  for(int i=0;i<4;i++) {
    a->mem[i]*=b;

  };
};

void divv(vec4* a, float b) {

  if(!b) {
    return;

  };for(int i=0;i<4;i++) {
    a->mem[i]/=b;

  };
};

//   ---     ---     ---     ---     ---
// length of vector

int lenv(vec4* a) {

  return sqrt(
    pow(a->x,2)+pow(a->y,2)
   +pow(a->z,2)+pow(a->w,2)

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