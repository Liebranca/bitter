/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    LYMATH                                *
*                                           *
*     -operations done MY way               *
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

#include "lymath.h"

//   ---     ---     ---     ---     ---

void SCUSTR2(ustr2* us, ustr2 dim, float x) { float v=(float) (us->x/dim.x);                \
                                              us->x=(v*x)*dim.x;                            \
                                              v=(float) (us->y/dim.y);                      \
                                              us->y=(v*x)*dim.y;                            };

//   ---     ---     ---     ---     ---