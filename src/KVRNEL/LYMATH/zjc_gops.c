/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    ZJC_GOPS                              *
*                                           *
*     -general-purpose math                 *
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

#include "zjc_gops.h"

//   ---     ---     ---     ---     ---

uint mxui  (uint a, uint b)                 { if(a>b) { return a; } return b;               };
uint mnui  (uint a, uint b)                 { if(a<b) { return a; } return b;               };
uint sbmxui(uint a, uint b)                 { return mnui(a,b)-mxui(a,b);                   };
uint sbmnui(uint a, uint b)                 { return mxui(a,b)-mnui(a,b);                   };

//   ---     ---     ---     ---     ---