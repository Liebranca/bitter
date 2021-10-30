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
    &lmdec

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

} INSZ;

INSZ svinsz(uint cast_to) {

    INSZ i   = {0};

    i.cast   = rd_cast;
    i.cbyte  = rd_cbyte;

    i.size   = rd_size;
    i.elems  = rd_elems;
    i.units  = rd_units;
    i.step   = rd_step;

    i.mask_a = szmask_a;
    i.mask_b = szmask_b;

    rd_cast=cast_to;

    return i;                                                                               };

void ldinsz(INSZ* i) {

    rd_cast  = i->cast;
    rd_cbyte = i->cbyte;

    rd_size  = i->size;
    rd_elems = i->elems;
    rd_units = i->units;
    rd_step  = i->step;

    szmask_a = i->mask_a;
    szmask_b = i->mask_b;                                                                    };

//   ---     ---     ---     ---     ---

void lmpush(uint loc)                       { costk[costk_top]=loc; costk_top++;            };
void lmpop (void    )                       { costk_top--; uint loc=costk[costk_top];       \
                                              ins = (CODE*) (mammi->jmpt[loc]);             \
                                              lm_ins_arr[ins->loc](); ins=NULL;             };

void ldins (uint loc)                       { ins = (CODE*) (mammi->jmpt[loc]);             \
                                              lm_ins_arr[ins->loc](); ins=NULL;             };

//   ---     ---     ---     ---     ---

