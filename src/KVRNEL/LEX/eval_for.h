// *   ---     ---     ---     ---     ---  *
// *  EVAL_FOR                              *
// *                                        *
// *  -interpreter for peso floops          *
// *                                        *
// *   ---     ---     ---     ---     ---  *

#ifndef __KVR_EVALFOR_H__
#define __KVR_EVALFOR_H__

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

#endif // __KVR_EVALFOR_H__