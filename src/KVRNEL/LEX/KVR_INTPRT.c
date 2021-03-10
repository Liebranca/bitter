/*   ---     ---     ---     ---     ---    *
 *  KVR_INTPRT.C                            *
 *                                          *
 *  -handles console user input             *
 *  -tokenization                           *
 *  -parsing                                *
 *                                          *
 *   ---     ---     ---     ---     ---    */

#include "KVR_INTPRT.h"
#include "../MEM/KVR_BIN.h"

#include <string.h>
#include <conio.h>
#include <stdio.h>

#define INTPRT_ERRC0(t) printf("INVALID_TOKEN: %s", t)

//   ---     ---     ---     ---     ---

static const char* KVR_SGL[] =              {

    "RNOW",
    "EXIT"

                                            };

static const char* KVR_VRB[] =              {

    "NEW",
    "GET",
    "DEL",

    "SAVE",
    "LOAD",
    "CLOSE",

                                            };

static const char* KVR_SBJ[] =              {

    "PX"

                                            };

//   ---     ---     ---     ---     ---

void GTIN(COM* com)                         {

    char buff[KCOM_INLEN]; buff[0] = '\0';

    char c   = '\0';
    int  ptr = 0;

    printf("$: ");                          // prompt

    while(c != '\r')                        { c = (char) getch();

        // force ALL CAPS
        if(c >= 'a' && c <= 'z') { c -= 32; }

        if(c == '\b') { if(ptr > 0)         {
            ptr--; buff[ptr] = ' ';         // no negative index, no deleting of prompt
            printf("\b \b");                /* subptr, go back, clear last char, go back */ };
                                                                                            }

        elif(ptr < KCOM_INLEN - 1)          {
            buff[ptr] = c;                  // no overflow, no terminator overwrite
            printf("%c", buff[ptr]); ptr++; /* add new char, print, addptr */               };

        fflush(stdout);                                                                     };
    buff[ptr] = '\0'; printf("\n");         // terminate @ptr so strlen gets it right

    TKIN(com, buff);

};

//   ---     ---     ---     ---     ---

int VALIDATE_TOKEN(char*        token,
                   const char** table)      { int len = ARRSIZE(KVR_VRB);

    for(int x = 0; x < len; x++)            { if( !strcmp(token, KVR_VRB[x]) )
                                                {  return x+1;                 }            };

    return 0;                                                                               };

int VALIDATE_COMMAND(COM* c)                {

    int verb;
    int subj;

    if(c->token_count < 2) {
        verb = VALIDATE_TOKEN(c->tokens[0], KVR_SGL);
        if(!verb) { INTPRT_ERRC0(c->tokens[0]); return 0; }

        c->ids[0] = verb - 1;
        c->order = -1;

        return 1;

    }

//   ---     ---     ---     ---     ---

    verb = VALIDATE_TOKEN(c->tokens[0], KVR_VRB);

    if(!verb) {
        int subj = VALIDATE_TOKEN(c->tokens[0], KVR_SBJ);
        if(!subj) { INTPRT_ERRC0(c->tokens[0]); return 0; };

        verb = VALIDATE_TOKEN(c->tokens[1], KVR_VRB);
        if(!verb) { INTPRT_ERRC0(c->tokens[1]); return 0; };

        c->order  = 1;
        c->ids[0] = subj - 1;
        c->ids[1] = verb - 1;

        return 1;                                                                           };

//   ---     ---     ---     ---     ---

    subj = VALIDATE_TOKEN(c->tokens[1], KVR_SBJ);
    if(!subj) { INTPRT_ERRC0(c->tokens[1]); return 0; };

    c->order  = 0;
    c->ids[0] = verb - 1;
    c->ids[1] = subj - 1;

    return 1;                                                                               };

//   ---     ---     ---     ---     ---

void TKIN(COM* com, char* buff)             {

    char last = '\0';

    int  i    = 0;
    int  k    = 0;

    if( strlen(buff) > (KCOM_INLEN))
    { printf("Command <%s> is overlength", buff); return; }

//   ---     ---     ---     ---     ---

    do {

        if(i > KCOM_MAXVAL) { break; }      // raise overargs
        if(k > KCOM_VALW-1) { break; }      // raise overlen

        while(*buff == ' ')                 { if(last != ' ') { i++; k = 0; }
                                              buff++; continue;                             };

        last = *buff; com->tokens[i][k] = last; k++;

    } while(*buff++); com->token_count = i;

    VALIDATE_COMMAND(com);                                                                  };

//   ---     ---     ---     ---     ---

/*

rules so i don't get lost

base form: <verb> <subj> <args>

means basic datablock operations which would
be valid for all and every datatype.

because this list is minimal, it's faster to
identify whether the command itself is valid

special form: <subj> <verb> <args>

this is for operations that are specific to
that particular family of datatypes; here we
want to identify valid datatype first, then
valid operation.

of course, one could declare a long table of
operations on global scope, check validity
of given action and then check that there is
a variation of that action for the subject
in question. but that would mean much slower
access speed for all commands, which is not
what we want.

these rules notwithstanding, we still follow
the naming convention <verb><subj> for all
functions. this is so the action takes the
precedence in an autocomplete case;

where typing "subj_" would yield a list of
all possible actions, typing "verb_" would
yield instead a list of all possible types,
which most likely would not be that many.

if a command has enough specifity to merit
not being present on the global scope, then
this is the preferred result.

people who do find this approach a true
example of degeneracy should not be writting
code in PE$O; that is all i have to say on
the matter.

*/

//   ---     ---     ---     ---     ---

int comverb(char* s) {

    int x = 0;

    switch(s[0])                            {

    case 'L': x =   1; break;               // LOAD
    default :          break;

                                            };

    return x;                                                                               };

int comsubj(char* s) {

    int x = 0;

    switch(s[0])                            {

    case 'P': x =   1; break;               // PX
    default :          break;

                                            };

    return x;                                                                               };

//   ---     ---     ---     ---     ---

RET RDIN(COM* c)                            {

    ;

    };

//   ---     ---     ---     ---     ---
