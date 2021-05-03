#include "zjc_stack.h"

//  - --- - --- - --- - --- -

void MKSTK(STK* s, uint size, char* id)     {

    s->m.id    = id;

    s->m.size  = sizeof(uint);
    s->m.count = size;
    s->top     = 0;

    MEMGET(byref(s->m), s->ptr, uint);                                                      };

//  - --- - --- - --- - --- -

sint PSHSTK(STK* s, uint x)                 {

    if  (s->top == (s->m.count  )) { return ERROR; }
    elif(s->top == (s->m.count+1)) { s->top=0;     };

    s->ptr[s->top]=x;
    s->top++;

    return 0;                                                                               };

uint POPSTK(STK* s)                         {

    if  (s->top == s->m.count+1) { return (uint) ERROR; }
    elif(s->top == s->m.count  ) { s->top=s->m.count-1; }

    uint value=s->ptr[s->top];

    if  (s->top  > 0) { s->top--;            }
    elif(s->top == 0) { s->top=s->m.count+1; };

    return value;                                                                           };

//  - --- - --- - --- - --- -