#ifndef __ZJC_STACK_H__
#define __ZJC_STACK_H__

#include "../MEM/kvr_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct ZJC_STACK_UINT {             // common LIFO stack to hold uints

    uint  bounds;                           // max elements
    uint  top;                              // current top idex
    uint* ptr;                              // ptr to MEM buff

} STK;

//  - --- - --- - --- - --- -

void MKSTK(STK* s, uint bounds);            // make a new stack

sint PSHSTK(STK* s, uint x);                // push to stack
uint POPSTK(STK* s);                        // pop from stack

//  - --- - --- - --- - --- -

#define STACKPUSH(stack, value)             { int retx=0;                                    \
    ERRCATCH(PSHSTK(stack, value), retx,  70, ""); /* throws full stack  */                 }

#define STACKPOP(stack, value)              {                                                \
    ERRCATCH(POPSTK(stack       ), value, 71, ""); /* throws empty stack */                 }

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_STACK_H__