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
#include "KVRNEL/lymath.h"

#include <stdio.h>
#include <libpng16/png.h>

//   ---     ---     ---     ---     ---

typedef struct SIN_TEXTURE {                // simple descriptor

    ID    id;
    ustr8 info;                             // x: dim
                                            // y: n_layers
                                            // z: cl
                                            // w: arr idex

    uint  loc;                              // gl location

} TEX;

typedef struct SIN_TEXMANG {                // array of texture descriptors

    MEM m;                                  // mem header

    TEX slots[SIN_TEXNIDS];                 // id array
    STK stack;                              // avail @stktop

} TEXMNG;

static TEXMNG* SIN_TEXTURES = NULL;
static HASH*   SIN_TEXHASH  = NULL;

//   ---     ---     ---     ---     ---

void NTSINTEX()                             {

                                            // set name and alloc to SIN_TEXTURES
    ID id=IDNEW                             ("DATA", "_SIN_TEXIDS"                 );       \
    MEMGET                                  (TEXMNG, SIN_TEXTURES                  ,        \
                                            xBYTES(SIN_TEXNIDS, uint), &id         );       \

                                            // init the slotstack
    MKSTK                                   (byref(SIN_TEXTURES->stack), SIN_TEXNIDS   );
    for(uint y=SIN_TEXNIDS; y>0; y--)       { STACKPUSH(byref(SIN_TEXTURES->stack), y-1);   };
                                                                                            };

void DLSINTEX()                             {
    for(uint idex=0     ;
        idex<SIN_TEXNIDS;
        idex++          )                   {   if(SIN_TEXTURES->slots[idex].loc)
                                                { DLTEX(idex); } }; DLMEM(SIN_TEXTURES);    };

//   ---     ---     ---     ---     ---

uint MKTEX(uint dim, char* name)            {

                                            // get next avail slot
    uint idex; STACKPOP                     (byref(SIN_TEXTURES->stack), idex        );

    TEX tex    = SIN_TEXTURES->slots[idex]; // fill out the forms
    tex.info.x = dim;
    tex.info.y = SIN_TEXLAYS;

                                            // alloc gl buffer
    glGenTextures                           (1, &(tex.loc)                           );
    glActiveTexture                         (GL_TEXTURE0                             );
    glBindTexture                           (GL_TEXTURE_2D_ARRAY, tex.loc            );
    glTexStorage3D                          (GL_TEXTURE_2D_ARRAY, tex.info.y, GL_RGBA,
                                             tex.info.x, tex.info.x, tex.info.y      );
    glBindTexture                           (GL_TEXTURE_2D_ARRAY, 0                  );

                                            // assign name to tex and insert to hash
    tex.id     = IDNEW                      ("TEX*", name                            );
    STSINHASH                               (&(tex.id)                               );

    return idex;                                                                            };

//   ---     ---     ---     ---     ---

void DLTEX(uint idex)                       {

    TEX tex = SIN_TEXTURES->slots[idex];

    void** tmp;                             // dummy hash get-pop
    GTSINHASH                               (&(tex.id), tmp, 1                  );

                                            // gl clean
    glBindTexture                           (GL_TEXTURE_2D_ARRAY, 0             );
    glDeleteTextures                        (1, &(tex.loc)                      );

                                            // push free to slotstack
    STACKPUSH                               (byref(SIN_TEXTURES->stack), idex   );
    CLMEM2                                  (&tex, sizeof(TEX)                  );

    CALOUT('E', "You never see this ALSO!");

          };

//   ---     ---     ---     ---     ---

void RDPNG(char* fpath)                     {

    png_structp png_ptr;
    png_infop   info_ptr;

    FILE*       fp;

    png_uint_32 width;
    png_uint_32 height;

    int         bit_depth;
    int         color_type;
    int         interlace_method;
    int         compression_method;
    int         filter_method;
    int         rowbytes;

    png_bytepp  rows;

//   ---     ---     ---     ---     ---

    fp = fopen (fpath, "rb");
    if (!fp) {
        CALOUT                              ('E', "Cannot open <%s>\n", fpath       );

    }; png_ptr = png_create_read_struct     (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr) {
        CALOUT                              ('E', "Cannot create PNG read structure");

    }; info_ptr = png_create_info_struct    (png_ptr                                );

    if (!png_ptr) {
        CALOUT                              ('E', "Cannot create PNG info structure");

    };

//   ---     ---     ---     ---     ---

    png_init_io                             (png_ptr, fp);
    png_read_png                            (png_ptr, info_ptr, 0, 0);

    png_get_IHDR                            (png_ptr, info_ptr, &width, &height     ,
                                             &bit_depth, &color_type                ,
                                             &interlace_method, &compression_method ,
                                             &filter_method                         );

    rows     = png_get_rows                 (png_ptr, info_ptr                      );
    rowbytes = png_get_rowbytes             (png_ptr, info_ptr                      );

//   ---     ---     ---     ---     ---

    /*for (j = 0; j < height; j++) {
        png_bytep row = rows[j];

        for (i = 0; i < rowbytes; i++) {
            png_byte r = row[i];

        };
    
    };*/

    // read 8x8 square
    for(int y=0; y<height; y++) {
        for(int x=0; x<(8*20)*3; x+=3) {
            uchar r=rows[y][x];
            if(r) { printf(" "); }
            else  { printf("x"); };

        };

        printf("\n");

    }; fclose(fp);


};

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