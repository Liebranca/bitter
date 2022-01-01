#ifndef __MA_CNTX_H__
#define __MA_CNTX_H__

#include "ma_boiler.h"

#ifdef __cplusplus
extern "C" {
#endif 

//   ---     ---     ---     ---     ---

void  REGMA (void);                         // toggle memory-set state
void  PROCMA(void);                         // toggle procedure state

NIHIL STOCB (void);                         // get callback for current state

//   ---     ---     ---     ---     ---

#define MAMMIT_LVLA_NXT { mammi->lvla_stack[mammi->lvla]=mammi->cntx; mammi->lvla++;    }

#define MAMMIT_LVLA_PRV {                                                               \
    /*NIHIL cb=STOCB(); if(cb) { cb(); }*/ mammi->lvla--;                               \
    mammi->cntx=mammi->lvla_stack[mammi->lvla];                                         }

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif 

#endif // __MA_CNTX_H__