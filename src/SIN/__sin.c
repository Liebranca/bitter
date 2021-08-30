/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    SIN                                   *
*                                           *
*     -bytesized graphics                   *
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

#include "sin_globals.h"
#include "sin_texture.h"
#include "sin_shader.h"

#include "REND/sin_canvas.h"

//   ---     ---     ---     ---     ---

void NTSIN() { NTSINHASH(); NTSINTEX(); NTSINSHD (); NTCANVAS (); };
void DLSIN() { DLCANVAS (); DLSINSHD(); DLSINTEX (); DLSINHASH(); };

//   ---     ---     ---     ---     ---

void GTCHRSZ (float* dst, uint chsiz)       { dst[0]=((float) chsiz)/((float) SIN_WSIZX);   \
                                              dst[1]=((float) chsiz)/((float) SIN_WSIZY);   };

void GTSCRSZ (uint* dst, uint chsiz)        {

    uint z[2] = { SIN_WSIZX, SIN_WSIZY };

    for(uint x=0; x<2; x++) {
        uint i=0;

        while(z[x]>chsiz) {
            z[x]-=chsiz; i++;

        }; if(z[x]>=(chsiz/2)) { i++; };
        dst[x]=i*2;

    };                                                                                      };

//   ---     ---     ---     ---     ---