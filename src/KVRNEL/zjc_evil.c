#include "ZJC_EVIL.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

//   ---     ---     ---     ---     ---

static       int    CALDEPTH   = 0;         // call depth counter
static       int    CALREG_I   = 0;         // current idex into locreg
static       int    ERRSTATE   = 0x00;      // global errorstate

static       FILE*  CALLOG;                 // handle to the log dump

static       DANG   CALREG[64];             // call register; a call dump

//   ---     ---     ---     ---     ---

void CALOUT(char *format, ...)              {

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

void __writoa  (int x, char* buff, int rdx) { _itoa(x, buff, rdx);                          };
int  __wrstrcmp(char* s0, char* s1        ) { return strcmp(s0, s1);                        };

DANG* __geterrloc(const char* p,
                  const char* f,

                  int         l)            {

    DANG* cal = &(CALREG[CALREG_I]);
    char callbranch[66]; callbranch[0]='\b'; callbranch[1]='\0';
    if(CALDEPTH) {

                                            // avoid recursion or you'll see this one
        if(CALDEPTH > 63)                   { printf("CALL DEPTH EXCEEDED!\n"); return NULL;};

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
    snprintf                                (cal->location, 255,
                                            "%s%i: <%s> on func %s line %i\n\b",
                                            callbranch, CALREG_I, p, f, l    );

    CALREG_I++;                             // move to next entry in register
    return cal;                                                                              };

void __printcalreg(int flush)               {

    for(int i = 0; i < CALREG_I; i++)       { CALOUT("%s", CALREG[i].location);
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

    if (errcode < 0x40)                     { mstart = "FATAL EXCEPTION"; ERRSTATE = FATAL; }
    else                                    { mstart = "EXCEPTION";       ERRSTATE = ERROR; };
    CALOUT                                  ("%s [ERRCODE %u]: ", mstart, errcode           );

    switch(errcode)                         {

        case  0: mbody =                    "Insuficcient memory (%s requested).";      break;

        // NOTE START
        /* these are from an old version  *
         * and were never used. i leave   *
         * them because they amuse me.    */
        case  1: mbody =                    "Access violation. %s";                     break;
        case  2: mbody =                    "The end times have come. %s";              break;
        case  3: mbody =                    "You just did something illegal. %s";       break;
        // NOTE END

        case 64: mbody =                    "Couln't open file <%s>";                   break;
        case 65: mbody =                    "File couldn't be closed <%s>";             break;
        case 66: mbody =                    "Error writting to file <%s>";              break;
        case 67: mbody =                    "Error reading from file <%s>.";            break;

        case 68: mbody =                    "Inappropriate file signature <%s>";        break;
        case 69: mbody =                    "Error deleting file <%s>";                 break;

        case 70: mbody =                    "Stack <%s> is full; can't push";           break;
        case 71: mbody =                    "Stack <%s> is empty; can't pop";           break;
        case 72: mbody =                    "Can't insert key <%s>";                    break;
        case 73: mbody =                    "Key <%s> not found";                       break;

        default: mbody =                    "UNRECOGNIZED %s";                          break;

                                            };

    CALOUT                                  (mbody, info                                    );
    CALOUT                                  ("\n\bERRLOC TRACKER: %i ENTRIES\n", CALREG_I   );

    __printcalreg(0);

    if(ERRSTATE == FATAL)                   { CALOUT("TERMINATED"); exit(ERRSTATE);         }
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