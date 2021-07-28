// *   ---     ---     ---     ---     ---  *
// *  BLKMGK                                *
// *                                        *
// *  -spiritual succesor of dsm module     *
// *  -entry point for data packers         *
// *  -gets teh joj                         *
// *                                        *
// *   ---     ---     ---     ---     ---  *

#include "zjc_joj.h"
#include "../KVRNEL/kvr_paths.h"

#define BLKMGK_V    0.1
#define BLKMGK_DONE 0x444f4e45

//   ---     ---     ---     ---     ---

static uint DAF_SIZE_I;
static uint DAF_SIZE_D;
static uint DAF_COUNTER;

static JOJH DAF_JITEMS[ZJC_DAFSIZE];

//   ---     ---     ---     ---     ---

EXPORT void NTBLKMGK(char** pth_l)          {

    NTKVRPTH(pth_l);

    DAF_SIZE_I  = 0;
    DAF_COUNTER = 0;

    for(int x=0; x<ZJC_DAFSIZE; x++)        { DAF_JITEMS[x].name[0] = '\0';                 \
                                              DAF_JITEMS[x].dim     = 0;                    };

    CALOUT("BLKMGK v%f\n\b", BLKMGK_V);                                                     };

EXPORT void DLBLKMGK() { DLKVRPTH(); };

//   ---     ---     ---     ---     ---

EXPORT void UTJOJ(uint i, uint dim,
                  char* name      )         {

                                            // kiknit on first run
    if(!i)                                  { NTJOJENG(JOJ_ENCODE);                   };
    ENCJOJ                                  (dim, &DAF_SIZE_I                         );

                                            // save entry data
    for(int x=0; x<(KVR_IDK_WIDTH-1); x++)  { DAF_JITEMS[DAF_COUNTER].name[x]=*(name+x);
                                              if(*(name+x)=='\0') { break;            }     };

//   ---     ---     ---     ---     ---

    DAF_JITEMS[DAF_COUNTER].dim = dim;
    DAF_COUNTER++;

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

    else {                                  // kikdel on added run
        DLJOJENG                            (JOJ_DECODE                                 );
    };                                                                                       };

//   ---     ---     ---     ---     ---