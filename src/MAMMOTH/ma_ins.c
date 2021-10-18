/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    MA_INS                                *
*                                           *
*     -turns numbers into actions           *
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

void lmcpy(void)                            {

    // uintptr_t to     = addr;

    // uint      upos   = FETMASK(rd_units, cunit);
    // uint      elen   = ?strlen(exp);

    // MEMUNIT   umask  = szmask_a;

    // MEMUNIT   bmask  = cbyte*8

    // MEMUNIT[] exp    = expression;

    uint      udr   = 0;                    // cur read offset into data
    uintptr_t vaddr = 0x00;                 // init address to constant

    for(uint x=0; x<sizeof(uintptr_t); x+=UNITSZ) {
        vaddr += (code->data[udr])<<(x*UNITSZ); udr++;

    };

    ADDR* addr    = (ADDR*) vaddr;
    uint  upos    = code->data[udr] & SIZMASK(sizeof(uint));
    uint  shft    = code->data[udr] >> (8*sizeof(uint));

    udr++;

    ulong   umask = code->data[udr]; udr++;
    MEMUNIT value = code->data[udr]; udr++;

    addr->box[upos] &=~(umask << shft);
    addr->box[upos] |= value  << shft;

    CALOUT(E, "%u units needed for %s\n", udr, __func__);


};

//   ---     ---     ---     ---     ---