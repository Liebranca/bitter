/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    ESP_READ                              *
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

#include "zjc_CommonTypes.h"
#include "zjc_evil.c"
#include "zjc_id.c"
#include "kvr_mem.c"
#include "kvr_str.c"
#include "kvr_bin.c"
#include "lymath.h"

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

//   ---     ---     ---     ---     ---

#ifndef AVTO_WSIZ_X
#define AVTO_WSIZ_X 96
#endif

#ifndef AVTO_WSIZ_Y
#define AVTO_WSIZ_Y 32
#endif

//   ---     ---     ---     ---     ---


typedef struct AVTO_KVRSOR {

    uint x;
    uint y;

} KVRSR; static KVRSR kvrsr = {1,1};

//   ---     ---     ---     ---     ---

static uchar COML[65];
static uchar ECHL[97];

void JMPKVRSR(uint x, uint y)               {

    kvrsr.y=y%AVTO_WSIZ_Y;
    kvrsr.x=x%AVTO_WSIZ_Y;

    snprintf                                (COML, 64, "\e[%u;%uH", kvrsr.y, kvrsr.x);
    snprintf                                (ECHL, 96, "bash -c 'echo -en \"%s\"'", COML);
    system                                  (ECHL                                   );      };

//   ---     ---     ---     ---     ---

typedef struct LYES_KVNSLRAT {

    struct termios t;
    struct termios restore;

    ustr8  kb;
    int    kbh;

} KVR; static KVR kvr={0};

//   ---     ---     ---     ---     ---

void NTKVR()                                { tcgetattr(0, &kvr.t); kvr.restore=kvr.t;
                                              kvr.kb.F=0;

                                              kvr.t.c_lflag&=~ICANON;
                                              kvr.t.c_lflag&=~ECHO;

                                              tcsetattr(0, TCSANOW, &kvr.t);
                                                                                            };

void DLKVR()                                { tcsetattr(0, TCSANOW, &kvr.restore);          };

int kbhit()                                 { ioctl(0, FIONREAD, &kvr.kbh); return kvr.kbh; };

//   ---     ---     ---     ---     ---

void PRKB(char ch)                          {

    char* pad = "   "; char* sign="";

    if  (100 >  ch && ch > 9) { pad="  "; }
    elif(100 <= ch          ) { pad=" ";  };

    if  (0   <= ch          ) { sign=" "; };

    printf("%s%i%s   0x%08X", sign, ch, pad, kvr.kb.F);                                     };

void RDKB(void)                             {

    int remain=kvr.kbh; int x=0;
    while(remain) {
        kvr.kb.V[x]=getchar(); x++; remain--;

    }; PRKB(kvr.kb.V[x-1]);

};

//   ---     ---     ---     ---     ---

char* ESP_LOGO = "\e[31m\
 *-------------------*------------------------------* \n\
 |                   | KVR * ESPECTRO               | \n\
 | * * *     * ** *  |                              | \n\
 | *$$$*   *$$$$$*   | termo for champs             | \n\
 | *$$$* *$$$$ *     |                              | \n\
 | *$$$*$$$$ *       | -license   :  GNU GPL3       | \n\
 | *$$$$$$$*         | -languages :  C, BASH, PE$O  | \n\
 | *$$$$$*           | -depends   :  ALL THINGS GNU | \n\
 | *$$$$$$$*         | -developers:  lyeb,(=)       | \n\
 | *$$$*$$$$$*       |                              | \n\
 | *$$$*  *$$$$$*    |                              | \n\
 | *$$$*    *$$$$$*  |                              | \n\
 | * * *      * ** * |                              | \n\
 |                   |                              | \n\
 *-------------------*------------------------------* \n\
\e[0m";

//   ---     ---     ---     ---     ---

void main(void) {

    printf("\e[1;1H%s", ESP_LOGO);

    NTKVR(); int i=0; char* clck=".-^~";

    do {

        kvr.kb.F=0;

        // arrows x__KK-5B1B
        // ord    x__KK-??C2

        if(kbhit()) {

            printf("Width: %i\e[32;16H", kvr.kbh);
            RDKB();

        }; usleep(100000); printf("\e[32;96H%c\e[32;1H", clck[i%4]); i++; fflush(stdout);

    } while(kvr.kb.x!=10);

    DLKVR();                                                                                };

//   ---     ---     ---     ---     ---
