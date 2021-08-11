/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    SIN_TEXTURE                           *
*                                           *
*     -image manager                        *
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

#include "sin_texture.h"
#include "sin_globals.h"
#include "../../KVRNEL/lymath.h"

//   ---     ---     ---     ---     ---

typedef struct SIN_TEXTURE {                // simple descriptor

    ID    id;
    ustr8 info;                             // x: dim
                                            // y: n_layers
                                            // z: cl
                                            // w: flags

    uint  loc;                              // gl location
    

} TEX;

typedef struct SIN_TEXMANG {                // array of texture descriptors

    MEM m;                                  // mem header

    TEX slots[SIN_TEXNIDS];                 // id array
    STK stack;                              // avail @stktop

} TEXMNG;

static TEXMNG* textures = NULL;
static HASH*   texhash  = NULL;

//   ---     ---     ---     ---     ---

void NTSINTEX()                             {

                                            // set name and alloc to textures
    ID id=IDNEW                             ("DATA", "_SIN_TEXIDS"                 );       \
    MEMGET                                  (TEXMNG, textures                      ,        \
                                            xBYTES(SIN_TEXNIDS, uint), &id         );       \

                                            // init the slotstack
    MKSTK                                   (byref(textures->stack), SIN_TEXNIDS   );
    for(uint y=SIN_TEXNIDS; y>0; y--)       { STACKPUSH(byref(textures->stack), y-1);       };
                                                                                            };

void DLSINTEX()                             { DLMEM(textures);                              };

//   ---     ---     ---     ---     ---

uint MKTEX(uint dim)                        {

    uint idex; STACKPOP                     (byref(textures->stack), idex            );

    TEX  tex   = textures->slots[idex];

    tex.info.x = dim;
    tex.info.y = SIN_TEXLAYS;

    glGenTextures                           (1, &tex.loc                             );
    glActiveTexture                         (GL_TEXTURE0                             );
    glBindTexture                           (GL_TEXTURE_2D_ARRAY, tex.loc            );
    glTexStorage3D                          (GL_TEXTURE_2D_ARRAY, tex.info.y, GL_RGBA,
                                             tex.info.x, tex.info.x, tex.info.y      );

    return tex.loc;                                                                         };

//   ---     ---     ---     ---     ---
/*
    for(uchar layer = 0;    \
        layer < tex->imcount;
        layer++)            {

        joj_subTexRead                      (dim, tex->imcount, layer, &pixels              );
        glTexSubImage3D                     (GL_TEXTURE_2D_ARRAY, 0, 0, 0,                  \
                                             layer, tex->width, tex->height, 1              ,
                                             GL_RGBA32UI, GL_UNSIGNED_INT, pixels           );
    };

//   ---     ---     ---     ---     ---

    glTexParameteri                         (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0  );
    glTexParameteri                         (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL,  3  );
    glTexParameteri                         (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S,        \
                                             GL_REPEAT                                      );
    glTexParameteri                         (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T,        \
                                             GL_REPEAT                                      );

    glTexParameterf                         (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,    \
                                             GL_NEAREST_MIPMAP_NEAREST                      );
    glTexParameterf                         (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER,    \
                                             GL_NEAREST                                     );

    glGenerateMipmap                        (GL_TEXTURE_2D_ARRAY                            );
    glBindTexture                           (GL_TEXTURE_2D_ARRAY, 0                         );

//   ---     ---     ---     ---     ---
*/