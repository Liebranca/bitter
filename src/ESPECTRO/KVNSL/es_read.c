/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    ES_READ                               *
*                                           *
*     -espectro reader                      *
*     -displays buffs on termo              *
*                                           */
// *   ---     ---     ---     ---     ---  *
/*    LIBRE SOFTWARE                        *
*                                           *
*     Licenced under GNU GPL3               *
*     be a bro and inherit                  *
*                                           */
// *   ---     ---     ---     ---     ---  *
/* CONTRIBUTORS                             *
*     lyeb,                                 *
*                                           */
/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/

#include "KVRNEL/zjc_CommonTypes.h"
#include "KVRNEL/MEM/kvr_str.h"
#include "KVRNEL/MEM/kvr_bin.h"
#include "KVRNEL/lymath.h"

#include "es_read.h"

//   ---     ---     ---     ---     ---

static uchar COML[65];
static uchar ECHL[97];

//   ---     ---     ---     ---     ---

void PRKB(char ch)                          {

    char* pad = "   "; char* sign="";

    if  (100 >  ch && ch > 9) { pad="  "; }
    elif(100 <= ch          ) { pad=" ";  };

    if  (0   <= ch          ) { sign=" "; };

    /*printf("%s%i%s   0x%08X", sign, ch, pad, kvr.kb.F);*/};

//   ---     ---     ---     ---     ---
