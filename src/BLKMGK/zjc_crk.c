#include "zjc_crk.h"
#include "kvr_zwrap.h"

#include "../KVRNEL/kvr_paths.h"
#include "../KVRNEL/MEM/kvr_str.h"

#include <string.h>

//   ---     ---     ---     ---     ---
// fun pointer for swapping out encoders
static void (*CRKENC_F)(float* v, CRKVRT* c);

BIN* CRK_FROM;                              // bins used for I/O
BIN* CRK_TO;
BIN* CRK_VRTDUMP;

MEM* CRK_VRTS;                              // a vertex buffer (keep it small)

//   ---     ---     ---     ---     ---

#define CRK_READCOUNT                       /* how many verts we read on each run     */     \
                                                                                             \
    xELEMS(ZJC_DAFPAGE, CRKVRT)             /* bcrkvrt that fit in a page             */     \
    /*-----------------------*/             /* over                                   */     \
    /ZJC_RAWVRT_ELEMS                       /* how many elements are in a vertex      */

//   ---     ---     ---     ---     ---

BIN* MKCRK(char* path, uint  mode)          {

    BIN* crk;
    int  evilstate;
                                            // make it big enough for *encoded* verts
    uint page = xBYTES                      (CRK_READCOUNT, CRKVRT            );
    BINOPEN                                 (crk, path, mode, KVR_FTYPE_CRK,  \
                                             page, evilstate                  );

    return crk;                                                                             };

//   ---     ---     ---     ---     ---

CRKVRT* GTCRK(BIN* crk)                     {

    char* path = PTHBIN                     (crk                          );
    uint  loc  =                            (strlen(path)+1) * sizeof(char);

    return MEMBUFF                          (byref(crk->m), CRKVRT, loc  );                 };

//   ---     ---     ---     ---     ---

