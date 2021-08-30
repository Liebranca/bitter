/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    ES_CORETYPES                          *
*                                           *
*     -kvnslrat boxes                       *
*     -termo data                           *
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

#include "KVRNEL/TYPES/zjc_hash.h"
#include "SIN/__sin.h"

#include "es_CoreTypes.h"

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

//   ---     ---     ---     ---     ---

#define ES_INBUFF_WIDTH 64

static PEOP  PE_OPS[4];                     // container for ops
static HASH* GNAMES;                        // globals and built-ins
static HASH* LNAMES;                        // user-defined or runtime-generated values

//   ---     ---     ---     ---     ---

typedef struct ES_KVNSLRAT {                // it's a modern typewriter

    struct termios t;                       // gross termios stuff
    struct termios restore;                 // idem

    uchar  kb[ES_INBUFF_WIDTH];             // reader for input buff
    int    kbh;                             // chars waiting on input buff

    float  curt[4];                         // cursor transform
    float  chrt[4];                         // (next) char transform
    ustr8  chrd[4];                         // (next) char data

    uint   nchars_x;                        // x-size of window, in number of chars
    uint   nchars_y;                        // y-size of window, in number of chars

} KVR; static KVR kvr={0};

#include "es_builtins.paste"

//   ---     ---     ---     ---     ---

void NTPEOP(uint  idex  ,
            char* opcode,
            uint  data  ,
            STARK fun   )                   { PEOP* op   = PE_OPS+idex;                     \
                                              op->id     = IDNEW("FUN*", opcode);           \
                                              op->data.F = data;                            \
                                              op->fun    = fun;                             };

PEOP* GTPENOP(void)                         { return PE_OPS+0;                              };

//   ---     ---     ---     ---     ---

CHRSPRT MKCHRSPRT(char* buff, uint co)      {

    CHRSPRT sprite = {0};
    sprite.buff    = buff;
    sprite.length  = strlen(buff);
    sprite.current = 0.0f;

                                            // clamp position to screen
    sprite.co.x    = clampui                ((co&0x00FF), 1, kvr.nchars_x);
    sprite.co.y    = clampui                ((co&0xFF00), 1, kvr.nchars_y);

                                            // init draw buffer
    PLCHRSPRT                               (&sprite, 0.0f               );

    return sprite;                                                                          };

void PLCHRSPRT(CHRSPRT* sprite,
               float dirn     )             {

    if(sprite->current >= sprite->length) { // wrap around end
        sprite->current=0.0f;

    }

    elif(sprite->current < 0) {             // wrap around start
        sprite->current=sprite->length-1;

    };

    char c           = sprite->buff         [(uint) (sprite->current)      ];
    sprite->current += dirn;

                                            // load data into frame
    sprintf                                 (sprite->frame, "$:jmp %c %c \x02;>%c",
                                             sprite->co.x, sprite->co.y, c         );       };

//   ---     ---     ---     ---     ---

void NTKVR(uint chrsz)                      {

                                            // gross termios stuff
    tcgetattr                               (0, &kvr.t                );

    kvr.restore   = kvr.t;                  // chasm has event polling, so this is redundant
                                            // but it's nice to have in case we drop sdl

    kvr.t.c_lflag&=~ICANON;
    kvr.t.c_lflag&=~ECHO;

    tcsetattr                               (0, TCSANOW, &kvr.t       );

//   ---     ---     ---     ---     ---    // actual chasm stuff

                                            // init cursor/char vars
    float sc[2]; GTCHRSZ                    (sc, chrsz                );

    kvr.chrt[0] = kvr.curt[0] = sc[0];      // set initial x,y scale & position
    kvr.chrt[1] = kvr.curt[1] = -1.0;       // we just initialize next char and cursor
    kvr.chrt[2] = kvr.curt[2] = sc[1];      // to the same thing
    kvr.chrt[3] = kvr.curt[3] =  1.0;

                                            // zero-out chardata
    CLMEM2                                  (kvr.chrd, xBYTES(4, ustr8)       );

    uint ws[2]; GTSCRSZ(ws, chrsz);         // screen size calculation, needed for jumps
    kvr.nchars_x = ws[0];
    kvr.nchars_y = ws[1];

                                            // blank out the inbuff
    CLMEM2                                  (kvr.kb, ES_INBUFF_WIDTH          );

//   ---     ---     ---     ---     ---

                                            // global names
    GNAMES        = MKHASH                  (5, "GNAMES"                      );

                                            // for opcode-to-funptr fetch
    NTPEOP                                  (0x01, "jmp", 0x0001, &JMPCON     );
    NTPEOP                                  (0x02, "gb",  0x0001, &GBPE       );
    NTPEOP                                  (0x03, "wt",  0x0001, &WTPE       );

                                            // upload opcodes to gnames
    HASHSET                                 (GNAMES, byref(PE_OPS[1].id)      );
    HASHSET                                 (GNAMES, byref(PE_OPS[2].id)      );
    HASHSET                                 (GNAMES, byref(PE_OPS[3].id)      );            };

//   ---     ---     ---     ---     ---

void DLKVR()                                {
    DLMEM(GNAMES); tcsetattr(0, TCSANOW, &kvr.restore);                                     };

//   ---     ---     ---     ---     ---

float* GTKVRCURT(void)                      { return kvr.curt;                              };
float* GTKVRCHRT(void)                      { return kvr.chrt;                              };
ustr8* GTKVRCHRD(void)                      { return kvr.chrd;                              };

//   ---     ---     ---     ---     ---    // just like conio

int  kbhit(void)                            { ioctl(0, FIONREAD, &kvr.kbh); return kvr.kbh; };
void getch(void)                            { int remain=kvr.kbh; int x=0; while(remain) {  \
                                              kvr.kb[x]=getchar(); x++; remain--;           \
                                              if(x==ES_INBUFF_WIDTH) { break; } };          };

//   ---     ---     ---     ---     ---

void GTOP(PEOP** dst, char* key)            { STR_HASHGET(GNAMES, key, *dst, 0);            };

//   ---     ---     ---     ---     ---