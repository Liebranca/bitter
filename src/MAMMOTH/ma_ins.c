/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    MA_INS                                *
*                                           *
*     -turns numbers into actions           *
*     -"low-level" instructions             *
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

#include "ma_ins.h"
#include "ma_boiler.h"

#include <stdio.h>
#include <string.h>

//   ---     ---     ---     ---     ---

static NIHIL lm_ins_arr[] = {               // table of low-level instructions
    &lmcpy,
    &lmmov,
    &lmwap,
    &lmwed,

    &lmjmp,
    &lmjif,
    &lmeif,
    &lmexit,

    &lmadd,
    &lmsub,
    &lminc,
    &lmdec,
    &lmmul,
    &lmdiv,
    &lmmod,

    &lmand,
    &lmor,
    &lmxor,
    &lmnor,
    &lmnand,
    &lmxnor,

    &lmtil,
    &lmcl,
    &lmclm,
    &lmnot,

    &lmshr,
    &lmshl,

    &lmlis

};

static CODE* ins          = NULL;           // current instruction

static uint  costk[16];                     // stack of code locations
static uint  costk_top    = 0;              // top of stack

//   ---     ---     ---     ---     ---

typedef struct INS_SIZES {

    uint    cast;
    uint    cbyte;

    uint    size;
    uint    elems;
    uint    units;
    uint    step;

    MEMUNIT mask_a;
    MEMUNIT mask_b;

} INSZ; static INSZ ins_sz={0};

void svinsz(uint cast_to) {

    ins_sz.cast   = rd_cast;
    ins_sz.cbyte  = rd_cbyte;

    ins_sz.size   = rd_size;
    ins_sz.elems  = rd_elems;
    ins_sz.units  = rd_units;
    ins_sz.step   = rd_step;

    ins_sz.mask_a = szmask_a;
    ins_sz.mask_b = szmask_b;

    rd_cast       = cast_to;                                                                };

void ldinsz(void) {

    rd_cast  = ins_sz.cast;
    rd_cbyte = ins_sz.cbyte;

    rd_size  = ins_sz.size;
    rd_elems = ins_sz.elems;
    rd_units = ins_sz.units;
    rd_step  = ins_sz.step;

    szmask_a = ins_sz.mask_a;
    szmask_b = ins_sz.mask_b;                                                               };

//   ---     ---     ---     ---     ---

void lmpush(uint loc)                       { costk[costk_top]=loc; costk_top++;            };
void lmpop (void    )                       { costk_top--; uint loc=costk[costk_top];       \
                                              ins = (CODE*) (mammi->jmpt[loc]);             \
                                              lm_ins_arr[ins->loc](); ins=NULL;             };

void ldins (uint loc)                       { ins = (CODE*) (mammi->jmpt[loc]);             \
                                              lm_ins_arr[ins->loc](); ins=NULL;             };

//   ---     ---     ---     ---     ---

void lmoff(uint* off)                       {

    off[1]           =                       off[0] / UNITSZ;
    off[0]          -=                       off[1] * UNITSZ;
    off[1]           = FETMASK              (rd_units, off[1]);                             }

//   ---     ---     ---     ---     ---

int lmfet(uintptr_t* dst        ,
          uint*      udr        ,
          uint*      off        ,
          uint       allow      )           {

                                            // cleanup
    RSTPTRS                                 (                                   );

                                            // apply typing to the read
    TPADDR                                  (rd_cast, -1                        );

                                            // fetch target
    lmasl                                   (udr                                );

//   ---     ---     ---     ---     ---

    if(allow&1) {                           // enforce alignment
        off[0]=0;
        while((*rd_result)%16) {
            (*rd_result)--;
            off[0]++;

        };

//   ---     ---     ---     ---     ---

        off[1]       ^= off[1];
        lmoff(off);

    }; *dst           = *rd_result;
    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

#define IF_CONST_ALLOWED(allow, v, src, cbyte, cunit, mask) {                               \
    if  (allow&1) { v=( ((MEMUNIT*) (src))[ cunit ]&(mask<<(cbyte*8))) >> (cbyte*8); }      \
    else          { v=(*rd_result)&mask;                                             }      }

