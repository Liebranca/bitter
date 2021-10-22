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

void lmoff(uint* off)                       {

    off[0]          *= rd_size;

    off[1]           =                       off[0] / UNITSZ;
    off[0]          -=                       off[1] * UNITSZ;
    off[1]           = FETMASK              (rd_units, off[1]);                             }

//   ---     ---     ---     ---     ---

void lmcpy(void)                            {

    ADDR*     addr_a         = NULL;        // dst operand
    ADDR*     addr_b         = NULL;        // src operand

    uint      udr            = 0;           // offset into ins->data
    uint      offsets[4];                   // [0..2] upos_a, cbyte_a
                                            // [2..4] upos_b, cbyte_b

//   ---     ---     ---     ---     ---    // clean the stack

    RSTPTRS                                 (                                   );
    CLMEM2                                  (rd_result, UNITSZ*ins->size        );

    rd_cbyte                 = 0;

                                            // fetch target and sizing data
    lmasl                                   (ins->data, &udr, ins->size         );
    addr_a                   =              (ADDR*) (mammi->vaddr-sizeof(ID));  \
    TPADDR                                  (addr_a                             );

    offsets[0]               = *rd_result;
    offsets[1]               = 0;

    szmask_b                 = szmask_a;

    lmoff(offsets);

//   ---     ---     ---     ---     ---

    RSTPTRS                                 (                                   );
    CLMEM2                                  (rd_result, UNITSZ*ins->size        );

    rd_cbyte                 = 0;

    lmasl                                   (ins->data, &udr, ins->size         );
    addr_b                   =              (ADDR*) (mammi->vaddr-sizeof(ID));  \
    TPADDR                                  (addr_b                             );

    offsets[2]               = *rd_result;
    offsets[3]               = 0;

    lmoff(offsets+2);

//   ---     ---     ---     ---     ---

    MEMUNIT value            =              ( (addr_b->box[ offsets[3] ] \
                                            & (szmask_a<<(offsets[2]*8)) )      )

                                            >> (offsets[2]*8                    );

    CALOUT(E, "A [%u : %u] | B [%u : %u]\n", offsets[0],offsets[1],offsets[2],offsets[3]);

                                            // clean masked section jic and set
    addr_a->box[offsets[1]] &=~             (szmask_b     << (offsets[0]*8));
    addr_a->box[offsets[1]] |=              value         << (offsets[0]*8);                };

//   ---     ---     ---     ---     ---

void lmasl(MEMUNIT* data,
           uint*    udr ,
           uint     size)                   {

    for(;(*udr)<size; (*udr)+=(sizeof(CTOK)/UNITSZ)) {

                                            // expand tokens
        uchar buff[UNITSZ*2]; CLMEM2        (buff, UNITSZ*2               );

        CTOK* t = (CTOK*) (data+(*udr));
        rd_rawv = buff+0;

//   ---     ---     ---     ---     ---

        if(t->ttype==CALCUS_FETCH) {        // pointer. setup fetch switches
            mammi->vaddr  = (uintptr_t) t->value;
            mammi->vtype  = t->vtype;

        }

        elif(t->ttype==CALCUS_SEPAR) {
            break;

        } else {                            // else it's a constant

            *rd_value     = t->value;

        };

//   ---     ---     ---     ---     ---

        for(uint y=0; y<UNITSZ; y++) {      // paste leftside operators into leftside of str

            uchar c       =                 (uchar) ((t->lops>>(y*8))&0xFF);
            if(!c) {
                break;

            } rd_rawv[y]  = c;

        };                                  // ^idem, rightside

        for(uint y=(UNITSZ*2)-1, z=0;
            y>-1; y--, z++          ) {

            uchar c      =                  (uchar) ((t->rops>>(z*8))&0xFF);
            if(!c) {
                break;

            } rd_rawv[y] = c;

                                            // now pop 'em to get evalstate
        }; POPOPS                           (                             );

//   ---     ---     ---     ---     ---    // compress expanded tokens into final value

        SOLVE: CALCUS_COLLAPSE();
        if(mammi->lvlb>0) {
            MAMMIT_LVLB_PRV;
            goto SOLVE;

        };
    }; (*udr)+=sizeof(CTOK)/UNITSZ;                                                         };

//   ---     ---     ---     ---     ---

void swboil(void)                           {
    // placeholder
                                                                                            };

void swcpy(void)                            {
    ins_code = 0x00;
    ins_argc = 2;

    swboil();
                                                                                            };

//   ---     ---     ---     ---     ---