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

//   ---     ---     ---     ---     ---

static NIHIL lm_ins_arr[] = {               // table of low-level instructions
    &lmcpy,
    &lmmov,
    &lmwap

};

static CODE* ins          = NULL;           // current instruction

static uint  costk[16];                     // stack of code locations
static uint  costk_top    = 0;              // top of stack

//   ---     ---     ---     ---     ---

void lmpush(uint loc)                       { costk[costk_top]=loc; costk_top++;            };
void lmpop (void    )                       { costk_top--; uint loc=costk[costk_top];       \
                                              ins = (CODE*) (mammi->lvalues+loc);           \
                                              lm_ins_arr[ins->loc](); ins=NULL;             };

//   ---     ---     ---     ---     ---

void lmoff(uint* off)                       {

    off[0]          *= rd_size;

    off[1]           =                       off[0] / UNITSZ;
    off[0]          -=                       off[1] * UNITSZ;
    off[1]           = FETMASK              (rd_units, off[1]);                             }

//   ---     ---     ---     ---     ---

int lmfet(ADDR** dst        ,
          uint*  udr        ,
          uint*  off        ,
          uint   allow_const)               {

                                            // cleanup
    RSTPTRS                                 (                                   );
    CLMEM2                                  (rd_result, UNITSZ*ins->size        );

    rd_cbyte     ^= rd_cbyte;
    mammi->vaddr ^= mammi->vaddr;

                                            // fetch target
    lmasl                                   (udr                                );

    if(!allow_const) {
        if(!mammi->vaddr) { return ERROR; }

        *dst          =                         (ADDR*) (mammi->vaddr-sizeof(ID));  \
        TPADDR                                  (*dst                               );

        off[0]        = *rd_result;
        off[1]       ^= off[1];

        lmoff(off);

    }; return DONE;                                                                         };

//   ---     ---     ---     ---     ---

#define IF_CONST_ALLOWED(allow, v, src, cbyte, cunit, mask) {                                \
    if  (src  ) { v=(src->box[ cunit ]&(mask<<(cbyte*8))) >> (cbyte*8); }                    \
    elif(allow) { v=*rd_result;                                         }                   }

#define TWO_FET_OP(aca, acb)                                                                 \
                                                                                             \
    ADDR*     addr_a         = NULL;        /* dst operand                     */            \
    ADDR*     addr_b         = NULL;        /* src operand                     */            \
                                                                                             \
    uint      udr            = 0;           /* offset into ins->data           */            \
    uint      offsets[4];                   /* [0..2] upos_a, cbyte_a          */            \
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

void lmcpy(void)                            { TWO_FET_OP(0, 1);

                                            // clean masked section jic and set
    addr_a->box[offsets[1]] &=~             (szmask_b     << (offsets[0]*8));
    addr_a->box[offsets[1]] |=              value_b       << (offsets[0]*8);                };

void lmmov(void)                            { TWO_FET_OP(0, 0);

                                            // ^same as cpy
    addr_a->box[offsets[1]] &=~             (szmask_b     << (offsets[0]*8));
    addr_a->box[offsets[1]] |=              value_b       << (offsets[0]*8);

                                            // then clean src
    addr_b->box[offsets[3]] &=~             (szmask_a     << (offsets[2]*8));                };

void lmwap(void)                            { TWO_FET_OP(0, 0);

                                            // ^same as cpy, but b = a
    addr_b->box[offsets[3]] &=~             (szmask_a     << (offsets[2]*8));
    addr_b->box[offsets[3]] |=              value_a       << (offsets[2]*8);

                                            // then a = old_b
    addr_a->box[offsets[1]] &=~             (szmask_b     << (offsets[0]*8));
    addr_a->box[offsets[1]] |=              value_b       << (offsets[0]*8);                };

//   ---     ---     ---     ---     ---

void lmasl(uint* udr)                       {

    CTOK* t;                                // current token

    uchar buff[UNITSZ*2];                   // for loading operator symbols
    uchar c = 0x00;                         // for reading one operator at a time

    EVAL_EXP: if((*udr)>=ins->size) { goto RESULT; }

    CLMEM2(buff, UNITSZ*2);

    t       = (CTOK*) (ins->data+(*udr));
    rd_rawv = buff+0;

//   ---     ---     ---     ---     ---

    for(uint y=0; y<UNITSZ; y++) {          // paste leftside operators into leftside of str

        c = (uchar) ((t->lops>>(y*8))&0xFF);// shift && mask to next byte

        if(!c) {
            break;

        } rd_rawv[y]  = c;

    };                                      // ^idem, rightside

    for(uint y=(UNITSZ*2)-1, z=0;
        y>-1; y--, z++          ) {

        c = (uchar) ((t->rops>>(z*8))&0xFF);
        if(!c) {
            break;

        } rd_rawv[y] = c;

                                            // now pop 'em to get evalstate
    }; POPOPS                               (                             );

//   ---     ---     ---     ---     ---

    if(t->ttype==CALCUS_FETCH) {            // pointer. setup fetch switches
        mammi->vaddr  = (uintptr_t) t->value;
        mammi->vtype  = t->vtype;

    }

    elif(t->ttype==CALCUS_SEPAR) {
        goto ALT_EXIT;

    } else {                                // else it's a constant

        *rd_value     = t->value;

    };

//   ---     ---     ---     ---     ---    // compress expanded tokens into final value

    (*udr)+=sizeof(CTOK)/UNITSZ; CALOUT(E, "\n");

    SOLVE:
        CALOUT(E, "0x%016" PRIX64 " %016" PRIX64 " %016" PRIX64 " -> ",
                  *rd_lhand, rd_flags, *rd_value                      );

        CALCUS_COLLAPSE();
        CALOUT(E, "%016" PRIX64 "\n", *rd_lhand);

    goto EVAL_EXP;

    RESULT: if(mammi->lvlb>0) {
        MAMMIT_LVLB_PRV;
        goto SOLVE;

    }; return;

    ALT_EXIT: (*udr)+=sizeof(CTOK)/UNITSZ; CALOUT(E, "\n");                                 };

//   ---     ---     ---     ---     ---

void swboil(void)                           { /* placeholder */                             };

void swcpy(void)                            { ins_code = 0x00; ins_argc = 2; swboil();      };
void swmov(void)                            { ins_code = 0x01; ins_argc = 2; swboil();      };
void swwap(void)                            { ins_code = 0x02; ins_argc = 2; swboil();      };

//   ---     ---     ---     ---     ---