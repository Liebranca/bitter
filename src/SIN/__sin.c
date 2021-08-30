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

#include "KVRNEL/lymath.h"

#include "sin_globals.h"
#include "sin_texture.h"
#include "sin_shader.h"

#include "REND/sin_canvas.h"
#include "__sin.h"

#include <math.h>

//   ---     ---     ---     ---     ---

void NTSIN() { NTSINHASH(); NTSINTEX(); NTSINSHD (); NTCANVAS (); };
void DLSIN() { DLCANVAS (); DLSINSHD(); DLSINTEX (); DLSINHASH(); };

//   ---     ---     ---     ---     ---

static uint SIN_CHRBLK[2] = { 16, 16 };

void GTCHRGRD(uint chrsz)                   {

    uint base_sz[2] = { chrsz, chrsz         };
    uint z      [2] = { SIN_WSIZX, SIN_WSIZY };

    for(uint x=0; x<2; x++) {
        while((base_sz[x]%z[x]) > chrsz) {
            base_sz[x]++; if(base_sz[x]==128) { break; }

        }; SIN_CHRBLK[x]=base_sz[x];
    };                                                                                      };

//   ---     ---     ---     ---     ---

void GTCHRSZ (float* dst, uint mul)         {
    dst[0]=( ((float) SIN_CHRBLK[0])/((float) SIN_WSIZX) )*mul;
    dst[1]=( ((float) SIN_CHRBLK[1])/((float) SIN_WSIZY) )*mul;                             };

void GTSCRSZ (uint* dst, uint mul)          {

    uint z[2] = { SIN_WSIZX, SIN_WSIZY };

    for(uint x=0; x<2; x++) {
        uint i=0; uint chrsiz=SIN_CHRBLK[x]*mul;

        while(z[x]>chrsiz) {
            z[x]-=chrsiz; i++;

        }; if(z[x]>=(chrsiz/2)) { i++; };
        dst[x]=i*2;

    };                                                                                      };

//   ---     ---     ---     ---     ---