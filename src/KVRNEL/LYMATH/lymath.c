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

void SCUSTR16(ustr16* us, ustr16 dim,
              float x               )       { float v;                    \

                                              v=(float) ( ( (float)us->x) \
                                                        / ( (float)dim.x) );

                                              us->x=(v*x)*dim.x;          \

                                              v=(float) ( ( (float)us->y) \
                                                        / ( (float)dim.y) );

                                              us->y=(v*x)*dim.y;                            };

//   ---     ---     ---     ---     ---

#define USTR8_ELEMS_OPEN(us) {              \
    uchar us##x = us & 0x0F;                \
    uchar us##y = us & 0xF0
                
#define USTR8_ELEMS_SVCLUS(us)              \
    *us=(*us##y<<4) & (*us##x); }

#define USTR8_ELEMS_CLUS(us) }

//   ---     ---     ---     ---     ---

void add(ustr8* a, ustr8 b)                 {

    USTR8_ELEMS_OPEN(a);
    USTR8_ELEMS_OPEN(b);

        *ax=(*ax)+(bx); *ax=(*ax)&0xF0;
        *ay=(*ay)+(by); *ay=(*ay)&0xF0;

    USTR8_ELEMS_CLUS(b)
    USTR8_ELEMS_SVCLUS(a);                                                                  };

//   ---     ---     ---     ---     ---