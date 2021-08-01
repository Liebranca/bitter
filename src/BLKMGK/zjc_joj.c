#include "zjc_joj.h"
#include "kvr_zwrap.h"

#include "../KVRNEL/kvr_paths.h"
#include "../KVRNEL/MEM/kvr_str.h"

#include <string.h>

//   ---     ---     ---     ---     ---
// fun pointer for swapping out encoders
static void (*JOJENC_F)(float* pix, JOJPIX* j);

BIN* JOJ_FROM;                              // bins used for I/O
BIN* JOJ_TO;
BIN* JOJ_PIXDUMP;

MEM* JOJ_PIXELS;                            // a pixel buffer (keep it small)

//   ---     ---     ---     ---     ---

#define JOJ_READCOUNT                       /* how many pixels we read on each run    */     \
                                                                                             \
    xELEMS(ZJC_DAFPAGE, JOJPIX)             /* jojpix that fit in a page              */     \
    /*-----------------------*/             /* over                                   */     \
    /ZJC_RAWCOL_ELEMS                       /* how many elements are in a pixel       */

//   ---     ---     ---     ---     ---

BIN* MKJOJ(char* path,
           uint  mode)                      {

    BIN* joj;
    int  evilstate;
                                            // make it big enough for *encoded* pixels
    uint page = xBYTES                      (JOJ_READCOUNT, JOJPIX                          );
    BINOPEN                                 (joj, path, mode, KVR_FTYPE_JOJ, page, evilstate);

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
    MEM*  m       = MKSTR                   (GTPECWD(), 64, 0                         );
    char* path    = MEMBUFF(m, char, 0);

//   ---     ---     ---     ---     ---    // set fname and create/open files

    RPSTR                                   (&m, "\\MATERAW.joj", strlen(path)        );
    JOJ_FROM = MKJOJ                        (path, KVR_FMODE_RBP                      );

    if(JOJ_FROM == (BIN*) ERROR)            { return ERROR;                           };

                                            // ensure mate is wiped when we *write* to it
    int mate_rmode =                        (mode == JOJ_ENCODE) ? KVR_FMODE_WB
                                                                 : KVR_FMODE_RBP;

    RPSTR                                   (&m, "MATE.joj", strlen(path)-11          );
    BINOPEN                                 (JOJ_TO, path, mate_rmode,                \
                                             KVR_FTYPE_JOJ, 0, evilstate              );

    RPSTR                                   (&m, "PIXDUMP.hx", strlen(path)-8         );
    BINOPEN                                 (JOJ_PIXDUMP, path, KVR_FMODE_RBP,        \
                                             KVR_FTYPE_DMP, 0, evilstate              );

    if(mode == JOJ_DECODE) {                // castling on invert op
        BIN* tmp = JOJ_FROM;
        JOJ_FROM = JOJ_TO;
        JOJ_TO   = tmp;
    };

//   ---     ---     ---     ---     ---    // make recycle pixel buffer

                                            // make it big enough to hold *unpacked* pixels
    uint page  = xBYTES                     (JOJ_READCOUNT, float) * ZJC_RAWCOL_ELEMS;\
    JOJ_PIXELS = MKSTR                      ("JOJBUF", page, 1                        );

                                            // free convenience string
    DLMEM                                   (m                                        );
    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

int DLJOJENG(int mode)                      {

    if(mode == JOJ_DECODE) {                // de-castling on invert op
        BIN* tmp = JOJ_FROM;
        JOJ_FROM = JOJ_TO;
        JOJ_TO   = tmp;
    };

                                            // close mate
    BINCLOSE                                (JOJ_TO     );
    DLMEM                                   (JOJ_TO     );

                                            // remove temps
    BINREMOVE                               (JOJ_FROM   );
    DLMEM                                   (JOJ_FROM   );
    BINREMOVE                               (JOJ_PIXDUMP);
    DLMEM                                   (JOJ_PIXDUMP);

                                            // free recycle buff
    DLMEM                                   (JOJ_PIXELS );
    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

int ENCJOJ(JOJH* jojh, uint* size_i)        {

    int     wb;
    uint    sq_dim  =                       (jojh->dim) * (jojh->dim);               \

                                            // get the joj
    JOJPIX* buff    = GTJOJ                 (JOJ_FROM                                );

//   ---     ---     ---     ---     ---

    JOJENC_F        = &ENCRGBA;             // set encoder 
    int  len        = strlen                (jojh->name                              );
    char imtype     =                       jojh->name[len-1];

    if  (imtype=='n'               )        { JOJENC_F=&ENCNVEC;                     }
    elif(imtype=='o' || imtype=='c')        { JOJENC_F=&ENCGREY;                     };

//   ---     ---     ---     ---     ---

                                            // sizing ints we need later
    uint    page    =                       JOJ_READCOUNT*ZJC_RAWCOL_ELEMS;          \
    uint    remain  =                       sq_dim*ZJC_RAWCOL_ELEMS;                 \

                                            // add to archive's total
    *size_i        +=                       sq_dim*sizeof(JOJPIX);                   \

                                            // get recycle pixel buffer
    float* pixels   = MEMBUFF               (JOJ_PIXELS, float, 0                    );

                                            // rewind cache and skip sig
    rewind                                  (JOJ_PIXDUMP->file                       );
    fseek                                   (JOJ_PIXDUMP->file, 0, SEEK_CUR          );
    fseek                                   (JOJ_PIXDUMP->file, sizeof(SIG), SEEK_CUR);
    fseek                                   (JOJ_PIXDUMP->file, 0, SEEK_CUR          );

//   ---     ---     ---     ---     ---

    while(remain) {                         // read one pixel-wide page at a time
                                            // each pixel being __always__ 32-bit RGBA

        uint readsize =                     (remain<page) ? remain : page;           \

                                            // load buffer from dump
        BINREAD                             (JOJ_PIXDUMP, wb, float, readsize, pixels);

        remain -= readsize;                 // discount read from pending

        for(uint x=0, y=0; x<(readsize);    // go through page and jojencode
            x+=ZJC_RAWCOL_ELEMS, y++   )    { JOJENC_F(pixels+x, buff+y);            };

                                            // save encoded pixels, we gzip later
        BINWRITE                            (JOJ_FROM, wb, JOJPIX,                   \
                                             readsize/ZJC_RAWCOL_ELEMS, buff         );

    }; return DONE;                                                                         };

//   ---     ---     ---     ---     ---

int DECJOJ(JOJH* jojh)                      {

    int     wb;

    uint    dim     = jojh->dim;
    uint    sq_dim  = dim*dim;

                                            // set color compression level
    STCFRACL                                (jojh->fracl                  );

                                            // get the joj
    JOJPIX* buff    = GTJOJ                 (JOJ_TO                       );

//   ---     ---     ---     ---     ---

    JOJENC_F        = &DECRGBA;             // set encoder 
    int len         = strlen                (jojh->name                   );
    char imtype     =                       jojh->name[len-1];

    if  (imtype=='n'               )        { JOJENC_F=&DECNVEC;          }
    elif(imtype=='o' || imtype=='c')        { JOJENC_F=&DECGREY;          };

//   ---     ---     ---     ---     ---

                                            // sizing ints we need later
    uint    page    =                       JOJ_READCOUNT;                \
    uint    remain  =                       sq_dim;                       \

                                            // get recycle pixel buffer
    float* pixels   = MEMBUFF               (JOJ_PIXELS, float, 0         );

                                            // rewind target and write header
    rewind                                  (JOJ_PIXDUMP->file);
    fseek                                   (JOJ_PIXDUMP->file, 0, SEEK_CUR);
    BINWRITE                                (JOJ_PIXDUMP, wb, JOJH,        \
                                             1, jojh                       );
    fseek                                   (JOJ_PIXDUMP->file, 0, SEEK_CUR);

//   ---     ---     ---     ---     ---

    while(remain) {                         // read one joj-wide page at a time
                                            // each joj being 8(fracl'd)-bit Y'UV+A

        uint readsize =                     (remain<page) ? remain : page;           \

                                            // load buffer from dump
        BINREAD                             (JOJ_TO, wb, JOJPIX, readsize, buff      );

        remain -= readsize;                 // discount read from pending

        for(uint x=0, y=0; y<(readsize);    // go through page and joj decode
            x+=ZJC_RAWCOL_ELEMS, y++  )     { JOJENC_F(pixels+x, buff+y);            };

                                            // save decoded pixels so caller can read
        BINWRITE                            (JOJ_PIXDUMP, wb, float,                 \
                                             readsize*ZJC_RAWCOL_ELEMS, pixels       );

    }; return DONE;                                                                         };

//   ---     ---     ---     ---     ---

int ZPJOJ(uint size_i, uint count,
          JOJH* jojh              )         {

    int  wb;
    uint size_d   = 0;
    uint offs_i   = 0;

    uint offs_d   =                         (sizeof(uint)*3) + (count * sizeof(JOJH)     );

                                            // quick blank mem for a flood fill
    MEM* m        = MKSTR                   ("ZPBLNK", offs_d, 1                         );

                                            // skip the signature
    fseek                                   (JOJ_TO->file, 0, SEEK_CUR                   );
    fseek                                   (JOJ_TO->file, sizeof(SIG), SEEK_CUR         );
    fseek                                   (JOJ_TO->file, 0, SEEK_CUR                   );

                                            // add padding so we can jump without crashing
    fseek                                   (JOJ_TO->file, 0, SEEK_CUR                   );
    BINWRITE                                (JOJ_TO, wb, uchar, offs_d, m->buff          );
    fseek                                   (JOJ_TO->file, 0, SEEK_CUR                   );

                                            // free blank mem
    DLMEM                                   (m                                           );

                                            // zip the entire file
    DEFLBIN                                 (JOJ_FROM, JOJ_TO, size_i, &size_d,          \
                                             offs_i, offs_d                              );

//   ---     ---     ---     ---     ---    // add sizes to header

    uint sizes[3] =                         { size_i, size_d, count                      };

                                            // rewind TO
    rewind                                  (JOJ_TO->file                                );
    fseek                                   (JOJ_TO->file, 0, SEEK_CUR                   );

                                            // signature being deleted for some reason
                                            // even though we religiously skip it...
                                            // soooo, just rewrite it till we fix that
    BINWRITE                                (JOJ_TO, wb, SIG, 1, &(JOJ_TO->sign)         );
                                            //^and dont forget we should be skipping intead:
    //fseek                                   (JOJ_TO->file, sizeof(SIG), SEEK_CUR         );

                                            // still the cursor and write
    fseek                                   (JOJ_TO->file, 0, SEEK_CUR                   );
    BINWRITE                                (JOJ_TO, wb, uint, 3, sizes                  );
    BINWRITE                                (JOJ_TO, wb, JOJH, count, jojh               );

    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

int UZPJOJ(JOJH* jojh, uint* sizes)         {

    int  wb;                                // read file header
    fseek                                   (JOJ_FROM->file, 0, SEEK_CUR                    );
    BINREAD                                 (JOJ_FROM, wb, uint, 3, sizes                   );
    BINREAD                                 (JOJ_FROM, wb, JOJH, sizes[2], jojh             );
    fseek                                   (JOJ_FROM->file, 0, SEEK_CUR                    );

//   ---     ---     ---     ---     ---

    uint offs_i = 0;                        // skip header from unzipping
    uint offs_d =                           (sizeof(uint)*3) + (sizeof(JOJH)*sizes[2]);     \

                                            // unzip
    INFLBIN                                 (JOJ_FROM, JOJ_TO, sizes[0], sizes[1],          \
                                             offs_i, offs_d                                 );

                                            // rewind & skip sig
    rewind                                  (JOJ_TO->file                                   );
    fseek                                   (JOJ_TO->file, 0, SEEK_CUR                      );
    fseek                                   (JOJ_TO->file, sizeof(SIG), SEEK_CUR            );
    fseek                                   (JOJ_TO->file, 0, SEEK_CUR                      );

    return DONE;                                                                            };

//   ---     ---     ---     ---     ---