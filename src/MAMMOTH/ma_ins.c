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
    uintptr_t vaddr  = 0x00;                // init address to constant

    uint      udr    = 0;                   // cur read offset into data
    uint      units  = 0;                   // unit-wise region bounds
    uint      upos   = 0;                   // unit-wise offset
    uint      cbyte  = 0;                   // byte-wise offset

    ulong     umask  = 0;                   // typemask
    MEMUNIT   value  = 0;                   // value to be copied over

//   ---     ---     ---     ---     ---    // read address

    for(uint x=0; x<sizeof(uintptr_t); x+=UNITSZ) {
        vaddr+=(ins->data[udr])<<(x*UNITSZ); udr++;

    }; addr          =                      (ADDR*) vaddr;

//   ---     ---     ---     ---     ---    // unpack sizing data

    cbyte            = ins->data[udr] & (SIZMASK(sizeof(uint)));
    units            = ins->data[udr] >> (8*(sizeof(uint)));

    upos             = cbyte / UNITSZ;
    cbyte           -= upos  * UNITSZ;

    upos             = FETMASK(units, upos);
    udr++;

    umask            = ins->data[udr]; udr++;
    //value            = ins->data[udr]; udr++;

    for(uint x=0; x<ins->size; x+=(sizeof(CTOK)/UNITSZ)) {
        CTOK* t      = (CTOK*) (ins->data+udr+x);

        CALOUT(E, ">CTOK @%u\n", x);
        CALOUT(E, "0x%08" PRIX32 " %08" PRIX32 "\n0x%016" PRIX64 " %016" PRIX64 "\n0x%016" PRIX64 "\n\n", t->ttype, t->vtype, t->lops, t->rops, t->value);

    };

//   ---     ---     ---     ---     ---    // copy value

    //addr->box[upos] &=~(umask << (cbyte*8));// clean masked section
    //addr->box[upos] |=  value << (cbyte*8); // flip them bits

    // CALOUT(E, "%u units needed for %s\n", udr, __func__);
                                                                                            };

//   ---     ---     ---     ---     ---