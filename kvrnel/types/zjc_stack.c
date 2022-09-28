/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    ZJC_STACK                             *
*                                           *
*     -simple stack for uints               *
*     -used to remember avail indices       *
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

#include "zjc_stack.h"

//  - --- - --- - --- - --- -

void MKSTK(STK* s, uint bounds)             { s->bounds=bounds; s->top=-1;                  };

//  - --- - --- - --- - --- -

int PSHSTK(STK* s, uint x)                  {

    if  (s->top == (s->bounds)) { return ERROR; }

    s->top++;
    s->buff[s->top]=x;

    return DONE;                                                                            };

uint POPSTK(STK* s)                         {

    if  (s->top == -1       ) { return (uint) ERROR; }
    elif(s->top == s->bounds) { s->top=s->bounds-1;  }

    uint value=s->buff[s->top];
    s->top--;

    return value;                                                                           };

void INCSTK(STK* s)                         { s->buff[s->top]++;                            };

//  - --- - --- - --- - --- -