void lmoff(uint* off)                       {

    off[0]          *= rd_size;

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
    CLMEM2                                  (rd_result, UNITSZ*ins->size        );

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
    INSZ       is;                                                                           \
                                                                                             \
/*   ---     ---     ---     ---     ---  */                                                 \
                                                                                             \
    if(ac&2) {                                                                               \
        is=svinsz(0x06);                                                                     \
                                                                                             \
    };                                                                                       \
                                                                                             \
    lmfet(&addr, &udr, offsets, ac);                                                         \
                                                                                             \
    MEMUNIT value;                                                                           \
    IF_CONST_ALLOWED(ac, value, addr, offsets[0], offsets[1], szmask_a)                      \
                                                                                             \
    if(ac&2) {                                                                               \
        ldinsz(&is);                                                                         \
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
    INSZ       is;                                                                           \
                                                                                             \
/*   ---     ---     ---     ---     ---  */                                                 \
                                                                                             \
    if(aca&2) {                                                                              \
        is=svinsz(0x06);                                                                     \
                                                                                             \
    };                                                                                       \
                                                                                             \
    lmfet(&addr_a, &udr, offsets, aca);                                                      \
                                                                                             \
                                                                                             \
    MEMUNIT value_a;                                                                         \
    IF_CONST_ALLOWED(aca, value_a, addr_a, offsets[0], offsets[1], szmask_a);                \
                                                                                             \
    if(aca&2) {                                                                              \
        ldinsz(&is);                                                                         \
                                                                                             \
    };                                                                                       \
                                                                                             \
    szmask_b                 = szmask_a;                                                     \
                                                                                             \
/*   ---     ---     ---     ---     ---                                       */            \
                                                                                             \
    if(acb&2) {                                                                              \
        is=svinsz(0x06);                                                                     \
                                                                                             \
    };                                                                                       \
                                                                                             \
    lmfet(&addr_b, &udr, offsets+2, acb);                                                    \
                                                                                             \
    {   ulong tmp            = szmask_a;                                                     \
        szmask_a             = szmask_b;                                                     \
        szmask_b             = tmp;                                                         }\
                                                                                             \
                                                                                             \
    MEMUNIT value_b;                                                                         \
    IF_CONST_ALLOWED(acb, value_b, addr_b, offsets[2], offsets[3], szmask_b);                \
                                                                                             \
    if(acb&2) {                                                                              \
        ldinsz(&is);                                                                         \
                                                                                             \
    };                                                                                       \

//   ---     ---     ---     ---     ---

void lmcpy(void)                            {

    TWO_FET_OP(0b01, 0);

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
    SYMBOL* sym = mammi->slots+value;

    if(*((uint*) sym->id.type)==0x45505954) {
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

    if(!value_a) {
        uint  loc = ADDRTOLOC(value_b);
        mammi->next=loc;

    };                                                                                      };

void lmexit(void)                           {

    ONE_FET_OP(0b10);

                                            /* cleanup any sentinels */
    mammi->lvalues[mammi->lvaltop] ^=       mammi->lvalues[mammi->lvaltop];

    mammi->lvalues[mammi->lvaltop]  = value;/* set return code       */
    mammi->next = mammi->jmpt_i;            /* force program to quit */                     };

//   ---     ---     ---     ---     ---

void lmadd(void)                            { TWO_FET_OP(0b01, 0);
    ((MEMUNIT*) addr_a)[offsets[1]] +=      value_b << (offsets[0]*8);                      };

void lmsub(void)                            { TWO_FET_OP(0b01, 0);
    ((MEMUNIT*) addr_a)[offsets[1]] -=      value_b << (offsets[0]*8);                      };

void lminc(void)                            { ONE_FET_OP(0b01);
    ((MEMUNIT*) addr)[offsets[1]]   +=      1       << (offsets[0]*8);                     };

void lmdec(void)                            { ONE_FET_OP(0b01);
    ((MEMUNIT*) addr)[offsets[1]]   -=      1       << (offsets[0]*8);                      };

//   ---     ---     ---     ---     ---

void lmasl(uint* udr)                       {

    CTOK* t;                                // current token

    uchar buff[UNITSZ*2];                   // for loading operator symbols
    uchar c = 0x00;                         // for reading one operator at a time

    uchar force_solve = 0;                  // quick exit flag

    EVAL_EXP: if( ((*udr)>=ins->size) \
              ||  (force_solve      ) )     { goto RESULT; }

    CLMEM2(buff, UNITSZ*2);

    t       = (CTOK*) (ins->data+(*udr));
    rd_rawv = buff+0;

//   ---     ---     ---     ---     ---

    uint w=0;
    for(uint y=0; y<ARRSIZE(t->lops); y++) { // paste leftside operators into leftside of str

        c = t->lops[y]; if(!c) {
            break;

        }; rd_rawv[y]  = c; w++;

    };                                      // ^idem, rightside

    for(int y=(ARRSIZE(t->rops)-1), z=0;
        y>-1; y--, z++                  ) {

        c = t->rops[z]; if(!c) {
            break;

        } rd_rawv[w] = c; w++;

                                            // now pop 'em to get evalstate
    }; POPOPS                               (                             );

//   ---     ---     ---     ---     ---

    if(t->ttype==CALCUS_SEPAR) {
        force_solve=1; goto RESULT;

    } else {                                // else it's a constant
        *rd_value     = t->value;

    };

//   ---     ---     ---     ---     ---    // compress expanded tokens into final value

    (*udr)+=sizeof(CTOK)/UNITSZ;
    SOLVE: CALCUS_COLLAPSE();

    goto EVAL_EXP;

    RESULT: if(mammi->lvlb>0) {
        MAMMIT_LVLB_PRV;
        goto SOLVE;

    }; if(force_solve) { goto ALT_EXIT; }
    return;

    ALT_EXIT: (*udr)+=sizeof(CTOK)/UNITSZ;                                                  };

//   ---     ---     ---     ---     ---

void swboil(void)                           { /* placeholder */                             };

void swcpy (void)                           { ins_code = 0x00; ins_argc = 2; swboil();      };
void swmov (void)                           { ins_code = 0x01; ins_argc = 2; swboil();      };
void swwap (void)                           { ins_code = 0x02; ins_argc = 2; swboil();      };
void swwed (void)                           { ins_code = 0x03; ins_argc = 1; swboil();      };

void swjmp (void)                           { ins_code = 0x04; ins_argc = 1; swboil();      };
void swjif (void)                           { ins_code = 0x05; ins_argc = 2; swboil();      };
void sweif (void)                           { ins_code = 0x06; ins_argc = 2; swboil();      };
void swexit(void)                           { ins_code = 0x07; ins_argc = 1; swboil();      };

void swadd (void)                           { ins_code = 0x08; ins_argc = 2; swboil();      };
void swsub (void)                           { ins_code = 0x09; ins_argc = 2; swboil();      };
void swinc (void)                           { ins_code = 0x0A; ins_argc = 1; swboil();      };
void swdec (void)                           { ins_code = 0x0B; ins_argc = 1; swboil();      };

//   ---     ---     ---     ---     ---

void stentry(void)                          {

    rd_tkx++; uchar* s=tokens[rd_tkx];
              int    x=0;

    for(; x<strlen(s); x++) {
        mammi->entry[x]=s[x];

    }; mammi->entry[x]=0x00;                                                                };

//   ---     ---     ---     ---     ---