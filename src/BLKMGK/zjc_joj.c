#include "zjc_joj.h"
#include "kvr_zwrap.h"

#include "../KVRNEL/kvr_paths.h"
#include "../KVRNEL/MEM/kvr_str.h"

#include <string.h>

//   ---     ---     ---     ---     ---

BIN* JOJ_FROM;
BIN* JOJ_TO;
BIN* JOJ_PIXDUMP;

MEM* JOJ_PIXELS;

//   ---     ---     ---     ---     ---

BIN* MKJOJ(char* path,
           uint  mode)                      {

    int  evilstate;

    uint page =                             ZJC_DAFPAGE/sizeof(JOJPIX);         \
    BIN* joj; BINOPEN                       (joj, path, mode, 1, page, evilstate);

    return joj;                                                                             };

//   ---     ---     ---     ---     ---

JOJPIX* GTJOJ(BIN* joj)                     {

    char* path = PTHBIN                     (joj                          );
    uint  loc  =                            (strlen(path)+1) * sizeof(char);

    return MEMBUFF                          (byref(joj->m), JOJPIX, loc   );                };

//   ---     ---     ---     ---     ---

int NTJOJENG(int mode)                      {

    int   evilstate;
                                            // make a convenience string
    MEM*  m       = MKSTR                   (GTPECWD(), 64                        );
    char* path    = MEMBUFF(m, char, 0);

//   ---     ---     ---     ---     ---    // set fname and create/open files

    RPSTR                                   (&m, "\\MATERAW.joj", strlen(path)    );
    JOJ_FROM = MKJOJ                        (path, KVR_FMODE_RBP                  );

    RPSTR                                   (&m, "MATE.joj", strlen(path)-11      );
    BINOPEN                                 (JOJ_TO, path, KVR_FMODE_RBP,         \
                                             KVR_FTYPE_JOJ, 0, evilstate          );

    RPSTR                                   (&m, "PIXDUMP.hx", strlen(path)-8     );
    BINOPEN                                 (JOJ_PIXDUMP, path, KVR_FMODE_RB,     \
                                             KVR_FTYPE_DMP, 0, evilstate          );

    if(mode == JOJ_DECODE) {                // castling on invert op
        BIN* tmp = JOJ_FROM;
        JOJ_FROM = JOJ_TO;
        JOJ_TO   = tmp;
    };

//   ---     ---     ---     ---     ---    // make recycle pixel buffer

    JOJ_PIXELS    = MKSTR                   ("BUF", ZJC_DAFPAGE                   );

                                            // free convenience string
    DLMEM                                   (m                                    );        };

//   ---     ---     ---     ---     ---

int DLJOJENG(void)                          {

                                            // close mate
    BINCLOSE                                (JOJ_TO     );
    DLMEM                                   (JOJ_TO     );

                                            // remove temps
    BINREMOVE                               (JOJ_FROM   );
    DLMEM                                   (JOJ_FROM   );
    BINREMOVE                               (JOJ_PIXDUMP);
    DLMEM                                   (JOJ_PIXDUMP);

                                            // free recycle buff
    DLMEM                                   (JOJ_PIXELS );                                  };

//   ---     ---     ---     ---     ---

int ENCJOJ(uint dim, uint* size_i)          {

    int     wb;
    uint    sq_dim  = dim*dim;

                                            // get the joj
    JOJPIX* buff    = GTJOJ                 (JOJ_FROM                   );

                                            // sizing ints we need later
    uint    page    =                       ZJC_DAFPAGE/sizeof(float);
    uint    remain  =                       sq_dim*4;                   \

                                            // add to archive's total
    *size_i        +=                       sq_dim*sizeof(JOJPIX);      \

                                            // get recycle pixel buffer
    float* pixels = MEMBUFF                 (JOJ_PIXELS, float, 0       );

    rewind(JOJ_PIXDUMP->file);              // rewind cache

//   ---     ---     ---     ---     ---

    while(remain) {                         // read one pixel-wide page at a time
                                            // each pixel being __always__ 32-bit RGBA

        uint readsize =                     (remain<page) ? remain : page;           \

                                            // load buffer from dump
        BINREAD                             (JOJ_PIXDUMP, wb, float, readsize, pixels);

        remain -= readsize;                 // discount read from pending

        for(uint x=0, y=0;                  // go through page and jojencode
            x<(readsize);                   // then write to file
            x+=4, y++    )                  { JOJPIX cpix=rgba_to_joj(pixels+x);     \
                                              buff[y]=cpix;                          };

                                            // save encoded pixels, we gzip later
        BINWRITE                            (JOJ_FROM, wb, JOJPIX,                   \
                                             readsize/4, buff                        );

    }; return 0;                                                                            };

