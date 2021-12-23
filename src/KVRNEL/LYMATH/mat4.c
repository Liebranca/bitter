//**************************
// MAT4                    *
// because 64 happens      *
//                         *
// LIBRE SOFTWARE          *
// Licensed under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************


#include "mat4.h"
#include <stdio.h>

//   ---     ---     ---     ---     ---

mat4 lookat(
  vec4 pos,
  vec4 at,
  vec4 up

) {

  vec4 fwd=normv(subv(pos,at));
  vec4 rgt=normv(crossv(up,fwd));

  up=normv(crossv(fwd,rgt));

  mat4 m={0};

  /*m.a=(vec4) {rgt.x,up.x,fwd.x,0};
  m.b=(vec4) {rgt.y,up.y,fwd.y,0};
  m.c=(vec4) {rgt.z,up.z,fwd.z,0};

  m.d.x=dotv(rgt,negv(pos));
  m.d.y=dotv(up,negv(pos));
  m.d.z=dotv(fwd,negv(pos));
  m.d.w=ftoc(1);*/

  m.a=(vec4) {pos.x,0,0,0};
  m.b=(vec4) {0,0,0,0};

  m.c=(vec4) {0,0,32,0};// culling planes

  m.d=(vec4) {32-pos.x,32-pos.y,0,32};

  return m;

};

mat4 normm(mat4* m) {

  mat4 n={0};

  n.a=normv(m->a);
  n.b=normv(m->b);
  n.c=normv(m->c);
  n.d=normv(m->d);

  return n;

};

//   ---     ---     ---     ---     ---

void prm(mat4* m) {

  printf("%s\n",prv(m->a));
  printf("%s\n",prv(m->b));
  printf("%s\n",prv(m->c));
  printf("%s\n\n",prv(m->d));

};

//   ---     ---     ---     ---     ---