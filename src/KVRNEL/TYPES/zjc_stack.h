#ifndef __ZJC_STACK_H__
#define __ZJC_STACK_H__

#include "../MEM/kvr_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct ZJC_STACK_UINT {             // common LIFO stack to hold uints

    uint bounds;                            // max elements
    uint top;                               // current top idex
    uint buff[];                            // value array

} STK;

//  - --- - --- - --- - --- -

void MKSTK(STK* s, uint bounds);            // make a new stack

int  PSHSTK(STK* s, uint x);                // push to stack
uint POPSTK(STK* s);                        // pop from stack

//  - --- - --- - --- - --- -

#define STACKPUSH(stack, value)             { int retx=PSHSTK(stack, value);                 \
    if(retx) { ERRCATCH(ERROR, retx,  70, ""); }           /* throws full stack  */         }

#define STACKPOP(stack, value)              { value=POPSTK(stack);                           \
    if(value==(uint) ERROR) { ERRCATCH(ERROR, value, 71, ""); } /* throws empty stack */    }

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_STACK_H__