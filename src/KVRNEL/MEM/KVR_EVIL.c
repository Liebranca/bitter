#include "KVR_EVIL.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

//   ---     ---     ---     ---     ---

static       int  CALDEPTH   = 0;           // call depth counter
static       int  CALREG_I   = 0;           // current idex into locreg
static       int  ERRSTATE   = 0x00;        // global errorstate

static       DANG CALREG[64];               // call register; a call dump

//   ---     ---     ---     ---     ---

void __writoa  (int x, char* buff, int rdx) { _itoa(x, buff, rdx);                          };
int  __wrstrcmp(char* s0, char* s1        ) { return strcmp(s0, s1);                        };

DANG* __geterrloc(const char* p,
                  const char* f,

                  int         l)            {

    DANG* cal = &(CALREG[CALREG_I]);
    char callbranch[66];
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
                                            "%s%i: <%s> on func %s line %i\n",
                                            callbranch, CALREG_I, p, f, l    );

    CALREG_I++;                             // move to next entry in register
    return cal;                                                                              };

void __printcalreg(int flush)               {

    for(int i = 0; i < CALREG_I; i++)       { fprintf(stderr, CALREG[i].location);
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
    printf                                  ("%s [ERRCODE %u]: ", mstart, errcode           );

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

        default: mbody =                    "UNRECOGNIZED %s";                          break;

                                            };

    printf                                  (mbody, info                                    );
    printf                                  ("\n\nERRLOC TRACKER: %i ENTRIES\n", CALREG_I   );

    __printcalreg(0);

    if(ERRSTATE == FATAL)                   { printf("TERMINATED"); exit(ERRSTATE);         }
    else                                    { /*?*/                                         };

};

//   ---     ---     ---     ---     ---