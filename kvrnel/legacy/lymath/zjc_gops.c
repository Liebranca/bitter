//**************************
// ZJC_GOPS                *
// generalpurpose math     *
//                         *
// LIBRE SOFTWARE          *
// Licenced under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************



#include "zjc_gops.h"
#include "zjc_evil.h"

//   ---     ---     ---     ---     ---
// #:0x0;>

uint mxui(uint a,uint b) {
  if(a>b) {return a;} return b;

};

uint mnui(uint a,uint b) {
  if(a<b) {return a;} return b;

};

uint sbmxui(uint a,uint b) {
  return mnui(a,b)-mxui(a,b);

};

uint sbmnui(uint a,uint b) {
  return mxui(a,b)-mnui(a,b);

};

uint clampui(uint x,uint mn,uint mx) {
  if  (x<mn) {return mn;}
  elif(x>mx) {return mx;};

  return x;

};

//   ---     ---     ---     ---     ---

float pal_utof(uint x,uint off) {
  return ((float) ((x&(0xFF<<off))>>off))/255.0f;

};

//   ---     ---     ---     ---     ---
// #:0x1;>

