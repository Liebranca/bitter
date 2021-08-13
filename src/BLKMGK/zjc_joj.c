/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    ZJC_JOJ                               *
*                                           *
*     -image degrader                       *
*     -total datapacker                     *
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

#include "zjc_joj.h"
#include "kvr_zwrap.h"

#include "../KVRNEL/kvr_paths.h"
#include "../KVRNEL/MEM/kvr_str.h"
#include "../KVRNEL/TYPES/zjc_hash.h"

#include <string.h>

//   ---     ---     ---     ---     ---
// fun pointer for swapping out encoders
static void (*JOJENC_F)(float* pix, JOJPIX* j);

typedef struct JOJ_PALETTE_COLOR {          // format for hashing colors

    ID     id;                              // lookup identifier
    uint   n;                               // numerical representation
    JOJPIX pix;                             // the actual color values

} JOJPC;

static JOJPC JOJ_PALETTE[ZJC_DAFSIZE];      // palette storage
static HASH* JOJ_PALHASH;                   // addr table

                                            // size of header for *zipped* files
static uint  PKD_JOJH_SIZE =                ( (sizeof(uint)*3) + (count * sizeof(JOJH)) \
                                            + (ZJC_DAFSIZE * sizeof(JOJPIX)             \
                                            + (ZJC_DAFSIZE)                             );

//   ---     ---     ---     ---     ---

#define JOJ_READCOUNT                       /* how many pixels we read on each run    */     \
                                                                                             \
    xELEMS(ZJC_DAFPAGE, JOJPIX)             /* jojpix that fit in a page              */     \
    /*-----------------------*/             /* over                                   */     \
    /ZJC_RAWCOL_ELEMS                       /* how many elements are in a pixel       */

//   ---     ---     ---     ---     ---

int NTJOJENG(int mode)                      {

    jpalette=MKHASH(6, "JOJ_PALETTE");      // is just a test

    int   evilstate;

                                            // make a convenience string
    MEM*  m       = MKSTR                   (GTPECWD(), 6, 0                             );
    char* path    = MEMBUFF(m, char, 0);

//   ---     ---     ---     ---     ---

    BIN** joj;                              // fetch adeq bin slot for current op
    if(mode==JOJ_ENCODE)                    { joj = GTBINTO();                           }
    else                                    { joj = GTBINFR();                           };

                                            // ensure joj is wiped when we *write* to it
    int mate_rmode =                        (mode == JOJ_ENCODE) ? KVR_FMODE_WB
                                                                 : KVR_FMODE_RBP;

                                            // set fname and open
    RPSTR                                   (&m, "\\JOJ", strlen(path)                   );
    BINOPEN                                 ((*joj), path, mate_rmode,                   \
                                             KVR_FTYPE_JOJ, 0, evilstate                 );

//   ---     ---     ---     ---     ---

    if(mode==JOJ_ENCODE) {

                                            // quick blank mem for a flood fill
        MEM* m        = MKSTR               ("ZPBLNK", PKD_JOJH_SIZE, 1                  );

                                            // skip the signature
        fseek                               ((*joj)->file, 0, SEEK_CUR                   );
        fseek                               ((*joj)->file, sizeof(SIG), SEEK_CUR         );
        fseek                               ((*joj)->file, 0, SEEK_CUR                   );

                                            // add padding so we can jump without crashing
        fseek                               ((*joj)->file, 0, SEEK_CUR                   );
        BINWRITE                            ((*joj), wb, uchar, PKD_JOJH_SIZE, m->buff   );
        fseek                               ((*joj)->file, 0, SEEK_CUR                   );

                                            // free blank mem
        DLMEM                               (m                                           );

                                            // nit deflate;
        NTDEFL                              (                                            );

    }                                       // nit inflate
    else                                    { NTINFL();                                  };

//   ---     ---     ---     ---     ---

                                            // free convenience string
    DLMEM                                   (m                                           );
    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

int DLJOJENG(int mode)                      {

    CALOUT('E', "Palette: %u/%u colors used", jpalette->nitems, jpalette->nslots*8);
    DLMEM(jpalette);

//   ---     ---     ---     ---     ---

    BIN** joj;                              // fetch adeq bin slot for current op
    if(mode==JOJ_ENCODE)                    { joj = GTBINTO(); DLDEFL();              }
    else                                    { joj = GTBINFR(); DLINFL();              };

                                            // close
    BINCLOSE                                ((*joj));
    DLMEM                                   (*joj  );

    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

int ENCJOJ(float* src, JOJH* jojh,
           HRW*   h              )          {

    int     wb;
    uint    sq_dim  =                       (jojh->dim) * (jojh->dim);               \

                                            // fetch
    BIN**   to      = GTBINTO               (                                        );
    MEM*    wt      = GTBINWT               (                                        );

                                            // get buff from static mem
    uchar*  buff    = MEMBUFF               (wt, uchar, 0                            );

    JOJPIX  jpix    = {0};                  // empty, intermediate write-to

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

    JOJPC*  jpc     = NULL;                 // hash fetch target
    void*   p_ptr   = NULL;                 // voidstar dummy for jpc-get

    char    idname[ZJC_IDK_WIDTH];          // buff for color id key

//   ---     ---     ---     ---     ---

    while(remain) {                         // read one pixel-wide page at a time
                                            // each pixel being __always__ 32-bit RGBA

        uint readsize  =                    (remain<page) ? remain : page;           \
        remain        -= readsize;          // discount read from pending

        for(uint x=0, y=0; x<(readsize);    // go through page and jojencode
            x+=ZJC_RAWCOL_ELEMS, y++   )    {

                                            // get the color
            JOJENC_F                        (src+x, jpix                             );

                                            // make an id
            snprintf                        (idname, ZJC_IDK_WIDTH-1, "%u.%u%.%u.%u" ,
                                             buff[y].luma, buff[y].chroma_u          ,
                                             buff[y].chroma_v, buff[y].alpha         );

                                            // check id against palette
            STR_HASHGET                     ( JOJ_PALHASH, idname, p_ptr, 0          );

//   ---     ---     ---     ---     ---

            if(!p_ptr) {                    // on new color, save to palette

                if(JOJ_PALHASH->nitems==ZJC_DAFSIZE) {
                    CALOUT                  ('E', "BAD JOJ: using over %u colors"    ,
                                            ZJC_DAFSIZE                              )

                    return ERROR;

                };

                                            // set values on new entry
                jpc      =                  JOJ_PALETTE+(JOJ_PALHASH->nitems);       \
                jpc->id  = IDNEW            ("COL*", idname                          );
                jpc->pix = jpix;                                                     \
                jpc->n   =                  JOJ_PALHASH->nitems                      \

                                            // insert on table
                HASHSET                     (JOJ_PALHASH, jpc                        );

            }
                                            // on repeat color, fetch from palette
            else                            { jpc = (JOJPC*) p_ptr;                  };

            buff[y]=jpc->n;                 /* save color index to buff */           };

//   ---     ---     ---     ---     ---

        DEFLBUF                             (buff, (*to), readsize                   ,
                                             &(h->size_d), 0, PKD_JOJH_SIZE+h->size_i,
                                             h->idex==(h->total-1) && !remain        );

    };
                                            // add to archive's total
    h->size_i      +=                       sq_dim*sizeof(uchar);                    \
    return DONE;                                                                         };

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

int ZPJOJ(HRW h, JOJH* jojh)                {

                                            // add sizes to header
    uint  sizes[4] =                        { h->size_i, h->size_d, h->total   ,
                                              JOJ_PALHASH->nitems              };

                                            // fetch bin
    BIN** to       = GTBINTO                (                                  );

                                            // rewind TO
    rewind                                  ((*to)->file                       );
    fseek                                   ((*to)->file, 0, SEEK_CUR          );
    fseek                                   ((*to)->file, sizeof(SIG), SEEK_CUR);

                                            // still the cursor and write header
    fseek                                   ((*to)->file, 0, SEEK_CUR          );
    BINWRITE                                ((*to), wb, uint, 3, sizes         );
    BINWRITE                                ((*to), wb, JOJH, count, jojh      );
    fseek                                   ((*to)->file, 0, SEEK_CUR          );

//   ---     ---     ---     ---     ---

                                            // make temp for palette write values
    MEM* pal       = MKSTR                  ("JOJPAL",                         \
                                                                               \
                                            ( (sizeof(JOJPIX)*ZJC_DAFSIZE)     \
                                            + (sizeof(uchar )*ZJC_DAFSIZE) )   ,

                                            1                                  );

    for(uint x=0, y=0; y<h->total;
        y++, x+=5                )  {       // copy palette data to buff

        JOJPC jpc = JOJ_PALETTE[y];

        pal[x+0]  = jpc->n;

        pal[x+1]  = jpc->pix.chroma_u;
        pal[x+2]  = jpc->pix.chroma_v;
        pal[x+3]  = jpc->pix.luma;
        pal[x+4]  = jpc->pix.alpha;

    };

//   ---     ---     ---     ---     ---

                                            // write palette data
    fseek                                   ((*to)->file, 0, SEEK_CUR          );
    BINWRITE                                ((*to), wb, uchar, ZJC_DAFSIZE*5   ,
                                             JOJ_PALETTE                       );
    fseek                                   ((*to)->file, 0, SEEK_CUR          );

    DLMEM(pal);
    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

int UZPJOJ(JOJH* jojh, uint* sizes)         {

                                            // fetch bin & buff
    BIN**  from    = GTBINFR                (                                  );
    MEM*   rd      = GTBINRD                (                                  );
    uchar* buff    = MEMBUFF                (rd, uchar, 0                      );

    int  wb;                                // read file header
    fseek                                   ((*from)->file, 0, SEEK_CUR                     );
    BINREAD                                 ((*from), wb, uint, 4, sizes                    );
    BINREAD                                 ((*from), wb, JOJH, sizes[2], jojh              );
    fseek                                   ((*from)->file, 0, SEEK_CUR                     );

//   ---     ---     ---     ---     ---

                                            // unzip
    INFLBUF                                 ((*from), buff, sizes[0], sizes[1],          \
                                             0, PKD_JOJH_SIZE                               );

                                            // rewind & skip sig
    rewind                                  (JOJ_TO->file                                   );
    fseek                                   (JOJ_TO->file, 0, SEEK_CUR                      );
    fseek                                   (JOJ_TO->file, sizeof(SIG), SEEK_CUR            );
    fseek                                   (JOJ_TO->file, 0, SEEK_CUR                      );

    return DONE;                                                                            };

//   ---     ---     ---     ---     ---