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

    TPADDR(addr); rd_cast=0x0A;

//   ---     ---     ---     ---     ---    // unpack sizing data

    cbyte            = (ins->data[udr] & (SIZMASK(sizeof(uint))));
    units            = ins->data[udr] >> (8*(sizeof(uint)));

    upos             = cbyte / UNITSZ;
    cbyte           -= upos  * UNITSZ;

    upos             = FETMASK(units, upos);
    udr++;

    umask            = ins->data[udr]; udr++;

//   ---     ---     ---     ---     ---

    RSTPTRS(); CLMEM2(rd_result, UNITSZ*128);

    for(uint x=0; x<ins->size; x+=(sizeof(CTOK)/UNITSZ)) {

        uchar buff[UNITSZ*2]; CLMEM2(buff, UNITSZ*2);

        CTOK* t      = (CTOK*) (ins->data+udr+x);
        rd_rawv      = buff+0;

//   ---     ---     ---     ---     ---

        if(t->ttype==CALCUS_FETCH) {
            mammi->state |= MAMMIT_SF_PFET;
            mammi->vaddr  = (uintptr_t) t->value;
            mammi->vtype  = t->vtype;

        } else {
            *rd_value     = t->value;

        };

//   ---     ---     ---     ---     ---

        for(uint y=0; y<UNITSZ; y++) {      // paste leftside operators into leftside of str

            uchar c  = (uchar) ((t->lops>>(y*8))&0xFF);
            if(!c) { break; } rd_rawv[y]=c;

        };                                  // ^idem, rightside

        for(uint y=(UNITSZ*2)-1, z=0; y>-1; y--, z++) {
            uchar c  = (uchar) ((t->rops>>(z*8))&0xFF);
            if(!c) { break; } rd_rawv[y]=c;

        }; uint len = POPOPS();             // now pop 'em to get evalstate

//   ---     ---     ---     ---     ---

        SOLVE: CALCUS_COLLAPSE();
        if(mammi->lvlb>0) {
            MAMMIT_LVLB_PRV;
            goto SOLVE;

        };
    }; value = *rd_result;

//   ---     ---     ---     ---     ---    // copy value

    addr->box[upos] &=~(umask << (cbyte*8));// clean masked section
    addr->box[upos] |=  value << (cbyte*8); // flip them bits
                                                                                            };

//   ---     ---     ---     ---     ---