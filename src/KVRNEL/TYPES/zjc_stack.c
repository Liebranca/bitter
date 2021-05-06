#include "zjc_stack.h"

//  - --- - --- - --- - --- -

void MKSTK(STK* s, uint bounds)             {

    s->bounds = bounds;
    s->top    = 0;

    s->ptr    = (uint*) (((char*) s) + sizeof(STK));

    };

//  - --- - --- - --- - --- -

sint PSHSTK(STK* s, uint x)                 {

    if  (s->top == (s->bounds  )) { return ERROR; }
    elif(s->top == (s->bounds+1)) { s->top=0;     };

    s->ptr[s->top]=x;
    s->top++;

    return 0;                                                                               };

uint POPSTK(STK* s)                         {

    if  (s->top == s->bounds+1) { return (uint) ERROR; }
    elif(s->top == s->bounds  ) { s->top=s->bounds-1;  }

    uint value=s->ptr[s->top];

    if  (s->top  > 0) { s->top--;           }
    elif(s->top == 0) { s->top=s->bounds+1; };

    return value;                                                                           };

//  - --- - --- - --- - --- -