//   ---     ---     ---     ---     ---
/*
void DECJOJ(BIN* src, float* pixels)        {

    uint size=0;
    uint deflsize=0;

    int wb; BIN* joj;

    rewind(src->file);
    fseek(src->file, 0,           SEEK_CUR);
    fseek(src->file, sizeof(SIG), SEEK_CUR);
    fseek(src->file, 0,           SEEK_CUR);

    fseek(src->file, 0,           SEEK_CUR);
    BINREAD(src, wb, uint, 1, &size    );
    BINREAD(src, wb, uint, 1, &deflsize);
    fseek(src->file, 0,           SEEK_CUR);

    char* srcpath=PTHBIN(src);
    MEM* path=MKSTR(srcpath, 4); RPSTR(path, "_dec.joj", path->bsize-9);
    joj=MKJOJ((char*) path->buff, 3, size); DLMEM(path);

    uint size_squared=size*size;
    uint bytesize=((size_squared*1)*sizeof(JOJPIX));
    INFLBIN(src, joj, bytesize, deflsize, sizeof(uint), sizeof(uint)*2);

    rewind(joj->file);
    fseek(joj->file,           0, SEEK_CUR);
    fseek(joj->file, sizeof(SIG)+sizeof(uint), SEEK_CUR);
    fseek(joj->file,           0, SEEK_CUR);

//   ---     ---     ---     ---     ---

    JOJPIX* buff;
    uint* loc_p=GTJOJ(joj); size=*loc_p;
    loc_p++; buff=(JOJPIX*) loc_p;

    for(uint z=0; z<4; z++) {               // loop through the four layers

        uint start=z*size_squared*4;        // read from joj
        BINREAD(joj, wb, JOJPIX, size_squared, buff);

        for(uint x=start, y=0;
            x<start+(size_squared*4);
            x+=4, y++               )       //convert jojpix to rgba and modify pixels

        { float* pixel = pixels+x; joj_to_rgba(pixel, buff+y); };

        break;

    };

//   ---     ---     ---     ---     ---

    BINCLOSE(joj); DLMEM(joj);                                                              };
*/
//   ---     ---     ---     ---     ---

int ZPJOJ(uint size_i, uint count,
          JOJH* jojh              )         {

    uint size_d   = 0;
    uint offs_i   = 0;

    uint offs_d   =                         (sizeof(uint)*3) + (count * KVR_IDK_WIDTH    );

                                            // zip the entire file
    DEFLBIN                                 (JOJ_FROM, JOJ_TO, size_i, &size_d,          \
                                             offs_i, offs_d                              );

//   ---     ---     ---     ---     ---    // add sizes to header

    int  wb;
    uint sizes[3] =                         { size_i, size_d, count                      };

                                            // rewind TO
    rewind                                  (JOJ_TO->file                                );
    fseek                                   (JOJ_TO->file, 0, SEEK_CUR                   );

                                            // skip the signature
    fseek                                   (JOJ_TO->file, sizeof(SIG), SEEK_CUR         );

                                            // still the cursor and write
    fseek                                   (JOJ_TO->file, 0, SEEK_CUR                   );
    BINWRITE                                (JOJ_TO, wb, uint, 3, sizes                  );
    BINWRITE                                (JOJ_TO, wb, JOJH, count, jojh               );
}

//   ---     ---     ---     ---     ---
