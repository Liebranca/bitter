/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    ES_PECON                              *
*                                           *
*     -behaviours of a PE$O machine         *
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

#include "KVRNEL/MEM/kvr_mem.h"
#include "KVRNEL/lymath.h"

#include "KVRNEL/TYPES/zjc_hash.h"

#include "SIN/__sin.h"
#include "SIN/REND/sin_canvas.h"

#include <string.h>
#include <inttypes.h>

//   ---     ---     ---     ---     ---

#define PESO_RUNNING 0x01
#define PESO_ESCAPED 0x02
#define PESO_ESCREAD 0x04

#define PESO_OPWIDTH 64
#define PESO_STKSIZE 256

//   ---     ---     ---     ---     ---

#define PESO_RDRULE(s)         \
        ( ((**s == 0x00)     ) \
        | ((**s == 0x3B) << 1) )

#define PESO_RDNULL(x) (x & 0x01)
#define PESO_RDSCLN(x) (x & 0x02)

//   ---     ---     ---     ---     ---

void NOPE(void* nope) {
    CALOUT(P, "\e[67;0HYou nope!");

};

void GBPE(void* buff) {
    ustr8 data = *((ustr8*) buff);
    CALOUT(P, "\e[67;0HYou gb 0x%08X", data.F);

};

void WTPE(void* buff) {
    CALOUT(P, "\e[67;0HYou wt!");

};

//   ---     ---     ---     ---     ---

typedef struct PESO_OPERATION {

    ID    id;

    ustr8 data;
    STARK fun;

} PEOP; static PEOP PE_OPS[4];

void NTPEOP(PEOP* op,
            char* opcode,
            uint  data  ,
            STARK fun   )                   { op->id     = IDNEW("FUN*", opcode);           \
                                              op->data.F = data;                            \
                                              op->fun    = fun;                             };

//   ---     ---     ---     ---     ---

typedef struct PESO_MACHINE {

    MEM   m;                                // mem header

    char  rdop[PESO_OPWIDTH];
    ustr8 args;

    uint  state;                            // state bitflags
    STK   stack;                            // loaded codes; push on lvlup pop on lvldw

} PECON;

static PECON* PCON;
static HASH*  GNAMES;

//   ---     ---     ---     ---     ---

void NTPCON(void)                          {

                                            // global names
    GNAMES        = MKHASH                  (5, "GNAMES"                      );

                                            // for opcode-to-funptr fetch
    NTPEOP                                  (PE_OPS+1, "np", 0x0000, &NOPE    );
    NTPEOP                                  (PE_OPS+2, "gb", 0x0101, &GBPE    );
    NTPEOP                                  (PE_OPS+3, "wt", 0x0002, &WTPE    );

                                            // upload opcodes to gnames
    HASHSET                                 (GNAMES, byref(PE_OPS[1].id)      );
    HASHSET                                 (GNAMES, byref(PE_OPS[2].id)      );
    HASHSET                                 (GNAMES, byref(PE_OPS[3].id)      );

    ID id    = IDNEW                        ("PE$O", "_MACHINE"               );
    MEMGET                                  (PECON, PCON                      ,
                                             xBYTES(PESO_STKSIZE, uint), &id  );

                                            // init the callstack
    MKSTK                                   (byref(PCON->stack), PESO_STKSIZE);            };

//   ---     ---     ---     ---     ---

void DLPCON(void)                          { DLMEM(PCON); DLMEM(GNAMES);                    };

//   ---     ---     ---     ---     ---

