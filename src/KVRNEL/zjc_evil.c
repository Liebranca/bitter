#include "ZJC_EVIL.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <time.h>

//   ---     ---     ---     ---     ---

static       int    CALDEPTH = 0;         // call depth counter
static       int    CALREG_I = 0;         // current idex into locreg
static       int    ERRSTATE = 0x00;      // global errorstate

static       FILE*  CALLOG   = NULL;      // handle to the log dump

static       DANG   CALREG[64];             // call register; a call dump

//   ---     ---     ---     ---     ---

static char* PALETTE[] = {
    "$:MSCPX(PALETTE[0]);>", "$:MSCPX(PALETTE[1]);>",
    "$:MSCPX(PALETTE[2]);>", "$:MSCPX(PALETTE[3]);>",

    "$:MSCPX(PALETTE['BACK']);>",
    "$:MSCPX(PALETTE['SEL0']);>"

};

//   ---     ---     ---     ---     ---

void CALOUT(char* format, ...)              {

    va_list args;

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    };

//   ---     ---     ---     ---     ---

void __openlog()                            {

    // hardcoded for now. pass in ROOT later or something to make this a relative path
    CALLOG=freopen("D:\\lieb_git\\KVR\\trashcan\\log\\KVNSLOG", "w+", stderr);

    };

void __closelog() {

    fclose(CALLOG);

    };

//   ---     ---     ---     ---     ---

LKUP NITKEY    (char* key                 ) { LKUP lkp={key, -1, -1}; return lkp;           };
void __writoa  (int x, char* buff, int rdx) { _itoa(x, buff, rdx);                          };
int  __wrstrcmp(char* s0, char* s1        ) { return strcmp(s0, s1);                        };

DANG* __geterrloc(const char* p,
                  const char* f,

                  int         l)            {

    DANG* cal = &(CALREG[CALREG_I]);
    char callbranch[66]; callbranch[0]='\b'; callbranch[1]='\0';
    if(CALDEPTH) {

                                            // avoid recursion or you'll see this one
        if(CALDEPTH > 63)                   { CALOUT("%s", "CALL DEPTH EXCEEDED!\n"); return NULL;};

                                            /* fill callbranch so that it reads like so:   *
                                             *                                             *
                                             * > depth 0                                   *
                                             * |-> depth 1                                 *
                                             * |--> depth 2, etc.                          */

        callbranch[0           ] = '|';     // branch start
        callbranch[CALDEPTH + 1] = '>';     // branch end
        callbranch[CALDEPTH + 2] = '\0';    // null-terminate it

                                            // now fill between branch start and end
        for(int x = 1; x < CALDEPTH+1; x++) { callbranch[x] = '-';                          }
    }

    cal->depth = CALDEPTH;                  // assign current depth
    CALDEPTH++;                             // then increment

                                            // paste everything into the errloc itself
    snprintf(
        cal->location, 255,
        "%s%s%i:\x1b[0m %s<%s>\x1b[0m on func %s%s\x1b[0m line %s%i\n\b",
        PALETTE[1], callbranch, CALREG_I, PALETTE[0], p, PALETTE[2], f, PALETTE[3], l
        );

    CALREG_I++;                             // move to next entry in register
    return cal;                                                                              };

void __printcalreg(int flush)               {

    for(int i = 0; i < CALREG_I; i++)       { CALOUT("%s%s", PALETTE[4], CALREG[i].location);
        if (flush)                          { CALREG[i].location[0] = '\0';     };          };

    if(flush)                               { CALREG_I = 0;                     };          };

void __popcalreg()                          {

    if(CALREG_I)                            { CALREG_I--;

                                              CALREG[CALREG_I].location[0] = '\0';
                                              CALREG[CALREG_I].depth       = 0;             };

    if(CALDEPTH)                            { CALDEPTH--;                                   };
                                                                                            };

void __terminator (int errcode, char* info) {

    char* mstart; char* mbody;

    if (errcode < 0x40) { mstart = "$:MSCPX(PALETTE[3]);>FTL_"; ERRSTATE = FATAL; }
    else                { mstart = "$:MSCPX(PALETTE[2]);>ERR_"; ERRSTATE = ERROR; };

    CALOUT(

        "\n\b\x1b[7m%s0x%X\x1b[27m: %s",
        mstart, errcode, PALETTE[4]

    );

    switch(errcode)                         {

        case  0: mbody =                    "Insuficcient memory (%s requested).";      break;

        // NOTE START
        /* these are from an old version  *
         * and were never used. i leave   *
         * them because they amuse me.    */
        case  1: mbody =                    "Access violation. %s%s";                   break;
        case  2: mbody =                    "The end times have come. %s%s";            break;
        case  3: mbody =                    "You just did something illegal. %s%s";     break;
        // NOTE END

        case 64: mbody =                    "Couln't open file %s<%s>";                 break;
        case 65: mbody =                    "File couldn't be closed %s<%s>";           break;
        case 66: mbody =                    "Error writting to file %s<%s>";            break;
        case 67: mbody =                    "Error reading from file %s<%s>.";          break;

        case 68: mbody =                    "Inappropriate file signature %s<%s>";      break;
        case 69: mbody =                    "Error deleting file %s<%s>";               break;

        case 70: mbody =                    "Stack is full; can't push. %s%s";          break;
        case 71: mbody =                    "Stack is empty; can't pop. %s%s";          break;
        case 72: mbody =                    "Can't insert key %s<%s>";                  break;
        case 73: mbody =                    "Key %s<%s>\x1b[0m not found";              break;

        default: mbody =                    "UNRECOGNIZED %s%s";                        break;

                                            };

    CALOUT(mbody, PALETTE[0], info);
    CALOUT(
        "\n\b%sERRLOC TRACKER: %i ENTRIES\n",
        PALETTE[4], CALREG_I

    ); __printcalreg(0);

    CALOUT("\n\b");

    if(ERRSTATE == FATAL)                   { CALOUT("%s", "TERMINATED"); exit(ERRSTATE);   }
    else                                    { /*?*/;                                        };
                                                                                            };

//   ---     ---     ---     ---     ---

int qstoi(char* buff) {

    int x   = 0;
    int y   = 0;
    int i   = 0;

    int len = (int) strlen(buff);

    for(char* ptr=buff+(len-1); i<len; ptr--, i++)
    {
        y=((*ptr)-48);
        if(i) { y*=((int)pow(10, i)); }

        x+=y;

    } return x;                                                                             };

//   ---     ---     ---     ---     ---
/* random timing stuff, find somewhere    **
** else to drop this into. likely         **
** zjc_time when we get around to         **
** porting that from dsm                  */

static clock_t framestart;
static clock_t frameend;
static float   CPS=1.0f/CLOCKS_PER_SEC;

void  timer_start(void)                     { framestart = clock();                         };
float deltaCalc  (void)                     { return (frameend-framestart) * CPS;           };
float timer_end  (void)                     { frameend   = clock(); return deltaCalc();     };