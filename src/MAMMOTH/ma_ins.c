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
#include "ma_boxes.h"
#include "ma_boiler.h"

//   ---     ---     ---     ---     ---

static NIHIL lm_ins_arr[] = {               // table of low-level instructions
    &lmcpy

};

static CODE* ins          = NULL;           // current instruction

static uint  costk[16];                     // stack of code locations
static uint  costk_top    = 0;              // top of stack

//   ---     ---     ---     ---     ---

void lmpush(uint loc)                       { costk[costk_top]=loc; costk_top++;            };
void lmpop (void    )                       { costk_top--; uint loc=costk[costk_top];       \
                                              ins = (CODE*) (mammi->lvalues+loc);           \
                                              lm_ins_arr[ins->loc]();                       };

//   ---     ---     ---     ---     ---

void lmcpy(void)                            {

    ADDR*     addr   = NULL;                // pointer to mem sub-region

    uint      udr    = 0;                   // offset into data
    uint      upos   = 0;                   // unit-wise offset
    uint      cbyte  = 0;                   // byte-wise offset

//   ---     ---     ---     ---     ---    // read address

    addr             =                      (ADDR*) ins->data[udr]; udr++;               \
    TPADDR                                  (addr                                        );

//   ---     ---     ---     ---     ---    // unpack sizing data

    cbyte            = FEL32                (ins->data[udr]                              );
    upos             =                      cbyte / UNITSZ;                              \
    cbyte           -=                      upos  * UNITSZ;                              \

    upos             = FETMASK              (rd_units, upos                              );

//   ---     ---     ---     ---     ---    // clean the stack

    RSTPTRS                                 (                                            );
    CLMEM2                                  (rd_result, UNITSZ*FEH32(ins->data[udr])     );

    udr++;                                  // inc to start of token list

//   ---     ---     ---     ---     ---

    for(uint x=0; x<ins->size;  \
        x+=(sizeof(CTOK)/UNITSZ)) {         // expand tokens

        uchar buff[UNITSZ*2]; CLMEM2        (buff, UNITSZ*2                              );

        CTOK* t      =                      (CTOK*) (ins->data+udr+x);                   \
        rd_rawv      =                      buff+0;                                      \

//   ---     ---     ---     ---     ---

        if(t->ttype==CALCUS_FETCH) {        // pointer. setup fetch switches

            mammi->state |=                 MAMMIT_SF_PFET;                              \
            mammi->vaddr  =                 (uintptr_t) t->value;                        \
            mammi->vtype  =                 t->vtype;                                    \

        } else {                            // else it's a constant

            *rd_value     =                 t->value;                                    \

        };

//   ---     ---     ---     ---     ---

        for(uint y=0; y<UNITSZ; y++) {      // paste leftside operators into leftside of str

            uchar c       =                 (uchar) ((t->lops>>(y*8))&0xFF);             \
            if(!c) {
                break;

            } rd_rawv[y]  = c;

        };                                  // ^idem, rightside

        for(uint y=(UNITSZ*2)-1, z=0;
            y>-1; y--, z++          ) {

            uchar c  =                      (uchar) ((t->rops>>(z*8))&0xFF);             \
            if(!c) {
                break;

            } rd_rawv[y]  = c;

                                            // now pop 'em to get evalstate
        }; uint len       = POPOPS          (                                            );

//   ---     ---     ---     ---     ---    // compress expanded tokens into final value

        SOLVE: CALCUS_COLLAPSE();
        if(mammi->lvlb>0) {
            MAMMIT_LVLB_PRV;
            goto SOLVE;

        };
    };

//   ---     ---     ---     ---     ---    // copy value

    // clean masked section jic and set
    addr->box[upos] &=~(szmask_a     << (cbyte*8));
    addr->box[upos] |= (*rd_result ) << (cbyte*8);
                                                                                            };

//   ---     ---     ---     ---     ---