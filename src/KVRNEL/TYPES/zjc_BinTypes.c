#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../zjc_evil.h"
#include "zjc_BinTypes.h"

#include <math.h>

//  - --- - --- - --- - --- -
// move this to lymath later

float clampf(float v,
             float start,
             float end  )                   { if      (v < start) { v = start; }
                                              else if (v > end  ) { v = end;   };

                                              return v;                                     };

//  - --- - --- - --- - --- -

void ZJC_pack_rawvert(VP3D* vert,
                      RWV3D* data)              {

    uint   co    = ( (float_tofrac(data->co     [0], 4, FRAC5, 32, 128)      )
                   | (float_tofrac(data->co     [1], 4, FRAC5, 32, 128) <<  8)
                   | (float_tofrac(data->co     [2], 4, FRAC5, 32, 128) << 16) );

    uint nnn     = ( (float_tofrac(data->normal [0], 1, FRAC5, 32,  32)      )
                   | (float_tofrac(data->normal [1], 1, FRAC4, 16,  16) <<  6)
                   | (            (data->normal [2] < 0               ) << 11)
                   | (      ((int) data->bhand                        ) << 12) );

    uint ttt     = ( (float_tofrac(data->tangent[0], 1, FRAC5, 32,  32)      )
                   | (float_tofrac(data->tangent[1], 1, FRAC5, 32,  32) <<  6)
                   | (            (data->tangent[2] < 0               ) << 12) );

    uint uvs     = ( (float_tofrac(data->uv     [0], 1, FRAC7, 64,  64)      )
                   | (float_tofrac(data->uv     [0], 1, FRAC7, 64,  64) <<  7) );

    vert->frac1  = (  (co)                    | ((nnn & 0xFF) << 24)           );
    vert->frac2  = ( ((nnn & (31 << 8)) >> 8) | (ttt << 5) | (uvs << 18)       );                                   }

void ZJC_pack_rawbox (BP3D*  box,
                      RWB3D* data)              {

    box->frac1 = ( (float_tofrac  (data->co[ 0], 4, FRAC5, 32, 128)      )
                 | (float_tofrac  (data->co[ 1], 4, FRAC5, 32, 128) <<  8)
                 | (float_tofrac  (data->co[ 2], 4, FRAC5, 32, 128) << 16)
                 | (float_tofrac  (data->co[ 3], 4, FRAC5, 32, 128) << 24) );

    box->frac2 = ( (float_tofrac  (data->co[ 4], 4, FRAC5, 32, 128)      )
                 | (float_tofrac  (data->co[ 5], 4, FRAC5, 32, 128) <<  8)
                 | (float_tofrac  (data->co[ 6], 4, FRAC5, 32, 128) << 16)
                 | (float_tofrac  (data->co[ 7], 4, FRAC5, 32, 128) << 24) );

    box->frac3 = ( (float_tofrac  (data->co[ 8], 4, FRAC5, 32, 128)      )
                 | (float_tofrac  (data->co[ 9], 4, FRAC5, 32, 128) <<  8)
                 | (float_tofrac  (data->co[10], 4, FRAC5, 32, 128) << 16)
                 | (float_tofrac  (data->co[11], 4, FRAC5, 32, 128) << 24) );

    box->frac4 = ( (float_tofrac  (data->co[12], 4, FRAC5, 32, 128)      )
                 | (float_tofrac  (data->co[13], 4, FRAC5, 32, 128) <<  8)
                 | (float_tofrac  (data->co[14], 4, FRAC5, 32, 128) << 16)
                 | (float_tofrac  (data->co[15], 4, FRAC5, 32, 128) << 24) );

    box->frac5 = ( (float_tofrac  (data->co[16], 4, FRAC5, 32, 128)      )
                 | (float_tofrac  (data->co[17], 4, FRAC5, 32, 128) <<  8)
                 | (float_tofrac  (data->co[18], 4, FRAC5, 32, 128) << 16)
                 | (float_tofrac  (data->co[19], 4, FRAC5, 32, 128) << 24) );

    box->frac6 = ( (float_tofrac  (data->co[20], 4, FRAC5, 32, 128)      )
                 | (float_tofrac  (data->co[21], 4, FRAC5, 32, 128) <<  8)
                 | (float_tofrac  (data->co[22], 4, FRAC5, 32, 128) << 16)
                 | (float_tofrac  (data->co[23], 4, FRAC5, 32, 128) << 24) );

                                                                                                                    }

