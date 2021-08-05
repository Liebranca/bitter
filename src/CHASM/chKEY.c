/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    chKEY                                 *
*                                           *
*     -sdl keyboard stuff                   *
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

#include "chKEY.h"

//   ---     ---     ---     ---     ---

void STWINKEY(uchar* k, uchar input)        {(*k)|= (1 << input);                           };
void CLWINKEY(uchar* k, uchar input)        {(*k)&=~(1 << input);                           };
int  GTWINKEY(uchar* k, uchar value)        { return ((*k)&value)==value;                   };

//   ---     ---     ---     ---     ---