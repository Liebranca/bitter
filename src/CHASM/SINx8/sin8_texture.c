/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    SINX8_TEXTURE                         *
*                                           *
*     -manages portion of data memlng       *
*     -fills that space with images         *
*     -helps you draw those images          *
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

#include "sinx8_texture.h"

//   ---     ---     ---     ---     ---



//   ---     ---     ---     ---     ---

    glGenTextures(1, &tex->location);       // move to gpu
    glActiveTexture                         (GL_TEXTURE0                                    );
    glBindTexture                           (GL_TEXTURE_2D_ARRAY, tex->location             );
    glTexStorage3D                          (GL_TEXTURE_2D_ARRAY, 4, GL_RGB8,               \
                                             tex->width, tex->height, tex->imcount          );

    for(uchar layer = 0;    \
        layer < tex->imcount;
        layer++)            {

        joj_subTexRead                      (dim, tex->imcount, layer, &pixels              );
        glTexSubImage3D                     (GL_TEXTURE_2D_ARRAY, 0, 0, 0,                  \
                                             layer, tex->width, tex->height, 1              ,
                                             GL_RGB, GL_FLOAT, pixels                       );
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