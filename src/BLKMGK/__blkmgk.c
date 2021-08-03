/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    BLKMGK                                *
*                                           *
*     -spiritual succesor of dsm module     *
*     -entry point for data packers         *
*     -gets the joj                         *
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

#include "zjc_joj.h"
#include "zjc_crk.h"
#include "../KVRNEL/kvr_paths.h"

#define BLKMGK_V    0.1
#define BLKMGK_DONE 0x444f4e45

//   ---     ---     ---     ---     ---

static uint DAF_SIZE_I;
static uint DAF_SIZE_D;
static uint DAF_COUNTER;

static JOJH DAF_JITEMS[ZJC_DAFSIZE];
static CRKH DAF_CITEMS[ZJC_DAFSIZE];

//   ---     ---     ---     ---     ---

EXPORT void NTBLKMGK(char** pth_l)          {

    NTKVRPTH(pth_l);

    DAF_SIZE_I  = 0;
    DAF_COUNTER = 0;

                                            // wipe these arrays on init
    CLMEM2                                  (DAF_JITEMS, xBYTES(ZJC_DAFSIZE, JOJH));
    CLMEM2                                  (DAF_CITEMS, xBYTES(ZJC_DAFSIZE, CRKH));
    CALOUT                                  ('K', "BLKMGK v%f\n\b", BLKMGK_V      );        };

EXPORT void DLBLKMGK() { DLKVRPTH(); };

//   ---     ---     ---     ---     ---

EXPORT void UTCRK(uint i, uint vert_count,
                  uint idex_count,       \
                  uint cl, char* name    )  {

                                            // kiknit on first run
    if(!i)                                  { NTCRKENG(CRK_ENCODE);                   };

//   ---     ---     ---     ---     ---

                                            // save entry data
    for(int x=0; x<(KVR_IDK_WIDTH-1); x++)  { DAF_CITEMS[DAF_COUNTER].name[x]=*(name+x);
                                              if(*(name+x)=='\0') { break;            }     };

    DAF_CITEMS[DAF_COUNTER].vert_count = vert_count;
    DAF_CITEMS[DAF_COUNTER].idex_count = idex_count;
    DAF_CITEMS[DAF_COUNTER].fracl      = cl;

//   ---     ---     ---     ---     ---

                                            // set compression level and encode
    STCFRACL                                (cl                                       );
    ENCCRK                                  (DAF_CITEMS+DAF_COUNTER, &DAF_SIZE_I      );

    DAF_COUNTER++;

//   ---     ---     ---     ---     ---

    if(i==BLKMGK_DONE) {                    // zip and kikdel on last run
        ZPCRK                               (DAF_SIZE_I, DAF_COUNTER, DAF_CITEMS      );
        DLCRKENG                            (CRK_ENCODE                               );
    };                                                                                      };

//   ---     ---     ---     ---     ---

EXPORT void UTJOJ(uint i, uint dim,
                  uint cl, char* name)      {

                                            // kiknit on first run
    if(!i)                                  { NTJOJENG(JOJ_ENCODE);                   };

//   ---     ---     ---     ---     ---

                                            // save entry data
    for(int x=0; x<(KVR_IDK_WIDTH-1); x++)  { DAF_JITEMS[DAF_COUNTER].name[x]=*(name+x);
                                              if(*(name+x)=='\0') { break;            }     };

    DAF_JITEMS[DAF_COUNTER].dim   = dim;
    DAF_JITEMS[DAF_COUNTER].fracl = cl;

//   ---     ---     ---     ---     ---

                                            // set compression level and encode
    STCFRACL                                (cl                                       );
    ENCJOJ                                  (DAF_JITEMS+DAF_COUNTER, &DAF_SIZE_I      );

    DAF_COUNTER++;

//   ---     ---     ---     ---     ---

    if(i==BLKMGK_DONE) {                    // zip and kikdel on last run
        ZPJOJ                               (DAF_SIZE_I, DAF_COUNTER, DAF_JITEMS      );
        DLJOJENG                            (JOJ_ENCODE                               );
    };                                                                                      };

//   ---     ---     ---     ---     ---

EXPORT void INJOJ(uint i)                   {

    if(!i) {                                // kiknit and unzip on first run
        NTJOJENG                            ( JOJ_DECODE                                );
        uint sizes[3] =                     { DAF_SIZE_I, DAF_SIZE_D, DAF_COUNTER       };
        UZPJOJ                              ( DAF_JITEMS, sizes                         );

                                            // size values from header
        DAF_SIZE_I    =                     sizes[0];                                   \
        DAF_SIZE_D    =                     sizes[1];                                   \
        DAF_COUNTER   =                     sizes[2];                                   \

    };

//   ---     ---     ---     ---     ---

    if(i!=BLKMGK_DONE) {                    // decode joj (writes to pixdump)
        DECJOJ                              (DAF_JITEMS+i                               );
    }

//   ---     ---     ---     ---     ---

    else {                                  // kikdel on added run
        DLJOJENG                            (JOJ_DECODE                                 );
    };                                                                                       };

//   ---     ---     ---     ---     ---
