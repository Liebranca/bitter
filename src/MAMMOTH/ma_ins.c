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
    NULL,
    &lmjmp,
    &lmexit

};

static CODE* ins          = NULL;           // current instruction

static uint  costk[16];                     // stack of code locations
static uint  costk_top    = 0;              // top of stack

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
          uint       sol_addr   )           {

                                            // cleanup
    RSTPTRS                                 (                                   );
    CLMEM2                                  (rd_result, UNITSZ*ins->size        );

    rd_cbyte     ^= rd_cbyte;
    mammi->vaddr ^= mammi->vaddr;
                                            // fetch target
    lmasl                                   (udr                                );
//   ---     ---     ---     ---     ---

    if(sol_addr) {                          // enforce alignment
        off[0]=0;
        while((*rd_result)%16) {
            (*rd_result)--;
            off[0]++;

        }; TPADDR(rd_cast, -1);             // infer sizes

//   ---     ---     ---     ---     ---

        off[1]       ^= off[1];
        lmoff(off);

    }; *dst           = *rd_result;
    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

#define IF_CONST_ALLOWED(allow, v, src, cbyte, cunit, mask) {                                \
    if  (allow) { v=( ((MEMUNIT*) (src))[ cunit ]&(mask<<(cbyte*8))) >> (cbyte*8); }         \
    else        { v=(*rd_result)&mask;                                             }        }

//   ---     ---     ---     ---     ---

#define ONE_FET_OP(ac)                                                                       \
    uintptr_t  addr          = 0;           /* sole operand                    */            \
    uint       udr           = 0;           /* offset into ins->data           */            \
    uint       offsets[2];                  /* [0..2] upos, cbyte              */            \
                                                                                             \
/*   ---     ---     ---     ---     ---    /* err-catch the fetch             */            \
                                                                                             \
                                                                                             \
    lmfet(&addr, &udr, offsets, ac);                                                         \
                                                                                             \
    MEMUNIT value;                                                                           \
    IF_CONST_ALLOWED(ac, value, addr, offsets[0], offsets[1], szmask_a)

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
/*   ---     ---     ---     ---     ---    /* err-catch the fetch             */            \
                                                                                             \
                                                                                             \
    MAMMCTCH                                (lmfet(&addr_a, &udr, offsets, aca  ),           \
                                             gblevil, MAMMIT_EV_NFET, "dst"     );           \
                                                                                             \
                                                                                             \
    MEMUNIT value_a;                                                                         \
    IF_CONST_ALLOWED(aca, value_a, addr_a, offsets[0], offsets[1], szmask_a);                \
                                                                                             \
    szmask_b                 = szmask_a;                                                     \
                                                                                             \
/*   ---     ---     ---     ---     ---                                       */            \
                                                                                             \
    MAMMCTCH                                (lmfet(&addr_b, &udr, offsets+2, acb),           \
                                             gblevil, MAMMIT_EV_NFET, "src"     );           \
                                                                                             \
    {   ulong tmp            = szmask_a;                                                     \
        szmask_a             = szmask_b;                                                     \
        szmask_b             = tmp;                                                         }\
                                                                                             \
                                                                                             \
    MEMUNIT value_b;                                                                         \
    IF_CONST_ALLOWED(acb, value_b, addr_b, offsets[2], offsets[3], szmask_b);

//   ---     ---     ---     ---     ---

void lmcpy(void)                            { TWO_FET_OP(1, 0);

                                            // clean masked section jic and set
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_b     << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      value_b       << (offsets[0]*8);                };

void lmmov(void)                            { TWO_FET_OP(1, 1);

                                            // ^same as cpy
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_b     << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      value_b       << (offsets[0]*8);

                                            // then clean src
    ((MEMUNIT*)addr_b)[offsets[3]] &=~      (szmask_a     << (offsets[2]*8));               };

void lmwap(void)                            { TWO_FET_OP(1, 1);

                                            // ^same as cpy, but b = a
    ((MEMUNIT*)addr_b)[offsets[3]] &=~      (szmask_a     << (offsets[2]*8));
    ((MEMUNIT*)addr_b)[offsets[3]] |=       value_a       << (offsets[2]*8);

                                            // then a = old_b
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_b     << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      value_b       << (offsets[0]*8);                };

void lmjmp(void)                            { rd_cast=0x06; TPADDR(rd_cast, -1); ONE_FET_OP(0);

    uchar loc_buff[18];
    uint  loc = ADDRTOLOC(value);

    snprintf(loc_buff, 18, "0x%" PRIXPTR "", value);
    MAMMCTCH(loc, gblevil, MAMMIT_EV_JUMP, loc_buff);

    mammi->next=loc;                                                                        };

void lmexit(void)                           { rd_cast=0x06; TPADDR(rd_cast, -1); ONE_FET_OP(0);

                                            /* cleanup any sentinels */
    mammi->lvalues[mammi->lvaltop]^=mammi->lvalues[mammi->lvaltop];

    mammi->lvalues[mammi->lvaltop]=value;   /* set return code       */
    mammi->next = mammi->jmpt_i;            /* force program to quit */                     };

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
    for(uint y=0; y<UNITSZ; y++) {          // paste leftside operators into leftside of str

        c = (uchar) ((t->lops>>(y*8))&0xFF);// shift && mask to next byte

        if(!c) {
            break;

        }; rd_rawv[y]  = c; w++;

    };                                      // ^idem, rightside

    for(int y=(UNITSZ*2)-1, z=0;
        y>-1; y--, z++          ) {

        c = (uchar) ((t->rops>>(z*8))&0xFF);
        if(!c) {
            break;

        } rd_rawv[w] = c;

                                            // now pop 'em to get evalstate
    }; POPOPS                               (                             );

//   ---     ---     ---     ---     ---

    if(t->ttype==CALCUS_FETCH) {            // pointer. setup fetch switches
        mammi->vaddr  = (uintptr_t) t->value;
        mammi->vtype  = t->vtype;

    }

    elif(t->ttype==CALCUS_SEPAR) {
        force_solve=1; goto RESULT;

    } else {                                // else it's a constant
        *rd_value     = t->value;

    };

//   ---     ---     ---     ---     ---    // compress expanded tokens into final value

    (*udr)+=sizeof(CTOK)/UNITSZ;

    SOLVE:
        CALCUS_COLLAPSE();

    goto EVAL_EXP;

    RESULT: if(mammi->lvlb>0) {
        MAMMIT_LVLB_PRV;
        goto SOLVE;

    }; if(force_solve) { goto ALT_EXIT; }
    return;

    ALT_EXIT: (*udr)+=sizeof(CTOK)/UNITSZ;                                                  };

//   ---     ---     ---     ---     ---

void swboil(void)                           { /* placeholder */                             };

void swcpy(void)                            { ins_code = 0x00; ins_argc = 2; swboil();      };
void swmov(void)                            { ins_code = 0x01; ins_argc = 2; swboil();      };
void swwap(void)                            { ins_code = 0x02; ins_argc = 2; swboil();      };

void swjmp(void)                            { ins_code = 0x04; ins_argc = 1; swboil();      };

void swexit(void)                           { ins_code = 0x05; ins_argc = 1; swboil();      };

//   ---     ---     ---     ---     ---

void stentry(void)                          {

    rd_tkx++; uchar* s=tokens[rd_tkx];
              int    x=0;

    for(; x<strlen(s); x++) {
        mammi->entry[x]=s[x];

    }; mammi->entry[x]=0x00;                                                                };

//   ---     ---     ---     ---     ---