//**************************
// __SHB7                  *
// launches the kvr shell  *
//                         *
// LIBRE SOFTWARE          *
// Licenced under GNU GPL3 *
// be a bro and inherit    *
//                         *
// CONTRIBUTORS            *
// lyeb,                   *
//**************************



#include "__shb7.h"

#include "CHASM/chMNG.h"

#include "SIN/__sin.h"
#include "SIN/sin_texture.h"
#include "SIN/REND/sin_canvas.h"

//   ---     ---     ---     ---     ---
// #:0x0;>

int main(int argc,char** argv) {

  NTCHMNG("SINx8",1);
  NTSIN();

  while(GTCHMNGRUN()) {

    int evilstate=FRBEGCHMNG();
    if(evilstate) {
      break;

    };

    FRENDCHMNG();
    SLEEPCHMNG();

  };

  DLSIN();
  DLCHMNG();

  return 0;

};

//   ---     ---     ---     ---     ---
// #:0x1;>