int NTCRKENG(int mode)                      {

    int   evilstate;
                                            // make a convenience string
    MEM*  m       = MKSTR                   (GTPECWD(), 64, 0                         );
    char* path    = MEMBUFF(m, char, 0);

//   ---     ---     ---     ---     ---    // set fname and create/open files

    RPSTR                                   (&m, "\\MEZZCRK.crk", strlen(path)        );
    CRK_FROM = MKCRK                        (path, KVR_FMODE_RBP                      );

    if(CRK_FROM == (BIN*) ERROR)            { return ERROR;                           };

                                            // ensure mezz is wiped when we *write* to it
    int mezz_rmode =                        (mode == CRK_ENCODE) ? KVR_FMODE_WB       \
                                                                 : KVR_FMODE_RBP;     \

    RPSTR                                   (&m, "MEZZ.crk", strlen(path)-11          );
    BINOPEN                                 (CRK_TO, path, mezz_rmode,                \
                                             KVR_FTYPE_CRK, 0, evilstate              );

    RPSTR                                   (&m, "VRTDUMP.hx", strlen(path)-8         );
    BINOPEN                                 (CRK_VRTDUMP, path, KVR_FMODE_RBP,        \
                                             KVR_FTYPE_DMP, 0, evilstate              );

    if(mode == CRK_DECODE) {                // castling on invert op
        BIN* tmp = CRK_FROM;
        CRK_FROM = CRK_TO;
        CRK_TO   = tmp;
    };

//   ---     ---     ---     ---     ---    // make recycle vertex buffer

                                            // make it big enough to hold *unpacked* vertices
    uint page = xBYTES                      (CRK_READCOUNT, float) * ZJC_RAWVRT_ELEMS;\
    CRK_VRTS = MKSTR                        ("CRKBUF", page, 1                        );

                                            // free convenience string
    DLMEM                                   (m                                        );
    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

int DLCRKENG(int mode)                      {

    if(mode == CRK_DECODE) {                 // de-castling on invert op
        BIN* tmp = CRK_FROM;
        CRK_FROM = CRK_TO;
        CRK_TO   = tmp;
    };

                                            // close mezz
    BINCLOSE                                (CRK_TO     );
    DLMEM                                   (CRK_TO     );

                                            // remove temps
    BINREMOVE                               (CRK_FROM   );
    DLMEM                                   (CRK_FROM   );
    BINREMOVE                               (CRK_VRTDUMP);
    DLMEM                                   (CRK_VRTDUMP);

                                            // free recycle buff
    DLMEM                                   (CRK_VRTS   );
    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

int ENCCRK(CRKH* crkh, uint* size_i)        {

    int     wb;
    uint    dim =                           crkh->vert_count;                            \

                                            // get __to__ buff
    CRKVRT* buff   = GTCRK                  (CRK_FROM                                    );

//   ---     ---     ---     ---     ---

    CRKENC_F       = &ENCVRT;               // set encoder 
                                            // i don't think we'll need another
                                            // BUT: nice to have in case someday we do

//   ---     ---     ---     ---     ---

                                            // sizing ints we need later
    uint    page    =                       CRK_READCOUNT*ZJC_RAWVRT_ELEMS;              \
    uint    remain  =                       dim*ZJC_RAWVRT_ELEMS;                        \

                                            // add to archive's total
    *size_i        +=                       dim*sizeof(CRKVRT);                          \

                                            // get recycle vert buffer
    float* verts    = MEMBUFF               (CRK_VRTS, float, 0                          );

                                            // rewind cache and skip sig
    rewind                                  (CRK_VRTDUMP->file                           );
    fseek                                   (CRK_VRTDUMP->file, 0, SEEK_CUR              );
    fseek                                   (CRK_VRTDUMP->file, sizeof(SIG), SEEK_CUR    );
    fseek                                   (CRK_VRTDUMP->file, 0, SEEK_CUR              );

                                            // write indices
    wb=BIN2BIN                              (CRK_VRTDUMP, CRK_FROM,                      \
                                            (crkh->idex_count*3)*sizeof(crkh->idex_count));

    if(wb==ERROR)                           { return ERROR;                              };

//   ---     ---     ---     ---     ---

    while(remain) {                         // read one vert-wide page at a time

        uint readsize =                     (remain<page) ? remain : page;               \

                                            // load buffer from dump
        BINREAD                             (CRK_VRTDUMP, wb, float, readsize, verts     );

        remain -= readsize;                 // discount read from pending

        for(uint x=0, y=0; x<(readsize);    // go through page and bcrkencode
            x+=ZJC_RAWVRT_ELEMS, y++   )    { CRKENC_F(verts+x, buff+y);                 };

                                            // save encoded verts
        BINWRITE                            (CRK_FROM, wb, CRKVRT,                       \
                                             readsize/ZJC_RAWVRT_ELEMS, buff             );

    }; return DONE;                                                                         };

//   ---     ---     ---     ---     ---

int ZPCRK(uint size_i, uint count,
          CRKH* crkh              )         {

    int  wb;
    uint size_d   = 0;
    uint offs_i   = 0;

    uint offs_d   =                         (sizeof(uint)*3) + (count * sizeof(CRKH)     );

                                            // quick blank mem for a flood fill
    MEM* m        = MKSTR                   ("ZPBLNK", offs_d, 1                         );

                                            // skip the signature
    fseek                                   (CRK_TO->file, 0, SEEK_CUR                   );
    fseek                                   (CRK_TO->file, sizeof(SIG), SEEK_CUR         );
    fseek                                   (CRK_TO->file, 0, SEEK_CUR                   );

                                            // add padding so we can jump without crashing
    fseek                                   (CRK_TO->file, 0, SEEK_CUR                   );
    BINWRITE                                (CRK_TO, wb, uchar, offs_d, m->buff          );
    fseek                                   (CRK_TO->file, 0, SEEK_CUR                   );

                                            // free blank mem
    DLMEM                                   (m                                           );

                                            // zip the entire file
    DEFLBIN                                 (CRK_FROM, CRK_TO, size_i, &size_d,          \
                                             offs_i, offs_d                              );

//   ---     ---     ---     ---     ---    // add sizes to header

    uint sizes[3] =                         { size_i, size_d, count                      };

                                            // rewind TO
    rewind                                  (CRK_TO->file                                );
    fseek                                   (CRK_TO->file, 0, SEEK_CUR                   );

                                            // signature being deleted for some reason
                                            // even though we religiously skip it...
                                            // soooo, just rewrite it till we fix that
    //BINWRITE                                (CRK_TO, wb, SIG, 1, &(CRK_TO->sign)         );
                                            //^and dont forget we should be skipping intead:
    fseek                                   (CRK_TO->file, sizeof(SIG), SEEK_CUR         );

                                            // still the cursor and write
    fseek                                   (CRK_TO->file, 0, SEEK_CUR                   );
    BINWRITE                                (CRK_TO, wb, uint, 3, sizes                  );
    BINWRITE                                (CRK_TO, wb, CRKH, count, crkh               );

    return DONE;                                                                            };

//   ---     ---     ---     ---     ---