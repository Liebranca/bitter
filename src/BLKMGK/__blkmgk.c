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

#define BLKMGK_KRUN 0x4B52554E
#define BLKMGK_DONE 0x444F4E45

//   ---     ---     ---     ---     ---

static HRW  DAF_DATA = {0};
static JOJH DAF_JITEMS[ZJC_DAFSIZE];
static CRKH DAF_CITEMS[ZJC_DAFSIZE];

//   ---     ---     ---     ---     ---

EXPORT void NTBLKMGK(char** pth_l)          {

    NTKVRPTH(pth_l);

                                            // wipe these arrays on init
    CLMEM2                                  (&DAF_DATA,   sizeof(HRW)             );
    CLMEM2                                  (DAF_JITEMS, xBYTES(ZJC_DAFSIZE, JOJH));
    CLMEM2                                  (DAF_CITEMS, xBYTES(ZJC_DAFSIZE, CRKH));

    CALOUT                                  ('K', "BLKMGK v%f\n\b", BLKMGK_V      );        };

EXPORT void DLBLKMGK()                      { DLKVRPTH();                                   };
EXPORT void STDAFTOT(uint v)                { DAF_DATA.total=v;                             };

//   ---     ---     ---     ---     ---

EXPORT void UTCRK(uint i, uint vert_count,
                  uint idex_count,       \
                  uint cl, char* name    )  {

                                            // kiknit on first run
    if(!i)                                  { NTCRKENG(CRK_ENCODE);                   };

//   ---     ---     ---     ---     ---

                                            // save entry data
    for(int x=0; x<(ZJC_IDK_WIDTH-1); x++)  { DAF_CITEMS[DAF_DATA.idex].name[x]=*(name+x);
                                              if(*(name+x)=='\0') { break;              }   };

    DAF_CITEMS[DAF_DATA.idex].vert_count = vert_count;
    DAF_CITEMS[DAF_DATA.idex].idex_count = idex_count;
    DAF_CITEMS[DAF_DATA.idex].fracl      = cl;

//   ---     ---     ---     ---     ---

                                            // set compression level and encode
    STCFRACL                                (cl                                        );
    ENCCRK                                  (DAF_CITEMS+DAF_DATA.idex, &DAF_DATA.size_i);

    DAF_DATA.idex++;

//   ---     ---     ---     ---     ---

    if(i==BLKMGK_DONE) {                    // zip and kikdel on last run
        ZPCRK                               (DAF_DATA.size_i, DAF_DATA.idex, DAF_CITEMS);
        DLCRKENG                            (CRK_ENCODE                                );
    };                                                                                      };

//   ---     ---     ---     ---     ---

EXPORT void BMENCJOJ(uint i, uint tot)      {

                                            // kiknit on first run
    if  (i==BLKMGK_KRUN)                    { NTWTBUF(); NTJOJENG(JOJ_ENCODE);         };

    elif(i==BLKMGK_DONE) {                  // zip and kikdel on last run
        ZPJOJ                               (DAF_DATA.size_i, DAF_DATA.idex, DAF_JITEMS);
        DLWTBUF                             (                                          );
        DLJOJENG                            (JOJ_ENCODE                                );
    };                                                                                      };

//   ---     ---     ---     ---     ---

EXPORT void UTJOJ(uint dim, uint cl     ,
                  char* name, float* src)   {

                                            // save entry data
    for(int x=0; x<(ZJC_IDK_WIDTH-1); x++)  { DAF_JITEMS[DAF_DATA.idex].name[x]=*(name+x);
                                              if(*(name+x)=='\0') { break;              }   };

    DAF_JITEMS[DAF_DATA.idex].dim   = dim;
    DAF_JITEMS[DAF_DATA.idex].fracl = cl;

//   ---     ---     ---     ---     ---

                                            // set compression level and encode
    STCFRACL                                (cl                                         );
    ENCJOJ                                  (src, DAF_JITEMS+DAF_DATA.idex, &DAF_DATA   );

    DAF_DATA.idex++;                                                                        };

//   ---     ---     ---     ---     ---

EXPORT void INJOJ(uint i)                   {

    if(!i) {                                // kiknit and unzip on first run
        NTJOJENG                            ( JOJ_DECODE                                );
        uint sizes[4] =                     { DAF_DATA.size_i, DAF_DATA.size_d,         \
                                              DAF_DATA.idex, 0                          };

        UZPJOJ                              ( DAF_JITEMS, sizes                         );

                                            // size values from header
        DAF_DATA.size_i =                   sizes[0];                                   \
        DAF_DATA.size_d =                   sizes[1];                                   \
        DAF_DATA.idex   =                   sizes[2];                                   \

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
