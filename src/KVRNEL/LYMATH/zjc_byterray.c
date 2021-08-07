/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    ZJC_BYTERRAY                          *
*                                           *
*     -basic bytesized unit                 *
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

#include "zjc_byterray.h"
#include <stdio.h>

//   ---     ---     ---     ---     ---

void add(ustr8* a, ustr8 b)                 {

    a->x=(a->x+b.x);
    a->y=(a->y+b.y);

};

//   ---     ---     ---     ---     ---

void main(void)                             {

    ustr8 a={1,1};
    ustr8 b={3,0};

    add(&a, b);
    printf("x%u y%u", a.x, a.y);
    fflush(stdout);

};

//   ---     ---     ---     ---     ---