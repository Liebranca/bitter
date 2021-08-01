// *   ---     ---     ---     ---     ---  *
// *  ZJC_BINTYPES                          *
// *                                        *
// *  -common binary ops                    *
// *  -float<->frac & float<->joj           *
// *  -vertex packing                       *
// *                                        *
// *   ---     ---     ---     ---     ---  *

#ifndef __ZJC_BINTYPES_H__
#define __ZJC_BINTYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

#include "../zjc_CommonTypes.h"
#include "../zjc_evil.h"

//   ---     ---     ---     ---     ---    // these types are all basically the same!
                                            // the one and only reason to have them?
                                            // NAMING: less confusing to maintain.

typedef struct JOJ_GREY_COMPRESSED {

    uchar a;
    uchar b;
    uchar c;
    uchar d;

} JOJGREY; CASSERT                          ( sizeof(JOJGREY) == sizeof(float), \
                                              "JOJGREY size != float size"      );

//   ---     ---     ---     ---     ---

typedef struct JOJ_NVEC_COMPRESSED {

    uchar sign;

    uchar x;
    uchar y;
    uchar w;

} JOJVEC; CASSERT                           ( sizeof(JOJVEC) == sizeof(float),  \
                                              "JOJVEC size != float size"       );

//   ---     ---     ---     ---     ---

typedef struct JOJ_COLOR_COMPRESSED {

    uchar chroma_u;
    uchar chroma_v;
    uchar luma;
    uchar alpha;

} JOJPIX; CASSERT                           ( sizeof(JOJPIX) == sizeof(float),  \
                                              "JOJPIX size != float size"       );

//   ---     ---     ---     ---     ---    // *unpacked* geometry types
                                            // ie, how we read it from file on unzip
                                            // we'd pack them for actual in-memory usage

typedef struct CRK_VERT_COMPRESSED {

    uchar co_x;                             // coordinates
    uchar co_y;
    uchar co_z;

    uchar nr_x;                             // normals
    uchar nr_y;
    uchar nr_z;

    uchar tn_x;                             // tangent
    uchar tn_y;
    uchar tn_z;

    uchar bhand;                            // bitangent handeness

    uchar u;                                // tex proj
    uchar v;

} CRKVRT;

//   ---     ---     ---     ---     ---

#define ZJC_RAWCOL_ELEMS 4                  // RGBA will always be four floats
                                            // but who knows? one day, maybe...

#define ZJC_RAWVRT_ELEMS 12                 // elemcount for vertdata likely to change

//   ---     ---     ---     ---     ---

void  STCFRACL (uint level           );     // sets frac level

void  ENCNVEC  (float* n, JOJPIX* j  );     // encodes unit vector into joj-like
void  DECNVEC  (float* n, JOJPIX* j  );     // decodes joj-like unit vector

void  ENCGREY  (float* p, JOJPIX* j  );     // encodes packed curv/orm into joj-like
void  DECGREY  (float* p, JOJPIX* j  );     // decodes joj-like into packed curv/orm

void  ENCRGBA  (float* p, JOJPIX* j  );     // encode rgba into joj
void  DECRGBA  (float* p, JOJPIX* j  );     // decode joj into rgba

//   ---     ---     ---     ---     ---

void ENCVRT    (float* v, CRKVRT* c  );     // enconde verts into crk format

//   ---     ---     ---     ---     ---

uchar bitsize  (uchar  x             );
uchar usbitsize(ushort x             );

int   nthbit   (uchar b, int n       );
int   uinthbit (uint  b, int n       );

int   takebits (uchar b, uint iStart,\
                uint iEnd            );

//   ---     ---     ---     ---     ---

/*
     _________________________________________________________________________________
    |  · VP3D_64BITS · VERTEX_PACKED_3D ·                                             |
    |_________________________________________________________________________________|
    |  FRST_UINT32                          |  SCND_UINT32                            |
    |_______________________________________|_________________________________________|
    |_byte0___|_byte1___|_byte2___|_byte3___|_byte0___|_byte1___|_byte2___|_byte3_____|
    | ·       | ·       | ·       | ·       |      ·  |         |   ·     |  ·        |
    | 10000000| 10000000| 10000000| 10000000| 00000100| 00000000| 00100000| 01000000  |
    |_________|_________|_________|_________|______.__|_________|___._____|__.________|
    |_vpx_____|_vpy_____|_vpz_____|_nnx_____+xxn___|_ttt____________|_uvx____|_uvy____|
    |         |         |         |                |                |        |        |
    | 8       | 8       | 8       | 13             | 13             | 7      | 7      |
    |_________|_________|_________|________________|________________|________|________|

    the packing is pretty tight so I'll explain it

    vertex position (first 24 bits) follows the FRAC8 model: (idex(0, 256) - 128) * (1/32)

    vertex normals is where it gets interesting. we do nnx + xxn,
    which is (last 8 bits of FRST_UINT32) | (first 5 bits of SCND_UINT32 << 8)

    this 13bit model is fairly unique. the x element's model is FRAC6: (idex(0, 64) - 32) * (1/32)
    while the y element's model is FRAC5: (idex(0, 32) - 16) * (1/16)

    the last two bits store the sign of the z element and the bitangent handedness

    how do we compute the z element of a 13bit pack unit vector?

        z = 0; d = 1 - ( pow(x, 2) + pow(y, 2) )
        if(d > 0) { z = sqrt(d) * sign; }

    and the bitangent

        b = cross(n, t) * handedness

    the ttt element (SCND_UINT32, bits 5-18) for the tangent can then make use of an extra bit
    so both x and y elements follow the FRAC6 model, last bit is the sign of z
    z is then computed at shader runtime, same as the normal vector's

    texture coordinates (uvx, uvy) occupy the last 14 bits
    they follow the UFRAC7 model: idex(0, 128) * (1/128)
    this squishing of the uvs is a necessity but I'm still salty about it

    also a lot of distortion is caused by this roguish packing scheme,
    so fine details are bound to be blurred or lost and normal mapping gets wobbly
    I get those oldschool aesthetics for real, you post-process worshipping schmucks

*/

typedef struct VertexPacked3D {

    uint frac1;                                 // vpx, vpy, vpz, nnx
    uint frac2;                                 // xxn, ttt, uvx, uvy

} VP3D;

// and all that explaining just for two ints in a struct!
// guess I could make this a single long but GLSL wouldn't like that

void ZJC_pack_rawvert(VP3D* vert, CRKVRT* data);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif //__ZJC_BINTYPES_H__
