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
#include "AVTOMAT/ffr.h"

#include <stdio.h>

mat4 IDENTITY={

  ffr_mul,0,0,0,
  0,ffr_mul,0,0,
  0,0,ffr_mul,0,
  0,0,0,ffr_mul

};

//   ---     ---     ---     ---     ---

/*

    (cy*cz, sx*sy*cz-cx*sz, cx*sy*cz+sx*sz),
    (cy*sz, sx*sy*sz+cx*cz, cx*sy*sz-sx*cz),
    (-sy, sx*cy, cx*cy)

*/

mat4 lookat(
  vec4 pos,
  vec4 at,
  vec4 up

) {

  // byte-sized gluLookAt
  vec4 F=normv(subv(at,pos));
  vec4 S=crossv(F,normv(up));
  vec4 U=crossv(normv(S),F);

  mat4 m={

    S.x,S.y,S.z,0,
    U.x,U.y,U.z,0,

    -F.x,-F.y,-F.z,0,

    0,0,0,ffr_mul

  };return m;

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