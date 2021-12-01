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

#include <stdlib.h>
#include <inttypes.h>

//   ---     ---     ---     ---     ---
// #:0x0;>

int main(int argc,char** argv) {

  NTCHMNG("SINx8",1);
  NTSIN(2);

  BEGPSH();

  float sc[2];
  uint  ws[2];
  GTCHRSZ(sc);
  GTSCRSZ(ws);

  uint CharCount=ws[0]*ws[1];
  uint* d=(uint*) malloc(CharCount*sizeof(uint));

  uchar c=0;
  for(uint y=0;y<ws[1];y++) {
    for(uint x=0;x<ws[0];x++) {
      d[x+(y*ws[0])]=c;c++;

    };
  };

  float t[4]={sc[0],-1,sc[1],1};

  while(GTCHMNGRUN()) {

    int evilstate=FRBEGCHMNG();
    if(evilstate) {
      break;

    };

    PSHCHR(d);

    FRENDCHMNG();
    SLEEPCHMNG();

  };

  free(d);

  DLSIN();
  DLCHMNG();

  return 0;

};

//   ---     ---     ---     ---     ---
// #:0x1;>

