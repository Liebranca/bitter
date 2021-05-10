/*   ---     ---     ---     ---     ---    *
 *  KVR_INTPRT.C                            *
 *                                          *
 *  -handles console user input             *
 *  -tokenization                           *
 *  -parsing                                *
 *                                          *
 *   ---     ---     ---     ---     ---    */

#include "kvr_intprt.h"
#include "../TYPES/zjc_hash.h"

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
void PESOEND()                              { DLMEM(byref(KVR_LNAMES->m));                  };

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

#define EVAL_FOR                            {                                                \
                                                                                             \
    (*j)=0; (*i)--;                                                                          \
    KVR_INCOM->tokens[*i][0]='f'; KVR_INCOM->tokens[*i][1]='o'; KVR_INCOM->tokens[*i][2]='r';\
    KVR_INCOM->tokens[*i][3]='\0'; (*i)++;                                                   \
                                                                                             \
    int rstage=0; int h=0; int f_flags=0; int z=0;                                           \
    char ftype [KCOM_VALW]; char fname [KCOM_VALW]; char farr  [KCOM_VALW];                  \
    char fstart[KCOM_VALW]; char fend  [KCOM_VALW]; char fstep [KCOM_VALW];                  \
                                                                                             \
    do {                                                                                     \
                                                                                             \
        if  (*buff==' '   )  { continue;      }                                              \
        elif(*buff==LBRACE)  { buff++; break; }                                              \
                                                                                             \
        if(rstage==0) {                                                                      \
            if  (*buff=='<')   { continue; }                                                 \
            elif(*buff=='>')   { ftype[h]='\0';  rstage++; h=0; continue; }                  \
            ftype[h]=*buff; h++;                                                             \
                                                                                             \
        }                                                                                    \
                                                                                             \
        elif(rstage==1) {                                                                    \
            if(*buff==',')     { fname[h]='\0';  rstage++; h=0; continue; }                  \
            fname[h]=*buff; h++;                                                             \
                                                                                             \
        }                                                                                    \
        elif(rstage==2) {                                                                    \
            if(!h && (*buff!='(')) { f_flags|=1; } /* is array loop-through */               \
            if(f_flags&1) {                                                                  \
                if(*buff=='(') { farr[h]='\0';   rstage++; h=0; continue; }                  \
                farr[h]=*buff; h++;                                                          \
                                                                                             \
            } else { rstage++; }                                                             \
        }                                                                                    \
        elif(rstage==3) {                                                                    \
            if  (*buff==',')   { fstart[h]='\0'; rstage++;  h=0; continue; }                 \
            elif(*buff==')')   { fstart[h]='\0'; rstage+=3; h=0; continue; }                 \
            fstart[h]=*buff; h++; z++;                                                       \
                                                                                             \
        }                                                                                    \
        elif(rstage==4) {                                                                    \
            if  (*buff==',')   { fend[h]='\0';   rstage++;  h=0; continue; }                 \
            elif(*buff==')')   { fend[h]='\0';   rstage+=2; h=0; continue; }                 \
            fend[h]=*buff; h++; f_flags|=2; /* has stop value; */                            \
                                                                                             \
        }                                                                                    \
        elif(rstage==5) {                                                                    \
            if(*buff==')')     { fstep[h]='\0';  rstage++;  h=0; continue; }                 \
            fstep[h]=*buff; h++; f_flags|=4; /* has step value; */                           \
                                                                                             \
        } else { break; }                                                                    \
    } while((*buff++)!=LBRACE);                                                              \
                                                                                             \
    int backwards=qstoi(fstart)>qstoi(fend);                                                 \
                                                                                             \
    if(f_flags&2 && f_flags&4)              /* with stop & step */                           \
    {   if(!(qstoi(fstart)>qstoi(fend)))    /* forward loop     */                           \
        {   if(f_flags&1)                   /* with array       */                           \
            {                                                                                \
                snprintf(KVR_INCOM->tokens[*i], KCOM_VALW,                                   \
                        "(%s %s=%s+%s; %s<%s+%s; %s+=%s)",                                   \
                        ftype, fname, farr, fstart, fname, farr, fend, fname, fstep);        \
            }                                                                                \
                                            /* with stop & step */                           \
                                            /* forward loop     */                           \
            else                            /* no array         */                           \
            {                                                                                \
                snprintf(KVR_INCOM->tokens[*i], KCOM_VALW,                                   \
                        "(%s %s=%s; %s<%s; %s+=%s)",                                         \
                        ftype, fname, fstart, fname, fend, fname, fstep);                    \
            }                                                                                \
        }                                                                                    \
        else                                                                                 \
        {                                   /* with stop & step */                           \
                                            /* backwards loop   */                           \
            if(f_flags&1)                   /* with array       */                           \
            {                                                                                \
                snprintf(KVR_INCOM->tokens[*i], KCOM_VALW,                                   \
                        "(%s %s=%s+%s; %s>%s+%s; %s-=%s)",                                   \
                        ftype, fname, farr, fstart, fname, farr, fend, fname, fstep);        \
            }                                                                                \
                                            /* with stop & step */                           \
                                            /* backwards loop   */                           \
            else                            /* no array         */                           \
            {                                                                                \
                snprintf(KVR_INCOM->tokens[*i], KCOM_VALW,                                   \
                        "(%s %s=%s; %s>%s; %s-=%s)",                                         \
                        ftype, fname, fstart, fname, fend, fname, fstep);                    \
            }                                                                                \
        }                                                                                    \
                                                                                             \
    }                                                                                        \
                                                                                             \
    elif(f_flags&2)                         /* with stop, no step */                         \
    {   if(!(qstoi(fstart)>qstoi(fend)))    /* forward loop       */                         \
        {   if(f_flags&1)                   /* with array         */                         \
            {                                                                                \
                snprintf(KVR_INCOM->tokens[*i], KCOM_VALW,                                   \
                        "(%s %s=%s+%s; %s<%s+%s; %s++)",                                     \
                        ftype, fname, farr, fstart, fname, farr, fend, fname);               \
            }                                                                                \
                                            /* with stop, no step */                         \
                                            /* forward loop       */                         \
            else                            /* no array           */                         \
            {                                                                                \
                snprintf(KVR_INCOM->tokens[*i], KCOM_VALW,                                   \
                        "(%s %s=%s; %s<%s; %s++)",                                           \
                        ftype, fname, fstart, fname, fend, fname);                           \
            }                                                                                \
        }                                                                                    \
        else                                                                                 \
        {                                   /* with stop, no step */                         \
                                            /* backwards loop     */                         \
            if(f_flags&1)                   /* with array         */                         \
            {                                                                                \
                snprintf(KVR_INCOM->tokens[*i], KCOM_VALW,                                   \
                        "(%s %s=%s+%s; %s>%s+%s; %s--)",                                     \
                        ftype, fname, farr, fstart, fname, farr, fend, fname);               \
            }                                                                                \
                                            /* with stop, no step */                         \
                                            /* backwards loop     */                         \
            else                            /* no array           */                         \
            {                                                                                \
                snprintf(KVR_INCOM->tokens[*i], KCOM_VALW,                                   \
                        "(%s %s=%s; %s>%s; %s--)",                                           \
                        ftype, fname, fstart, fname, fend, fname);                           \
            }                                                                                \
        }                                                                                    \
                                                                                             \
    }                                                                                        \
                                                                                             \
    else                                    /* no stop, so no step */                        \
    {   if(!(qstoi(fstart)<0))              /* forward loop        */                        \
        {   if(f_flags&1)                   /* with array          */                        \
            {                                                                                \
                snprintf(KVR_INCOM->tokens[*i], KCOM_VALW,                                   \
                        "(%s %s=%s+0; %s<%s+%s; %s++)",                                      \
                        ftype, fname, farr, fname, farr, fstart, fname);                     \
            }                                                                                \
                                            /* no stop, so no step */                        \
                                            /* forward loop        */                        \
            else                            /* no array            */                        \
            {                                                                                \
                snprintf(KVR_INCOM->tokens[*i], KCOM_VALW,                                   \
                        "(%s %s=0; %s<%s; %s++)",                                            \
                        ftype, fname, fname, fstart, fname);                                 \
            }                                                                                \
        }                                                                                    \
        else                                                                                 \
        {   for(int chx=0; chx<z; chx++)    { if(chx+1<z) { fstart[chx]=fstart[chx+1]; }    }\
            fstart[z-1]='\0';                                                                \
                                            /* no stop, so no step */                        \
                                            /* backwards loop      */                        \
            if(f_flags&1)                   /* with array          */                        \
            {                                                                                \
                snprintf(KVR_INCOM->tokens[*i], KCOM_VALW,                                   \
                        "(%s %s=%s+%s; %s>%s-1; %s--)",                                      \
                        ftype, fname, farr, fstart, fname, farr, fname);                     \
            }                                                                                \
                                            /* no stop, so no step */                        \
                                            /* backwards loop      */                        \
            else                            /* no array            */                        \
            {                                                                                \
                snprintf(KVR_INCOM->tokens[*i], KCOM_VALW,                                   \
                        "(%s %s=%s; %s>-1; %s--)",                                           \
                        ftype, fname, fstart, fname, fname);                                 \
            }                                                                                \
        }                                                                                    \
    }                                                                                        \
    mode=RDN;                                                                                \
    (*i)++; KVR_INCOM->tokens[*i][0]=LBRACE; KVR_INCOM->tokens[*i][1]='\0'; (*i)++;         }\

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

