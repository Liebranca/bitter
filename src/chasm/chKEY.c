//**************************
// CHKEY                   *
// sdl keyboard stuff      *
//                         *
// LIBRE SOFTWARE          *
// Licenced under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************



#include "chKEY.h"

//   ---     ---     ---     ---     ---
// #:0x0;>

void STWINKEY(
  uint* k,
  uint input) {

  (*k)|=(1L<<input);

};

void CLWINKEY(
  uint* k,
  uint input) {

  (*k)&=~(1L<<input);

};

int GTWINKEY(
  uint* k,
  uint value) {

  return((*k)&value)==value;

};

//   ---     ---     ---     ---     ---
// #:0x1;>

