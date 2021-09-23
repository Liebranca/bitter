/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    MA_RD                                 *
*                                           *
*     -reads pe files                       *
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
#include "KVRNEL/kvr_paths.h"
#include "KVRNEL/MEM/kvr_str.h"
#include <stdio.h>
#include <string.h>

//   ---     ---     ---     ---     ---

#define MAMMIT_SF_PESC 0x00000001
#define MAMMIT_SF_PLIT 0x00000002

#define MAMMIT_SF_NREG 0x00000100
#define MAMMIT_SF_NDEC 0x00000200
#define MAMMIT_SF_NDEF 0x00000400

#define MAMMIT_SF_NCLN 0x00000800
#define MAMMIT_SF_NFUN 0x00001000

//   ---     ---     ---     ---     ---

typedef struct MAMM_INTERPRETER {           // smach for pe-text input

    uint state;

} MAMMIT;

//   ---     ---     ---     ---     ---

static int   iptr=0;
static char* types[] = {

    "int",
    "\0"

}; int isType(char* t) {
    while(strcmp(t, types[iptr])) { iptr++;
        if(!(types[iptr][0])) { iptr=0; return 0; }

    }; return 1;

};

//   ---     ---     ---     ---     ---

// reg: vardecls;
// hed: (pub) fundecls, clandecls;
// src: (prv) fundecls, clandecls;
// def: (all) fundefns, clandefns;

//   ---     ---     ---     ---     ---

static uchar  tokens[16][64];
static uchar* rd_tk   = tokens[0];
static uint   rd_tki  = 0;
static uint   rd_tkp  = 0;

static uchar* rd_buff = NULL;

static uchar  rd_cur  = '\0';
static uchar  rd_prv  = '\0';
static uint   rd_pos  = 0;

void VALIDATE_STATEMENT(void)               {

    for(uint x=0; x<rd_tkp; x++) {
        CALOUT(K, "%u: %s\n", x, tokens[x]);

    };

};

//   ---     ---     ---     ---     ---

void RDNXT(void)                            {

    TOP:
        rd_prv=rd_cur;                      // remember current
        rd_cur=rd_buff[rd_pos];             // advance to next

        rd_pos++;

    switch(rd_cur) {

//   ---     ---     ---     ---     ---    BLANKS 0-15

        case 0x00: return;                  // end on nullterm \0

        case 0x01: goto APTOK;              // skip control chars
        case 0x02: goto APTOK;              // some or all of these could be used in the future
        case 0x03: goto APTOK;              // for now they are meaningless, so skip all
        case 0x04: goto APTOK;              // keeping the cases for when the day comes
        case 0x05: goto APTOK;
        case 0x06: goto APTOK;
        case 0x07: goto APTOK;
        case 0x08: goto APTOK;
        case 0x09: goto APTOK;
        case 0x0A: goto APTOK;
        case 0x0B: goto APTOK;
        case 0x0C: goto APTOK;
        case 0x0D: goto APTOK;
        case 0x0E: goto APTOK;
        case 0x0F: goto APTOK;

//   ---     ---     ---     ---     ---    BLANKS 16-31

        case 0x10: goto APTOK;              // same as above, meaningless for now
        case 0x11: goto APTOK;              // reason why they're here, above as well
        case 0x12: goto APTOK;
        case 0x13: goto APTOK;
        case 0x14: goto APTOK;
        case 0x15: goto APTOK;
        case 0x16: goto APTOK;
        case 0x17: goto APTOK;
        case 0x18: goto APTOK;
        case 0x19: goto APTOK;
        case 0x1A: goto APTOK;
        case 0x1B: goto APTOK;
        case 0x1C: goto APTOK;
        case 0x1D: goto APTOK;
        case 0x1E: goto APTOK;
        case 0x1F: goto APTOK;

//   ---     ---     ---     ---     ---    BLANK 32

        case 0x20: APTOK:                   // for space and skipped (see above x2)

            if(!(  0x01 <= rd_prv \
                && 0x20 >= rd_prv ) ) {     // if previous is not *also* control char {1,32}

                rd_tkp = 0; rd_tki++;       // reset position && advance slot
                rd_tk  = tokens[rd_tki];    // push token

            }; goto TOP;                    // back up and read next char

//   ---     ---     ---     ---     ---    TERMINATORS

        case 0x3B: VALIDATE_STATEMENT();    // process token sequence and clean up

            for(uint i=0; i<16; i++) {
                CLMEM2(tokens[i], 64); rd_tki=0;

            }; rd_tk=tokens[rd_tki]; break;

//   ---     ---     ---     ---     ---    CHARACTERS

        default:                            // 'cat' char to cur token
            rd_tk[rd_tkp]=rd_cur; rd_tkp++; goto TOP;

    };                                                                                      };

//   ---     ---     ---     ---     ---

void main(void)                             {

    MEM* s=MKSTR("MAMM_RD", 1024, 1); CLMEM(s);

    RPSTR(&s, "this is a test;", 0);
    rd_buff=MEMBUFF(s, uchar, 0);

    RDNXT();
    DLMEM(s);

};

//   ---     ---     ---     ---     ---