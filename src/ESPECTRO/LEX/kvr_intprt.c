/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    KVR_INTPRT                            *
*                                           *
*     -reads PE$O input                     *
*     -produces C output                    *
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

#include "kvr_intprt.h"
#include "KVRNEL/TYPES/zjc_hash.h"
#include "eval_for.h"

#include <string.h>
#include <conio.h>
#include <stdio.h>

#define INTPRT_ERRC0(t) printf("INVALID_TOKEN: %s", t)

//   ---     ---     ---     ---     ---

static char  KVR_INBUFF[KCOM_INLEN];

static int   KVR_INITER[3] = {0,0,0};
static COM*  KVR_INCOM     = NULL;
static HASH* KVR_LNAMES    = NULL;

static char  LBRACE='{'; static char RBRACE='}';

//   ---     ---     ---     ---     ---

enum RDMODES                                {

    RDN,     RDIF,      RDELIF,    RDELSE,
    RDFOR,   RDWHILE,   RDDO,      RDRET,
    RDSKIP,  RDBREAK,   RDJMPT,    RDCASE,
    RDHALT,  RDEXTENDS, RDINCLUDE, RDMODULE,
    RDHED,   RDREG,     RDGUARD,   RDEXPORT,
    RDGOTO,  RDHERE,    RDSTM,     RDDEF,
    RDTBL,   RDOPEN,    RDCLOSE,   RDSET,
    RDGET

};

/*

   IF A [^ THEN B] {};
   R:([size ^ start, stop ^ start, stop, step]) [^ of ARR]
   FOR<TYPE> NAME in [R ^ R of TYPE_ARR]
   DO {} WHILE CONDO;

   JMPT VALUE:
   CASE A  {}
   CASE B  {}
   DEFAULT {}

*/

//   ---     ---     ---     ---     ---

void PESONIT()                              { KVR_LNAMES=MKHASH(8, "LNAMES");               };
void PESOEND()                              { DLMEM(byref(KVR_LNAMES));                     };

//   ---     ---     ---     ---     ---

void GTIN()                                 {

    char* buff = KVR_INBUFF;
    buff[0]    = '\0';

    char c     = '\0';
    int  ptr   = 0;

    printf("$: ");                          // prompt

    while(c != '\r')                        { c = (char) getch();

        // force ALL CAPS
        if(c >= 'a' && c <= 'z') { c -= 32; }

        if(c == '\b') { if(ptr > 0)         {
            ptr--; buff[ptr] = ' ';         // no negative index, no deleting of prompt
            printf("\b \b");                /* subptr, go back, clear last char, go back */ };
                                                                                            }

        elif(ptr < KCOM_INLEN - 1)          {
            KVR_INBUFF[ptr] = c;            // no overflow, no terminator overwrite
            printf("%c", buff[ptr]); ptr++; /* add new char, print, addptr */               };

        fflush(stdout);                                                                     };

    buff[ptr] = '\0'; printf("\n");         // terminate @ptr so strlen gets it right
    TKIN();

};

//   ---     ---     ---     ---     ---

#define EVAL_IF                             {                                                \
                                                                                             \
    (*j)=0; (*i)--;                                                                          \
    KVR_INCOM->tokens[*i][0]='i';KVR_INCOM->tokens[*i][1]='f'; KVR_INCOM->tokens[*i][2]='\0';\
    (*i)++; KVR_INCOM->tokens[*i][0] = '('; (*j)++;                                          \
                                                                                             \
    do {                                                                                     \
        if(*buff!=' ' && *buff!=LBRACE)                                                      \
        {                                                                                    \
            if( (*buff=='T') && ( ((*k)+4) < slen ) )                                        \
            {                                                                                \
                if( (*(buff+1)=='H') && (*(buff+2)=='E') && (*(buff+3)=='N') )               \
                {                                                                            \
                    KVR_INCOM->tokens[*i][*j]=')'; (*j)++;                                   \
                    KVR_INCOM->tokens[*i][*j]='\0';(*i)++;                                   \
                    KVR_INCOM->tokens[*i][0]=LBRACE;                                         \
                    KVR_INCOM->tokens[*i][1]='\0';(*i)++;                                    \
                                                                                             \
                    KVR_INCOM->tokens[*i][0]='i';                                            \
                    KVR_INCOM->tokens[*i][1]='f';                                            \
                    KVR_INCOM->tokens[*i][2]='\0';(*i)++;                                    \
                    KVR_INCOM->tokens[*i][0]='('; (*j)=1;                                    \
                                                                                             \
                    (*k)+=4; buff+=4;                                                        \
                    if(*buff==' ') { (*k)++; buff++; }; lvl++;                               \
                };                                                                           \
            };                                                                               \
                                                                                             \
            last = *buff; KVR_INCOM->tokens[*i][*j] = last;                                  \
            (*j)++; (*k)++;                                                                  \
        };                                                                                   \
                                                                                             \
    }while((*buff++)!=LBRACE);                                                               \
                                                                                             \
    KVR_INCOM->tokens[*i][*j]=')'; (*j)++; KVR_INCOM->tokens[*i][*j]='\0'; (*j)=0; (*i)++;   \
    KVR_INCOM->tokens[*i][*j]=LBRACE; (*j)++;                                                \
    KVR_INCOM->tokens[*i][*j]='\0'; mode=RDN; (*i)++; (*j)=0;                               }



void SETINBUFF(char* src) { strcpy(KVR_INBUFF, src); }
void SETINCOM(COM* c) { KVR_INCOM=c; }

void TKIN()                                 {

    char* ltkn = NULL;
    char  last = '\0';
    char* buff = KVR_INBUFF;

    int   lvl  = 0;
    int*  i    = KVR_INITER+0;
    int*  j    = KVR_INITER+1;
    int*  k    = KVR_INITER+2; *i = *j = *k = 0;

    int   mode = 0;
    int   slen = strlen(KVR_INBUFF);

    if( slen > (KCOM_INLEN))
    { printf("Command <%s> is overlength", KVR_INBUFF); return; }

//   ---     ---     ---     ---     ---

    do {

        if(*i > KCOM_MAXVAL) { break; }      // raise overargs
        if(*j > KCOM_VALW-1) { break; }      // raise overlen

        if(*buff == '<')                    { KVR_INCOM->tokens[*i][*j]='\0'; (*k)++;
                                              ltkn=KVR_INCOM->tokens[*i]; (*i)++; (*j)=0;   };

        while(*buff == ' ')                 { if(last != ' ')

                                            { KVR_INCOM->tokens[*i][*j]='\0';
                                              ltkn=KVR_INCOM->tokens[*i]; (*i)++; (*j)=0;   }

                                              buff++; (*k)++;                               };

        if(ltkn        )                    {
            if  (!strcmp(ltkn, "IF" )) { mode=RDIF;  }
            elif(!strcmp(ltkn, "FOR")) { mode=RDFOR; }

        }; ltkn=NULL;

        if(mode!=RDN)                       {

            switch(mode)                    {
                case RDIF :                 { EVAL_IF;  break; }
                case RDFOR:                 { EVAL_FOR; break; }
                default   :                 {           break; }

                                            };

        } else { last = *buff; KVR_INCOM->tokens[*i][*j] = last; (*j)++; (*k)++; };

        if(last==RBRACE && lvl)             { KVR_INCOM->tokens[*i][0]=RBRACE;
                                              KVR_INCOM->tokens[*i][1]='\0'; (*i)++;        };

    } while(*buff++); KVR_INCOM->token_count = *i;                                          };

//   ---     ---     ---     ---     ---

