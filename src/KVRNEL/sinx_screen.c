/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    SINX_SCREEN                           *
*                                           *
*     -byte-buffer                          *
*     -stylish writes to out                *
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

#include "KVRNEL/MEM/kvr_str.h"

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <unistd.h>

//   ---     ---     ---     ---     ---

static cuint SCREEN_WIDTH  = 76;
static cuint SCREEN_HEIGHT = 24;

static MEM*  SINX_SCREEN   = NULL;

//   ---     ---     ---     ---     ---

void SINX_NTSCREEN(void)                    {
    SINX_SCREEN  = MKSTR                    ("SCREEN", SCREEN_WIDTH*SCREEN_HEIGHT, 1);

    char* row    =                          SINX_SCREEN->buff+SCREEN_WIDTH*0;
    strcpy(row, "\x1b[2;1H*------------------------*\n\b\0");

    row          =                          SINX_SCREEN->buff+SCREEN_WIDTH*1;
    strcpy(row, "\x1b[3;1H*         |______        *\n\b\0");

    row          =                          SINX_SCREEN->buff+SCREEN_WIDTH*2;
    strcpy(row, "\x1b[4;1H*         |      |       *\n\b\0");

    row          =                          SINX_SCREEN->buff+SCREEN_WIDTH*3;
    strcpy(row, "\x1b[5;1H*         |______|       *\n\b\0");

    row          =                          SINX_SCREEN->buff+SCREEN_WIDTH*4;
    strcpy(row, "\x1b[6;1H*------------------------*\n\b\0");

    CALOUT('E', "%s%s%s%s%s",

        SINX_SCREEN->buff+SCREEN_WIDTH*0,
        SINX_SCREEN->buff+SCREEN_WIDTH*1,
        SINX_SCREEN->buff+SCREEN_WIDTH*2,
        SINX_SCREEN->buff+SCREEN_WIDTH*3,
        SINX_SCREEN->buff+SCREEN_WIDTH*4

    );

};

//   ---     ---     ---     ---     ---

void SINX_DRAW(void)                        {

    char* row0   =                          SINX_SCREEN->buff+SCREEN_WIDTH*0;
    char* row1   =                          SINX_SCREEN->buff+SCREEN_WIDTH*1;
    char* row2   =                          SINX_SCREEN->buff+SCREEN_WIDTH*2;
    char* row3   =                          SINX_SCREEN->buff+SCREEN_WIDTH*3;

};

//   ---     ---     ---     ---     ---

void SINX_LOOP(void)                        {

    SINX_NTSCREEN();
    char  c     = '\0';
    float delta = 0.0f;

    int pos[2]={2,22};

    do {

        //KFRBEG();
        if(kbhit()) {

            c = (char) getch();

            if(c==0x73) {
                CALOUT('E', "\x1b[%i;%iH \b", pos[1], pos[0]);
                pos[1]++;
                CALOUT('E', "\x1b[%i;%iH@\b", pos[1], pos[0]);
                c=0x00;

            }

            elif(c==0x77) {
                CALOUT('E', "\x1b[%i;%iH \b", pos[1], pos[0]);
                pos[1]--;
                CALOUT('E', "\x1b[%i;%iH@\b", pos[1], pos[0]);
                c=0x00;

            };

            if(c==0x64) {
                CALOUT('E', "\x1b[%i;%iH \b", pos[1], pos[0]);
                pos[0]++;
                CALOUT('E', "\x1b[%i;%iH@\b", pos[1], pos[0]);
                c=0x00;

            }

            elif(c==0x61) {
                CALOUT('E', "\x1b[%i;%iH \b", pos[1], pos[0]);
                pos[0]--;
                CALOUT('E', "\x1b[%i;%iH@\b", pos[1], pos[0]);
                c=0x00;

            };
        };

        // SINX_DRAW();


    } while(c != '\r');

    DLMEM(SINX_SCREEN);                                                                     };

//   ---     ---     ---     ---     ---