//   ---     ---     ---     ---     ---

#define ONE_FET_OP(ac)                                                                       \
    uintptr_t  addr          = 0;           /* sole operand                    */            \
    uint       udr           = 0;           /* offset into ins->data           */            \
    uint       offsets[2];                  /* [0..2] upos, cbyte              */            \
                                                                                             \
/*   ---     ---     ---     ---     ---  */                                                 \
                                                                                             \
    if(ac&2) {                                                                               \
        svinsz(0x06);                                                                        \
                                                                                             \
    };                                                                                       \
                                                                                             \
    lmfet(&addr, &udr, offsets, ac);                                                         \
                                                                                             \
    MEMUNIT value;                                                                           \
    IF_CONST_ALLOWED(ac, value, addr, offsets[0], offsets[1], szmask_a)                      \
                                                                                             \
    if(ac&2) {                                                                               \
        ldinsz();                                                                            \
                                                                                             \
    }

#define MNY_FET_OP                                                                           \
    uintptr_t  addr          = 0;           /* sole operand                    */            \
    uint       udr           = 0;           /* offset into ins->data           */            \
    uint       offsets[2];                  /* [0..2] upos, cbyte              */            \
    MEMUNIT    value;                                                                        \
                                                                                             \

#define FET_NXT(ac)                                                                          \
    addr       ^= addr;                                                                      \
    value      ^= value;                                                                     \
    offsets[0] ^= offsets[0];                                                                \
    offsets[1] ^= offsets[1];                                                                \
                                                                                             \
    if(ac&2) {                                                                               \
        svinsz(0x06);                                                                        \
                                                                                             \
    };                                                                                       \
                                                                                             \
    lmfet(&addr, &udr, offsets, ac);                                                         \
                                                                                             \
    IF_CONST_ALLOWED(ac, value, addr, offsets[0], offsets[1], szmask_a)                      \
                                                                                             \
    if(ac&2) {                                                                               \
        ldinsz();                                                                            \
                                                                                             \
    }

//   ---     ---     ---     ---     ---

#define TWO_FET_OP(aca, acb)                                                                 \
                                                                                             \
    uintptr_t  addr_a        = 0;           /* dst operand                     */            \
    uintptr_t  addr_b        = 0;           /* src operand                     */            \
                                                                                             \
    uint       udr           = 0;           /* offset into ins->data           */            \
    uint       offsets[4];                  /* [0..2] upos_a, cbyte_a          */            \
                                            /* [2..4] upos_b, cbyte_b          */            \
                                                                                             \
/*   ---     ---     ---     ---     ---  */                                                 \
                                                                                             \
    if(aca&2) {                                                                              \
        svinsz(0x06);                                                                        \
                                                                                             \
    };                                                                                       \
                                                                                             \
    lmfet(&addr_a, &udr, offsets, aca);                                                      \
                                                                                             \
    MEMUNIT value_a;                                                                         \
    IF_CONST_ALLOWED(aca, value_a, addr_a, offsets[0], offsets[1], szmask_a);                \
                                                                                             \
    if(aca&2) {                                                                              \
        ldinsz();                                                                            \
                                                                                             \
    };                                                                                       \
                                                                                             \
    szmask_b                 = szmask_a;                                                     \
                                                                                             \
/*   ---     ---     ---     ---     ---                                       */            \
                                                                                             \
    if(acb&2) {                                                                              \
        svinsz(0x06);                                                                        \
                                                                                             \
    };                                                                                       \
                                                                                             \
    lmfet(&addr_b, &udr, offsets+2, acb);                                                    \
                                                                                             \
    {   ulong tmp            = szmask_a;                                                     \
        szmask_a             = szmask_b;                                                     \
        szmask_b             = tmp;                                                         }\
                                                                                             \
    MEMUNIT value_b;                                                                         \
    IF_CONST_ALLOWED(acb, value_b, addr_b, offsets[2], offsets[3], szmask_b);                \
                                                                                             \
    if(acb&2) {                                                                              \
        ldinsz();                                                                            \
                                                                                             \
    };                                                                                       \

