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

static NIHIL MA_SCB_TABLE[8] = {            // table of state callbacks

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

    uint sf    = mammi->cntx;
    uint idex  = 0;

    for(uint x=0; x<8; x++) {
        uint b = !(sf&(1<<x));
        if(!b) { break; }

        idex  += b;

    }; return MA_SCB_TABLE[idex];                                                           };

//   ---     ---     ---     ---     ---

uint statement_count(void)                  {

    uchar* c       = rd_buff+rd_pos;
    uchar  last    = 0x00;

    uint   lvl     = mammi->lvla;
    uint   old_lvl = lvl; lvl++;

    uint   cnt     = 0;

//   ---     ---     ---     ---     ---

    while( (*c          ) \
    &&     (lvl!=old_lvl) ) {

        cnt += ((*c)==0x3B && last!=0x7D);

        last = *c;
        lvl += last==0x7B;
        lvl -= last==0x7D;

        c++;

    }; return cnt;                                                                          };

//   ---     ---     ---     ---     ---

#define CNTX_INIT_BOILER(T, whom, idtype)                                                    \
                                            /* fetch tokens */                               \
    rd_tkx++; uchar*  name   =               tokens[rd_tkx];                                 \
    rd_tkx++; uchar*  rwsize =               tokens[rd_tkx];                                 \
              MEMUNIT chsize =               1;                                              \
                                                                                             \
/*   ---     ---     ---     ---     ---    // string translation and sizings... */          \
                                                                                             \
    if(rwsize[0]) {                         /* if no mag specified, count ;s     */          \
        TRDECVAL                            (rwsize, &chsize                       );        \
                                                                                             \
    } else {                                                                                 \
        chsize               =              statement_count();                               \
                                                                                             \
    };                                                                                       \
                                            /* set bounds and inc lval top       */          \
    whom->bound              = GTUNITCNT    (sizeof(whom->bound), chsize           );        \
                                                                                             \
    whom->start = mammi->lvaltop;           /* set start idex                    */          \
    whom->size  = 0;                        /* cleanup, just in case             */          \
    whom->elems = 0;                        /*                                   */          \
                                                                                             \
                                                                                             \
    INCLVAL                                 ( (sizeof(T)                           )         \
                                            + ((whom->bound+1)*sizeof(whom->bound) ) );      \
                                                                                             \
                                            /* make id */                                    \
    whom->id                 = IDNEW        (idtype, name                          );        \
                                                                                             \
/*   ---     ---     ---     ---     ---    // point unused slots to frblk */                \
                                                                                             \
    for(uint x=0; x<whom->bound; x++) {                                                      \
        whom->jmpt[x]=FRBLK - whom->start;                                                   \
    }

//   ---     ---     ---     ---     ---

void REGMA(void)                            {

    if(!(mammi->state&MAMMIT_SF_CREG)) {    // if unset, do and ret

        mammi->state |= MAMMIT_SF_CREG;     // set state
        pe_reg        = (REG*) CURLVAL;     // claim place in stack

                                            // fooken boiler
        CNTX_INIT_BOILER                    (REG, pe_reg, "REG*");

//   ---     ---     ---     ---     ---    // ut something to console

        CALOUT                              (K, "reg %s[%u]\n", name, pe_reg->bound      );
        return;

    }; mammi->state &=~MAMMIT_SF_CREG;      // effectively, an implicit else
                                                                                            };

//   ---     ---     ---     ---     ---

void PROCMA(void)                           {
    if(!(mammi->state&MAMMIT_SF_CPRC)) {
        mammi->state|=MAMMIT_SF_CPRC;

        pe_proc = (PROC*) CURLVAL;

        pe_proc->alias_blk = 0;
        pe_proc->elems     = 0;

                                            // fooken boiler
        CNTX_INIT_BOILER                    (PROC, pe_proc, "POC*"                   );
        CALOUT                              (K, "proc %s[%u]\n", name, pe_proc->bound);

        return;

    }; mammi->state &=~MAMMIT_SF_CPRC;                                                      };

//   ---     ---     ---     ---     ---