void RDCON(char** st  ,
           char   last,
           ustr8* d   ,
           float* t   )                     {

//   ---     ---     ---     ---     ---

    PEOP* op   = PE_OPS+0;
    ustr8 arg  = {0};
    uint  lap  = 0;
    uint  flg  = 0;
    int lstate = 0;

//   ---     ---     ---     ---     ---

    VERIFY_PE_STATE:
    d[0].x=(**st);
    switch(d[0].x) {                        // check current state

        case 0x00: PCON->state &=~PESO_RUNNING; break;
        case 0x24: PCON->state |= PESO_ESCAPED * ((*((*st)+1))==0x3A); break;
        case 0x3B: PCON->state &=~PESO_ESCAPED * ((*((*st)+1))==0x3E); break;

        default  : break;

    };

    if(lstate && !(PCON->state & PESO_ESCAPED)) {
        lstate=0; (*st)+=2; d[0].x=(**st);

    };

//   ---     ---     ---     ---     ---

    CALOUT(P, "\e[56;0HPE_RUNNING %d\
               \e[57;0HPE_ESCAPED %d\
               \e[58;0HPE_ESCREAD %d"      ,

                 PCON->state & PESO_RUNNING,
                 PCON->state & PESO_ESCAPED,
                 PCON->state & PESO_ESCREAD);

    /*if(PCON->state & PESO_ESCREAD) {
        INCSTK(&(PCON->stack));

    };*/

//   ---     ---     ---     ---     ---

    if(PCON->state & PESO_ESCAPED) {       // process current state

        (*st)+=2; d[0].x=(**st); lstate=1;

        switch(d[0].x) {
            case 0x00: return;
            case 0x25: PCON->state |= PESO_ESCREAD; (*st)++; break;
            case 0x2F: PCON->state &=~PESO_ESCREAD; (*st)++; break;

            default  : break;

        };

//   ---     ---     ---     ---     ---

        uint i=0;
        for(flg=0; !flg; flg=PESO_RDRULE(st)) {
            if(!lap) { PCON->rdop[i]=(**st); i++;     };
            if( lap) { arg.V[lap-1]+=(**st);          };
            (*st)++; if(**st==0x20) { (*st)++; lap++; };

        }; PCON->rdop[i]='\0';

//   ---     ---     ---     ---     ---

        CALOUT(P, "\e[60;48HFLG 0x%02X", flg       );
        STR_HASHGET(GNAMES, PCON->rdop, op, 0      );
        CALOUT(P, "\e[60;0H0x%" PRIXPTR "\e[60;10H:\
                   \e[60;12HPE_OPCODE %s"          ,
                   (uintptr_t) op, PCON->rdop      );

//   ---     ---     ---     ---     ---

        if(op) {
            CALOUT(P, "\e[60;24H -> INS 0x%02X", op->data.x );

            /*if(PCON->state & PESO_ESCREAD) {
                STACKPUSH(byref(PCON->stack), 0);

            } else {
                STACKPOP(byref(PCON->stack), PCON->args.F);

            };*/

            CALOUT(P, "\e[61;0HARG 0x%02X", arg.F);
            op->fun((void*) &arg);

        } else { CALOUT(P, "\e[60;24H -> CANNOT FETCH"); };

        goto VERIFY_PE_STATE;
    }

//   ---     ---     ---     ---     ---

    else {
        switch(last) {                      // printing

            case 0x00: return;
            case 0x0A: t[1]=(-1); t[3]-=(t[2]*1.5); break;
            case 0x08: t[1]-=t[0]; break;

            default  : t[1]+=t[0]; break;

        }; CALOUT(P, "\e[68;0H\e[2KWRITING %c", d[0].x);

    };                                                                                      };

//   ---     ---     ---     ---     ---
/*
< ESPECTRO~GNU >\n\
~es-shell\n\
\n\
LIBRE SOFTWARE\n\
>Licenced under GPL3\n\
\n\
Copyleft (c) CIXXPAKK 2021\n\
*/


char* es_notice="\
\xFF" "es-shell\xFF\n\
$:gb \x24;>\
";

static uint anim_i=0;

void HICON(void)                            {

                                            // begin pushing chars
    BEGPSH                                  (                         );
    float sc[2]; GTCHRSZ                    (sc, 16                   );
    float t [4]  =                          { sc[0], -1.0, sc[1], 1.0 };
    ustr8 d [4]  =                          { 0,0,0,0                 };

    char* tstr = es_notice;
    char  last='\0';

    do {                                    // process input and push
        RDCON                               (&tstr, last, d, t        );
        PSHCHR                              (t, d                     );

        last=d[0].x;                        // save last for scp-chk

    } while(*tstr++);

    d[0].x=226;                             // draw cursor and close
    ENDPSH                                  (d                        );                    };

//   ---     ---     ---     ---     ---