//   ---     ---     ---     ---     ---

void lmcpy(void)                            {

    TWO_FET_OP(0b01, 0);

    if(typedata.flags&0x10) {               // corner case: strcpy

        CTOK*    t    = (CTOK*) ((uintptr_t) value_b);

        uint     len  = t->vsize;
        uchar*   s    = (uchar*) &(t->value);

        MEMUNIT  mask = 0x00LL;
        MEMUNIT  sstr = 0x00LL;

        MEMUNIT* dst  = ((MEMUNIT*) addr_a)+offsets[1];

        for(uint x=0; x<len; x++) {

            MEMUNIT c  = ((MEMUNIT) s[x]) << (offsets[0]*8);

            mask      |= (0xFFLL << (offsets[0]*8));
            sstr      |= c;

            offsets[0]++;

            if(offsets[0] && !(offsets[0]%UNITSZ)) {
                *dst &=~ mask; mask^=mask;
                *dst |=  sstr; sstr^=sstr;
                offsets[0]^=offsets[0]; dst++;

            };

        } if(mask) {
            *dst &=~ mask;
            *dst |=  sstr;

        }; return;
    };

                                            // clean masked section jic and set
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_b     << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      value_b       << (offsets[0]*8);                };

void lmmov(void)                            {

    TWO_FET_OP(0b01, 0b01);

                                            // ^same as cpy
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_b     << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      value_b       << (offsets[0]*8);

                                            // then clean src
    ((MEMUNIT*)addr_b)[offsets[3]] &=~      (szmask_a     << (offsets[2]*8));               };

void lmwap(void)                            {

    TWO_FET_OP(0b01, 0b01);

                                            // ^same as cpy, but b = a
    ((MEMUNIT*)addr_b)[offsets[3]] &=~      (szmask_a     << (offsets[2]*8));
    ((MEMUNIT*)addr_b)[offsets[3]] |=       value_a       << (offsets[2]*8);

                                            // then a = old_b
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_b     << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      value_b       << (offsets[0]*8);                };

void lmwed(void)                            {

    ONE_FET_OP(0b10);
    SYMBOL* sym = mammi->gvalues+value;

    if( (*((uint*) sym->id.type)==0x45505954)
    ||  (*((uint*) sym->id.type)==0x47414C46) ) {
        sym->onrd();

    };                                                                                      };

//   ---     ---     ---     ---     ---

void lmjmp(void)                            {

    ONE_FET_OP(0b10);

    uint  loc = ADDRTOLOC(value);
    mammi->next=loc;                                                                        };

void lmjif(void)                            {

    TWO_FET_OP(0b10,0);

    if(value_b) {
        uint  loc = ADDRTOLOC(value_a);
        mammi->next=loc;

    };                                                                                      };

void lmeif(void)                            {

    TWO_FET_OP(0b10,0);

    if(!value_b) {
        uint  loc = ADDRTOLOC(value_a);
        mammi->next=loc;

    };                                                                                      };

void lmexit(void)                           {

    ONE_FET_OP(0);

                                            /* cleanup any sentinels */
    mammi->lvalues[mammi->lvaltop] ^=       mammi->lvalues[mammi->lvaltop];

    mammi->lvalues[mammi->lvaltop]  = value;/* set return code       */
    mammi->next = mammi->jmpt_i;            /* force program to quit */                     };

//   ---     ---     ---     ---     ---

