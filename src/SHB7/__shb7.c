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
  NTSIN(2);

  BEGPSH();

  float sc[2];
  uint  ws[2];
  GTCHRSZ(sc);
  GTSCRSZ(ws);

  ustr8 d={0x24,0,0,0};
  float t[4]={sc[0],-1,sc[1],1};

  int fuck=0;

  while(GTCHMNGRUN()) {

    int evilstate=FRBEGCHMNG();
    if(evilstate) {
      break;

    };

    t[0]=sc[0];t[1]=-1;
    t[2]=sc[1];t[3]= 1;

    for(int y=0;y<ws[1];y++) {
      for(int x=0;x<ws[0];x++) {
        PSHCHR(t,&d);
        t[1]+=t[0];

      };t[1]=(-1);
        t[3]-=(t[2]*1.5);

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

