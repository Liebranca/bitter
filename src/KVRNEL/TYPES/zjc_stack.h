#ifndef __ZJC_STACK_H__
#define __ZJC_STACK_H__

#include "../MEM/kvr_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

//  - --- - --- - --- - --- -

typedef struct ZJC_STACK_UINT {             // common LIFO stack to hold uints

    MEM   m;                                // memory block header

    uint  top;                              // current top idex
    uint* ptr;                              // ptr to MEM buff

} STK;

//  - --- - --- - --- - --- -

void MKSTK(STK* s, uint size, char* id);    // make a new stack

sint PSHSTK(STK* s, uint x);                // push to stack
uint POPSTK(STK* s);                        // pop from stack

//  - --- - --- - --- - --- -

#define STACKPUSH(stack, value)             { int retx=0;                                    \
    CALL(PSHSTK(stack, value), retx, 70, stack->m.id); /* throws full stack  */             }

#define STACKPOP(stack, value)              {                                                \
    CALL(POPSTK(stack), value, 71, stack->m.id);       /* throws empty stack */             }

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_STACK_H__