void lmadd(void)                            { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a+value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmsub(void)                            { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a-value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lminc(void)                            { ONE_FET_OP(0b01);

    MEMUNIT result                  =       (value+1)&szmask_a;
    ((MEMUNIT*) addr)[offsets[1]]  &=~      (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr)[offsets[1]]  |=       result    << (offsets[0]*8);                    };

void lmdec(void)                            { ONE_FET_OP(0b01);

    MEMUNIT result                  =       (value-1)&szmask_a;
    ((MEMUNIT*) addr)[offsets[1]]  &=~      (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr)[offsets[1]]  |=       result    << (offsets[0]*8);                    };

void lmmul (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a*value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmdiv (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a/value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmmod (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a%value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

//   ---     ---     ---     ---     ---

void lmand (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a&value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmor  (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a|value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmxor (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a^value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmnor (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (~(value_a|value_b))&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmnand(void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (~(value_a&value_b))&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmxnor(void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (~(value_a^value_b))&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

//   ---     ---     ---     ---     ---

void lmtil (void)                           { ONE_FET_OP(0b01);

    MEMUNIT result                 =        (~value)&szmask_a;
    ((MEMUNIT*) addr)[offsets[1]] &=~       (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr)[offsets[1]] |=        result    << (offsets[0]*8);                    };

void lmcl  (void)                           { ONE_FET_OP(0b01);

    ((MEMUNIT*) addr)[offsets[1]] &=~       (szmask_a << (offsets[0]*8));                   };

void lmclm (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a&(~value_b))&szmask_a;

    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmnot (void)                           { ONE_FET_OP(0b01);

    MEMUNIT result                 =        (!value)&szmask_a;
    ((MEMUNIT*) addr)[offsets[1]] &=~       (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr)[offsets[1]] |=        result    << (offsets[0]*8);                    };


//   ---     ---     ---     ---     ---

void lmshr (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a>>value_b)&szmask_a;

    if(typedata.flags&1) {                  // handle signed

        if( (value_a&szmask_a) > (szmask_a/2)) {

            result                   =      ((0xFFLL<<value_b)|result)&szmask_a;

        };
    };

    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmshl (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a<<value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

//   ---     ---     ---     ---     ---

void lmlis (void)                           { MNY_FET_OP;

    FET_NXT(0);
    CTOK* t = (CTOK*) *rd_result;

    FET_NXT(0); MEMUNIT* s = (MEMUNIT*) &(t->value);
    for(uint x=0; x<1+(t->vsize/UNITSZ); x++) {
        rd_result[x]=s[x];

    };

//   ---     ---     ---     ---     ---

    MACRO* m     = NULL;
    void*  nulmy = NULL;

    STR_HASHGET(MNAMES_HASH, (uchar*) rd_result, nulmy, 0);
    if(nulmy!=NULL) {
        m = (MACRO*) nulmy;

    }; m->value = value;
    HASHSET(MNAMES_HASH, byref(m->id));                                                     };

//   ---     ---     ---     ---     ---

void lmasl(uint* udr)                       {

    CTOK* t;                                // current token

    uchar buff[UNITSZ*2];                   // for loading operator symbols
    uchar c = 0x00;                         // for reading one operator at a time

    uchar force_solve = 0;                  // quick exit flag

    mammi->ctrl^=mammi->ctrl;

    EVAL_EXP: if( ((*udr)>=ins->size) \
              ||  (force_solve      ) )     { goto RESULT; }

    rd_value  = rd_lhand+rd_step;
    CLMEM2(rd_value, rd_step);

    CLMEM2(buff, UNITSZ*2);

    t         = (CTOK*) (ins->data+(*udr));
    rd_rawv   = buff+0;

//   ---     ---     ---     ---     ---

    uint w=0;
    for(uint y=0; y<ARRSIZE(t->lops); y++) { // paste leftside operators into leftside of str

        c = t->lops[y]; if(!c) {
            break;

        }; rd_rawv[y]  = c; w++;

    };                                      // ^idem, rightside

    for(int y=(ARRSIZE(t->rops)-1), z=0;
        y>-1; y--, z++                 ) {

        c = t->rops[z]; if(!c) {
            break;

        } rd_rawv[w] = c; w++;

                                            // now pop 'em to get evalstate
    }; rd_rawv[w]=0x00; POPOPS              (                             );

//   ---     ---     ---     ---     ---

    *rd_value = t->value;

    if(t->ttype==CALCUS_SEPAR) {
        force_solve=1; goto RESULT;

    } elif(t->ttype==CALCUS_CHSTR) {

        mammi->state     |= MAMMIT_SF_PSTR;
        uint slen         = 1+(t->vsize/UNITSZ);
        *rd_result        = (uintptr_t) t;

        while(slen%(sizeof(CTOK)/UNITSZ)) {
            slen++;

        }; *udr          += slen+sizeof(CTOK)/UNITSZ;

        goto EVAL_EXP;

    } elif(t->ttype==CALCUS_FETCH) {
        mammi->state |= MAMMIT_SF_PFET;

    };

//   ---     ---     ---     ---     ---    // compress expanded tokens into final value

    (*udr)+=sizeof(CTOK)/UNITSZ;

    if(mammi->state&MAMMIT_SF_PSTR) {
        mammi->state &=~MAMMIT_SF_PSTR;
        goto EVAL_EXP;

    };

    SOLVE: CALCUS_COLLAPSE();

    goto EVAL_EXP;

    RESULT: if(mammi->lvlb>0) {
        MAMMIT_LVLB_PRV;
        goto SOLVE;

    }; mammi->state &=~MAMMIT_SF_PFET;

    if(force_solve) { goto ALT_EXIT; }
    return;

    ALT_EXIT: (*udr)+=sizeof(CTOK)/UNITSZ;                                                  };

//   ---     ---     ---     ---     ---

void swcpy (void)                           { ins_code = 0x00; ins_argc = 2;                };
void swmov (void)                           { ins_code = 0x01; ins_argc = 2;                };
void swwap (void)                           { ins_code = 0x02; ins_argc = 2;                };
void swwed (void)                           { ins_code = 0x03; ins_argc = 1;                };

void swjmp (void)                           { ins_code = 0x04; ins_argc = 1;                };
void swjif (void)                           { ins_code = 0x05; ins_argc = 2;                };
void sweif (void)                           { ins_code = 0x06; ins_argc = 2;                };
void swexit(void)                           { ins_code = 0x07; ins_argc = 1;                };

void swadd (void)                           { ins_code = 0x08; ins_argc = 2;                };
void swsub (void)                           { ins_code = 0x09; ins_argc = 2;                };
void swinc (void)                           { ins_code = 0x0A; ins_argc = 1;                };
void swdec (void)                           { ins_code = 0x0B; ins_argc = 1;                };
void swmul (void)                           { ins_code = 0x0C; ins_argc = 2;                };
void swdiv (void)                           { ins_code = 0x0D; ins_argc = 2;                };
void swmod (void)                           { ins_code = 0x0E; ins_argc = 2;                };

void swand (void)                           { ins_code = 0x0F; ins_argc = 2;                };
void swor  (void)                           { ins_code = 0x10; ins_argc = 2;                };
void swxor (void)                           { ins_code = 0x11; ins_argc = 2;                };
void swnor (void)                           { ins_code = 0x12; ins_argc = 2;                };
void swnand(void)                           { ins_code = 0x13; ins_argc = 2;                };
void swxnor(void)                           { ins_code = 0x14; ins_argc = 2;                };

void swtil (void)                           { ins_code = 0x15; ins_argc = 1;                };
void swcl  (void)                           { ins_code = 0x16; ins_argc = 1;                };
void swclm (void)                           { ins_code = 0x17; ins_argc = 2;                };
void swnot (void)                           { ins_code = 0x18; ins_argc = 1;                };

void swshr (void)                           { ins_code = 0x19; ins_argc = 2;                };
void swshl (void)                           { ins_code = 0x1A; ins_argc = 2;                };

void swlis (void)                           { ins_code = 0x1B; ins_argc = 2;                };

//   ---     ---     ---     ---     ---

void stentry(void)                          {

    rd_tkx++; uchar* s=tokens[rd_tkx];
              int    x=0;

    for(; x<strlen(s); x++) {
        mammi->entry[x]=s[x];

    }; mammi->entry[x]=0x00;                                                                };

//   ---     ---     ---     ---     ---