//  - --- - --- - --- - --- -

uchar bitsize (uchar x)                     {

    uchar bsize = 0;
    uchar trail = 0;

    for(uchar b = 0; b < 8; b++) {

        uchar bitset = nthbit(x, b);

        if(bitset) { bsize++; bsize += trail; trail = 0; }
        else       { trail++;                            };

    };

    return bsize;                                                                           };

uchar usbitsize (ushort x)                  {

    uchar bsize = 0;
    uchar trail = 0;

    for(uchar b = 0; b < 16; b++) {

        uchar bitset = uinthbit(x, b);

        if(bitset) { bsize++; bsize += trail; trail = 0; }
        else       { trail++;                            };

    };

    return bsize;                                                                           };

int   nthbit    (uchar b, int n)            { return (b & (1 << n)) != 0;                   };
int   uinthbit  (uint  b, int n)            { return (b & (1 << n)) != 0;                   };

int takebits(uchar b,
             uint  iStart,
             uint  iEnd  )                  {

    int x = 0b0; int bit;

    for (uint i = iStart; i < iEnd + 1; i++)
    {
        bit = nthbit(b, i);
        if(bit) { x |= bit << (i - iStart); };

    };

    return x;                                                                               };

//  - --- - --- - --- - --- -

float frac_tofloat(uint  frac,
                   uint  maxval,
                   float fac,
                   uint  shift)             { if(frac == maxval - 1) { frac = maxval; };
                                              return ((int)(frac - shift)) * fac;           };
uchar float_tofrac (float v,
                    float maxval,
                    float fraction,
                    uint  fac,
                    uint  shift)            { v = clampf(v, -maxval, maxval - fraction);
                                              return (uchar) roundf(v * fac) + shift;       };

//      - --- - --- - --- - --- -

// try packing four pixels at a time someday

JOJPIX rgba_to_joj  (float* pixel)          {

    JOJPIX joj   = { 0 };

    float  r     = pixel[0];
    float  g     = pixel[1];
    float  b     = pixel[2];
    float  a     = pixel[3];

    joj.luma     = float_tofrac(( 0.257f * r) + (0.504f * g) + (0.098f * b), 1, FRAC6, 64,  0);
    joj.chroma_u = float_tofrac((-0.148f * r) - (0.291f * g) + (0.439f * b), 1, FRAC5, 32, 32);
    joj.chroma_v = float_tofrac(( 0.439f * r) - (0.368f * g) - (0.071f * b), 1, FRAC5, 32, 32);
    joj.alpha    = float_tofrac(a,                                           1, FRAC6, 64,  0);

    return joj;                                                                             };

void  joj_to_rgba  (float* pixel,
                    JOJPIX* joj )           {

    // * 0.015625f  ) * 1.164000f; * 0.03125f;

    float luma        = frac_tofloat(joj->luma,     64, FRAC6,  0) * 1.164000f;
    float chroma_u    = frac_tofloat(joj->chroma_u, 64, FRAC5, 32);
    float chroma_v    = frac_tofloat(joj->chroma_v, 64, FRAC5, 32);
    float alpha       = frac_tofloat(joj->alpha,    64, FRAC6,  0);

    pixel[0]          = (luma) + (1.596f * chroma_v);
    pixel[1]          = (luma) - (0.392f * chroma_u) - (0.813f * chroma_v);
    pixel[2]          = (luma) + (2.017f * chroma_u);
    pixel[3]          = alpha;                                                              };

//      - --- - --- - --- - --- -