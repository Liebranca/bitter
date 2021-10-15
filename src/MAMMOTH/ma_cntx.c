/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    MA_CNTX                               *
*                                           *
*     -flips switches                       *
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

#include "ma_cntx.h"
#include "ma_trans.h"
#include <string.h>
#include <math.h>

//   ---     ---     ---     ---     ---

static NIHIL MA_SCB_TABLE[9] = {            // table of state callbacks

    NULL,                                   // NON              0x00000000

    &REGMA,                                 // MAMMIT_SF_CREG   0x00000100
    NULL,                                   // MAMMIT_SF_CDEC   0x00000200
    NULL,                                   // MAMMIT_SF_CDEF   0x00000400
    NULL,                                   // MAMMIT_SF_CCLA   0x00000800
    &PROCMA,                                // MAMMIT_SF_CPRC   0x00001000
    NULL,                                   // MAMMIT_SF_____   0x00002000
    NULL,                                   // MAMMIT_SF_____   0x00004000
    NULL                                    // MAMMIT_SF_____   0x00008000

};

//   ---     ---     ---     ---     ---

NIHIL STOCB(void)                           {

    uint sf   = mammi->state >> 8;
    uint idex = 0;

    for(uint x=0; x<8; x++) {
        idex += sf&(1<<x);

    }; return MA_SCB_TABLE[idex];                                                           };

//   ---     ---     ---     ---     ---

void REGMA(void)                            {

    if(!(mammi->state&MAMMIT_SF_CREG)) {    // if unset, do and ret

        mammi->state |= MAMMIT_SF_CREG;     // set state

                                            // claim place in stack
        pe_reg                      =       (REG*) CURLVAL;

                                            // fetch tokens
        rd_tkx++; uchar*  name      =       tokens[rd_tkx];
        rd_tkx++; uchar*  rwsize    =       tokens[rd_tkx];
                  MEMUNIT chsize    =       0;

        TRDECVAL                            (rwsize, &chsize, UNITSZ                     );

//   ---     ---     ---     ---     ---

        pe_reg->bound      = (uint) chsize;

        while((pe_reg->bound*sizeof(uint))%UNITSZ) {
            pe_reg->bound++;

        };

        pe_reg->id         = IDNEW          ("REG*", name                                );

        pe_reg->size       = 0;             // cleanup, just in case
        pe_reg->elems      = 0;

                                            // set start idex of this block and inc to next
        pe_reg->start      =                mammi->lvaltop;
        INCLVAL                             (sizeof(REG)+((pe_reg->bound+1)*sizeof(uint)));

//   ---     ---     ---     ---     ---

                                            // ut something to console
        CALOUT                              (K, "reg %s[%u]\n", name, pe_reg->bound      );

        return;

    }; CALOUT(K, "UT REG\n");
mammi->state &=~MAMMIT_SF_CREG;      // effectively, an implicit else
                                                                                            };

//   ---     ---     ---     ---     ---

void PROCMA(void)                           {
    ;

};

//   ---     ---     ---     ---     ---
