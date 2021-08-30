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

#include "KVRNEL/TYPES/zjc_hash.h"
#include "SIN/REND/sin_canvas.h"

#include "es_CoreTypes.h"

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

//   ---     ---     ---     ---     ---

typedef struct PESO_MACHINE {

    MEM   m;                                // mem header

    char  rdop[PESO_OPWIDTH];               // read buffer
    ustr8 args;                             // args container

    uint  state;                            // state bitflags
    STK   stack;                            // loaded codes; push on lvlup pop on lvldw

} PECON;

static PECON*  PCON;
static HASH*   GNAMES;
static CHRSPRT UI_SPRITES[1];

//   ---     ---     ---     ---     ---

void NTPCON(void)                           {

    NTKVR                                   (1                                 );

    ID id    = IDNEW                        ("PE$O", "_MACHINE"                );
    MEMGET                                  (PECON, PCON                       ,
                                             xBYTES(PESO_STKSIZE, uint), &id   );

                                            // init the callstack
    MKSTK                                   (byref(PCON->stack), PESO_STKSIZE  );

                                            // init sprites for ui
    UI_SPRITES[0] = MKCHRSPRT               ("\xA9\xAA\xAB\xAC\xAD\xAE\xAF\xB0",
                                             0xFF01                            );
                                                                                            };

//   ---     ---     ---     ---     ---

void DLPCON(void)                          { DLMEM(PCON); DLKVR();                          };

//   ---     ---     ---     ---     ---

void RDCON(uchar** st  ,
           uchar  last ,
           ustr8* d    ,
           float* t    )                    {

//   ---     ---     ---     ---     ---

    PEOP* op   = GTPENOP();                 // container for embedded hooks
    ustr8 arg  = {0};                       // holds args for those hooks
    uint  lap  = 0;                         // ntimes the read loop was entered
    uint  flg  = 0;                         // flags set by the read loop

    int lstate = 0;                         // 1 if jumping back from a escape read

//   ---     ---     ---     ---     ---

    VERIFY_PE_STATE:
    d[0].x=(**st);
    switch(d[0].x) {                        // check current state

        case 0x00: PCON->state &=~PESO_RUNNING; break;
        case 0x24: PCON->state |= PESO_ESCAPED * ((*((*st)+1))==0x3A); break;
        case 0x3B: PCON->state &=~PESO_ESCAPED * ((*((*st)+1))==0x3E); break;

        case 0xB4: t[3]+=t[2]*0.5; break; // acute
        case 0x60: t[3]+=t[2]*0.5; break; // grave
        case 0x5E: t[3]+=t[2]*0.5; break; // circum
        case 0x7E: t[3]+=t[2]*0.5; break; // tilde

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

//   ---     ---     ---     ---     ---

    if(PCON->state & PESO_ESCAPED) {        // process current state

        (*st)+=2; d[0].x=(**st); lstate=1;

        switch(d[0].x) {
            case 0x00: return;
            case 0x25: PCON->state |= PESO_ESCREAD; (*st)++; break;
            case 0x2F: PCON->state &=~PESO_ESCREAD; (*st)++; break;

            default  : break;

        };

//   ---     ---     ---     ---     ---    // read sequence token by token

        uint i=0;
        for(flg=0; !flg; flg=PESO_RDRULE(st)) {
            if(!lap) { PCON->rdop[i]=(**st); i++;     };
            if( lap) { arg.V[lap-1]+=(**st);          };
            (*st)++; if(**st==0x20) { (*st)++; lap++; };

        }; PCON->rdop[i]='\0';

//   ---     ---     ---     ---     ---

                                            // get op matching key from gnames
        CALOUT                              (P, "\e[60;48HFLG 0x%02X", flg       );
        GTOP                                (&op, PCON->rdop                     );
        CALOUT                              (P, "\e[60;0H0x%" PRIXPTR "\e[60;10H:\
                                                 \e[60;12HPE_OPCODE %s"          ,
                                                 (uintptr_t) op, PCON->rdop      );

//   ---     ---     ---     ---     ---

        if(op) {                            // if match found, execute
            CALOUT                          (P, "\e[60;32H -> INS 0x%02X", op->data.x );
            CALOUT                          (P, "\e[61;0HARG 0x%02X", arg.F           );
            op->fun                         ((void*) &arg                             );

        }

        else {                              // else do nothing
            CALOUT                          (P, "\e[60;32H -> CANNOT FETCH"           );

        }; goto VERIFY_PE_STATE;            // check end of sequence
    }

//   ---     ---     ---     ---     ---

    else {
        switch(last) {                      // printing

            case 0x00: return;
            case 0x0A: t[1]=(-1); t[3]-=(t[2]*1.5); break;
            case 0x08: t[1]-=t[0]; break;

            case 0xB4: t[3]-=t[2]*0.5; break; // acute
            case 0x60: t[3]-=t[2]*0.5; break; // grave
            case 0x5E: t[3]-=t[2]*0.5; break; // circum
            case 0x7E: t[3]-=t[2]*0.5; break; // tilde

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


uchar* es_notice="\
$:jmp \x01 \x01 \x02;>\
\xFF" "ES" "\x7F" "SHELL\xFF\n\
";

void HICON(void)                            {

                                            // begin pushing chars
    BEGPSH                                  (                         );

    uchar* tstr = es_notice;
    uchar  last ='\0';

    float* t    = GTKVRCHRT();              // get cursor/next char values
    ustr8* d    = GTKVRCHRD();

    uint   i    = 0;                        // pass index

//   ---     ---     ---     ---     ---

    ES_DRAW:                                // draw next pass

        do {                                // process input and push
            RDCON                           (&tstr, last, d, t        );
            PSHCHR                          (t, d                     );

            last=d[0].x;                    // save last for scp-chk

        } while(*tstr++); i++;

//   ---     ---     ---     ---     ---

    switch(i) {                             // get next pass
        case 1 : tstr=UI_SPRITES[0].frame;      goto ES_DRAW;
        case 2 : tstr="$:jmp \x01 \x01 \x02;>"; goto ES_DRAW;

        default: break;

    }; PLCHRSPRT(UI_SPRITES+0, 0.5);

//   ---     ---     ---     ---     ---

    d[0].x=226;                             // draw cursor and close
    ENDPSH                                  (d                        );                    };

//   ---     ---     ---     ---     ---