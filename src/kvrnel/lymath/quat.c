//**************************
// QUAT                    *
// because 360 happens     *
//                         *
// LIBRE SOFTWARE          *
// Licensed under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************

#include "quat.h"
#include "ffr.h"

//   ---     ---     ---     ---     ---

char lenq(quat a) {
  return sqrtc(
    pow2c(a.x)+pow2c(a.y)
   +pow2c(a.z)+pow2c(a.w)

  );

};

quat normq(quat a) {

  char len=lenv(a);
  len+=1*(!len);

  quat c={
    divc(a.x,len),
    divc(a.y,len),
    divc(a.z,len),
    divc(a.w,len),

  };return c;

};

//   ---     ---     ---     ---     ---

quat conjq(quat a) {

  quat c={
    -a.x,
    -a.y,
    -a.z,
    a.w

  };return c;

};

//   ---     ---     ---     ---     ---

quat mulq(quat a,quat b) {

  quat c={

    mulc(a.x,b.w)+mulc(a.w,b.x)
   +mulc(a.y,b.z)-mulc(a.z,b.y),

    mulc(a.w,b.y)-mulc(a.x,b.z)
   +mulc(a.y,b.w)+mulc(a.z,b.x),

    mulc(a.w,b.z)+mulc(a.x,b.y)
   -mulc(a.y,b.x)+mulc(a.z,b.w),

    mulc(a.w,b.w)-mulc(a.x,b.x)
   -mulc(a.y,b.y)-mulc(a.z,b.z)

  };return c;

};

//   ---     ---     ---     ---     ---

quat qaxang(vec4 v,char ang) {

  char hf_ang=sinc(divc(ang,2*ffr_scale));

  quat c={

    mulc(v.x,hf_ang),
    mulc(v.y,hf_ang),
    mulc(v.z,hf_ang),

    sinc(divc(ang,2*ffr_scale))

  };return c;

};

//   ---     ---     ---     ---     ---