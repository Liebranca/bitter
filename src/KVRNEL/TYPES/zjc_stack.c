#include "zjc_stack.h"

//  - --- - --- - --- - --- -

void MKSTK(STK* s, uint bounds)             {

    s->bounds = bounds;
    s->top    = -1;

    s->ptr    = (uint*) (((char*) s) + sizeof(STK));

    };

//  - --- - --- - --- - --- -

int PSHSTK(STK* s, uint x)                  {

    if  (s->top == (s->bounds)) { return ERROR; }

    s->top++;
    s->ptr[s->top]=x;

    return 0;                                                                               };

uint POPSTK(STK* s)                         {

    if  (s->top == -1       ) { return (uint) ERROR; }
    elif(s->top == s->bounds) { s->top=s->bounds-1;  }

    uint value=s->ptr[s->top];
    s->top--;

    return value;                                                                           };

//  - --- - --- - --- - --- -