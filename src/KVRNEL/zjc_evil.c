/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    ZJC_EVIL                              *
*                                           *
*     -debug stuff                          *
*     -hoh sys                              *
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

#include "zjc_evil.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

//   ---     ---     ---     ---     ---

static       int    CALDEPTH = 0;           // call depth counter
static       int    CALREG_I = 0;           // current idex into locreg
static       int    ERRSTATE = 0x00;        // global errorstate

static       DANG   CALREG[64];             // call register; a call dump

//   ---     ---     ---     ---     ---

char* __shpath(char* path)                  {
                                            // shortens filepath to everything after "/src/"
    char* file    = path;
    char* curchar = '\0';

    int src[3]    = { 0, 0, 0 };

    while (*path++)                         // iter through string
    {   curchar = path;                     /* check if current char is a slash            *
                                             * if so, check that next three match "src"    */
        if (  (*curchar == '\\')
           || (*curchar == '/' )  )         { src[0] = *(path+1) == 's';
                                              src[1] = *(path+2) == 'r';
                                              src[2] = *(path+3) == 'c';

                                            // shorten file string on success
            if(src[0] && src[1] && src[2])  { file = path+5; break; }                       };

    } return file;                          /* original path is returned on failure */      };

//   ---     ---     ---     ---     ---

static char* PALETTE[] = {
    "\e[38;2;0;255;255m\e[48;2;23;30;35m",
    "\e[38;2;0;255;0m\e[48;2;23;30;35m",
    "\e[38;2;255;255;0m\e[48;2;23;30;35m",
    "\e[38;2;255;192;128m\e[48;2;23;30;35m",

    "\e[38;2;192;206;192m\e[48;2;23;30;35m",
    "\e[38;2;192;206;192m\e[48;2;0;0;0m"

};

//   ---     ---     ---     ---     ---

static char CALOUT_LAST=0x4D;

void __calout(char fam, char* format, ...)  {

                                            // newline on fambreak
    if(fam!=CALOUT_LAST)                    { CALOUT_LAST=fam; fprintf(stderr, "\n");     };

    va_list args;

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    };

//   ---     ---     ---     ---     ---

void __writoa  (int x, char* buff, int rdx) { sprintf(buff, "%i", x);                       };
int  __wrstrcmp(char* s0, char* s1        ) { return strcmp(s0, s1);                        };

DANG* __geterrloc(const char* p,
                  const char* f,

                  int         l)            {

    DANG* cal = &(CALREG[CALREG_I]);
    char callbranch[66]; callbranch[0]='\b'; callbranch[1]='\0';
    if(CALDEPTH) {

                                            // avoid recursion or you'll see this one
        if(CALDEPTH > 63)                   { CALOUT(E,"%s", "CALL DEPTH EXCEEDED!\n");   \
                                              return NULL;                                  };

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
        "%s%s%i:\x1b[0m %s<%s>\x1b[0m on func %s%s\x1b[0m line %s%i\e[0m\n",
        PALETTE[1], callbranch, CALREG_I, PALETTE[0], p, PALETTE[2], f, PALETTE[3], l
        );

    CALREG_I++;                             // move to next entry in register
    return cal;                                                                              };

void __printcalreg(int flush)               {

    for(int i = 0; i < CALREG_I; i++)       { CALOUT(E, "%s%s",                 \
                                              PALETTE[4], CALREG[i].location    );
        if (flush)                          { CALREG[i].location[0] = '\0';     };          };

    if(flush)                               { CALREG_I = 0;                     };          };

void __popcalreg()                          {

    if(CALREG_I)                            { CALREG_I--;

                                              CALREG[CALREG_I].location[0] = '\0';
                                              CALREG[CALREG_I].depth       = 0;             };

    if(CALDEPTH)                            { CALDEPTH--;                                   };
                                                                                            };

void __terminator (int errcode, char* info) {

    char mstart[256]; char* mbody;
                           
    if (errcode < 0x40) { snprintf(mstart, 255, "%sFTL_", PALETTE[3]); ERRSTATE = FATAL; }
    else                { snprintf(mstart, 255, "%sERR_", PALETTE[2]); ERRSTATE = ERROR; };

    CALOUT(E                              ,
           "\n\x1b[7m%s0x%X\x1b[27m: %s",
           mstart, errcode, PALETTE[4]    );

    switch(errcode)                         {

        case  0: mbody =                    "Insuficcient memory %s(%s requested)";     break;

        // NOTE START
        /* these are from an old version  *
         * and were never used. i leave   *
         * them because they amuse me.    */
        case  1: mbody =                    "Access violation %s%s";                    break;
        case  2: mbody =                    "The end times have come %s%s";             break;
        case  3: mbody =                    "You just did something illegal %s%s";      break;
        // NOTE END

        case  4: mbody =                    "ZLIB went PLOP %s(status %s)";             break;

        case 64: mbody =                    "Couln't open file %s<%s>";                 break;
        case 65: mbody =                    "File couldn't be closed %s<%s>";           break;
        case 66: mbody =                    "Error writting to file %s<%s>";            break;
        case 67: mbody =                    "Error reading from file %s<%s>.";          break;

        case 68: mbody =                    "Inappropriate file signature %s<%s>";      break;
        case 69: mbody =                    "Error deleting file %s<%s>";               break;

        case 70: mbody =                    "Stack is full; can't push %s%s";           break;
        case 71: mbody =                    "Stack is empty; can't pop %s%s";           break;
        case 72: mbody =                    "Can't insert key %s<%s>";                  break;
        case 73: mbody =                    "Key %s<%s>\x1b[0m not found";              break;

        // 0x010000 onwards is PE$O debug
        case 0x010001:
            mbody =                         "Multiple declarations of value %s'%s'";    break;

        case 0x010002:
            mbody =                         "Oversized value for type %s%s";            break;

        case 0x010003:
            mbody =                         "Element %s%s\e[0m is not an address-of";   break;

        default: mbody =                    "UNRECOGNIZED %s%s";                        break;

                                            };

    CALOUT(E, mbody, PALETTE[0], info);
    CALOUT(E,
        "\n%sERRLOC TRACKER: %i ENTRIES\n",
        PALETTE[4], CALREG_I

    ); __printcalreg(0);

    CALOUT(E, "\n");

    if(ERRSTATE == FATAL)                   { CALOUT(E, "%s", "TERMINATED");                \
                                              exit(ERRSTATE);                               }
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