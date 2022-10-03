/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    KVR_ZWRAP                             *
*                                           *
*     -'wraps' zlib calls                   *
*     -deflates/inflates bins               *
*     -stream-deal pending                  *
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

#include "kvr_zwrap.h"
#include "ZLIB/zlib.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

//  - --- - --- - --- - --- -

#define windowBits 15
#define GZIP_ENCODING 16
#define ENABLE_ZLIB_GZIP 32

//  - --- - --- - --- - --- -

static z_stream ZLIB_STRM   = {0};
static STR*     ZLIB_MEM    = NULL;
static int      ZLIB_STATUS = 0;

char* CHR_ZLIB_STATUS(void)                 {

    switch(ZLIB_STATUS) {

        case -1: return "Z_ERRNO";
        case -2: return "Z_STREAM_ERROR";
        case -3: return "Z_DATA_ERROR";
        case -4: return "Z_MEM_ERROR";
        case -5: return "Z_BUF_ERROR";
        case -6: return "Z_VERSION_ERROR";

        case  1: return "Z_STREAM_END";
        case  2: return "Z_NEED_DICT";

        default: return "Z_OK";

    };                                                                                      };

#define CALL_ZLIB(x) {                                                      \
        ZLIB_STATUS = x;                                                    \
        if (ZLIB_STATUS < 0) {                                              \
                                                                            \
        if(KVR_DEBUG) { CALOUT('E', "FAILED %s\n\b", #x); }                 \
                                                                            \
            __terminator(4, CHR_ZLIB_STATUS());                             \
        }                                                                   \
    }

//  - --- - --- - --- - --- -

static void defstrm_init (z_stream * strm)  {

    strm->zalloc = Z_NULL;
    strm->zfree  = Z_NULL;
    strm->opaque = Z_NULL;

    CALL_ZLIB (deflateInit2 (strm, Z_BEST_COMPRESSION, Z_DEFLATED,
                             windowBits | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY));           };

static void infstrm_init(z_stream* strm)    {

    strm->zalloc       = Z_NULL;
    strm->zfree        = Z_NULL;
    strm->opaque       = Z_NULL;

    CALL_ZLIB(inflateInit2(strm, windowBits | ENABLE_ZLIB_GZIP));                           };

//  - --- - --- - --- - --- -

void NTINFL(void)                           {

    ZLIB_STRM.next_in  = Z_NULL;
    ZLIB_STRM.avail_in = 0;

    infstrm_init                            (&ZLIB_STRM                  );

    if(!ZLIB_MEM) {
        ZLIB_MEM = MKSTR                    ("ZLIB_MEM", ZJC_DAFPAGE*2, 1); }               };

void DLINFL(void)                           {

    DLMEM     (ZLIB_MEM  );
    inflateEnd(&ZLIB_STRM);                                                                 };

//   ---     ---     ---     ---     ---

int INFLBIN(BIN* src,    BIN* dst,
            uint size_i, uint size_d,
            uint offs_i, uint offs_d)       {

    NTINFL();

                                            // fetch locs
    uchar*   in       = MEMBUFF             (ZLIB_MEM, uchar, 0          );
    uchar*   out      = MEMBUFF             (ZLIB_MEM, uchar, ZJC_DAFPAGE);

    uint     readsize = ZJC_DAFPAGE;
    uint     dataleft = size_d;

    rewind(src->file); rewind(dst->file);
    fseek(src->file,           0,        SEEK_CUR);
    fseek(src->file, sizeof(SIG)+offs_d, SEEK_CUR);
    fseek(src->file,           0,        SEEK_CUR);
    fseek(dst->file,           0,        SEEK_CUR);
    fseek(dst->file, sizeof(SIG)+offs_i, SEEK_CUR);
    fseek(dst->file,           0,        SEEK_CUR);

    int bytes_read; uint tot=0;

//  - --- - --- - --- - --- -

    while(dataleft) {
                                            // read next block to decompress
        if(readsize > dataleft)             { readsize = dataleft;                };
        fseek                               (src->file, 0, SEEK_CUR               );
        BINREAD                             (src, bytes_read, uchar, readsize, in );
        fseek                               (src->file, 0, SEEK_CUR               );

        ZLIB_STRM.avail_in = readsize;
        ZLIB_STRM.next_in  = in;

        dataleft     -= readsize;

//  - --- - --- - --- - --- -

        while(ZLIB_STRM.avail_in || !ZLIB_STRM.avail_out) {

            uint have;

            ZLIB_STRM.avail_out = ZJC_DAFPAGE;
            ZLIB_STRM.next_out  = out;
            CALL_ZLIB(inflate(&ZLIB_STRM, Z_NO_FLUSH));

            have           = ZJC_DAFPAGE - ZLIB_STRM.avail_out;

            fseek                           (dst->file, 0, SEEK_CUR           );
            BINWRITE                        (dst, bytes_read, uchar, have, out);
            fseek                           (dst->file, 0, SEEK_CUR           );

            tot+=have;

        };
    };

    DLINFL();

//  - --- - --- - --- - --- -

#if KVR_DEBUG & KVR_CALOF
    CALOUT('F', "\n\bINFLATED %u/%u\n\b", tot, size_i);
#endif

    return DONE;                                                                            };

//  - --- - --- - --- - --- -

void NTDEFL(void)                           {

    ZLIB_STRM.next_in  = Z_NULL;
    ZLIB_STRM.avail_in = 0;

    defstrm_init                            (&ZLIB_STRM                  );

    if(!ZLIB_MEM) {
        ZLIB_MEM = MKSTR                    ("ZLIB_MEM", ZJC_DAFPAGE*2, 1); }               };

void DLDEFL(void)                           {

    DLMEM     (ZLIB_MEM  );
    deflateEnd(&ZLIB_STRM);                                                                 };

//   ---     ---     ---     ---     ---

int DEFLBUF(void* src, BIN* dst      ,
            uint size_i, uint* size_d,
            uint offs_i, uint offs_d )      {

                                            // fetch locs
    uchar*   out = MEMBUFF                  (ZLIB_MEM, uchar, ZJC_DAFPAGE           );
    uchar*   in  = (uchar*) src;

                                            // rewind and skip
    rewind                                  (dst->file                              );
    fseek                                   (dst->file,           0,        SEEK_CUR);
    fseek                                   (dst->file, sizeof(SIG)+offs_d, SEEK_CUR);
    fseek                                   (dst->file,           0,        SEEK_CUR);

    int flush; int wb;
    uint dataleft=size_i;

//  - --- - --- - --- - --- -

    while(dataleft) {

        uint readsize            =          (readsize > dataleft) ? dataleft : ZJC_DAFPAGE;

        ZLIB_STRM.next_in        =          in;
        ZLIB_STRM.avail_in       =          readsize;

        dataleft                -=          readsize;
        flush                    =          (dataleft) ? Z_NO_FLUSH : Z_FINISH;

//  - --- - --- - --- - --- -

        while(ZLIB_STRM.avail_in || !ZLIB_STRM.avail_out) {

            uint have;

            ZLIB_STRM.avail_out  =          ZJC_DAFPAGE;
            ZLIB_STRM.next_out   =          out;

            CALL_ZLIB                       (deflate (& ZLIB_STRM, flush));

            have                 =          ZJC_DAFPAGE - ZLIB_STRM.avail_out;
            (*size_d)           +=          have;

            fseek                           (dst->file, 0, SEEK_CUR   );
            BINWRITE                        (dst, wb, uchar, have, out);
            fseek                           (dst->file, 0, SEEK_CUR   );

        };  in                  +=          readsize;

    };

//  - --- - --- - --- - --- -

#if KVR_DEBUG & KVR_CALOF
    int finalsize = (int) (100.0 - (( (float) ( *size_d) / (float) (size_i) ) * 100));
    if(finalsize < 0) { CALOUT('F', "BAD DEFLATE: %u/%u | approx. %i%% file size increase\n",
                              *size_d, size_i, -finalsize                                ); }

    else              { CALOUT('F', "GOOD DEFLATE: %u/%u | approx. %i%% file size reduction\n",
                               *size_d, size_i,  finalsize                               ); };
#endif

    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

int DEFLBIN(BIN* src,    BIN* dst,
            uint size_i, uint* size_d,
            uint offs_i, uint offs_d)      {

    NTDEFL();

                                            // fetch locs
    uchar*   in       = MEMBUFF             (ZLIB_MEM, uchar, 0          );
    uchar*   out      = MEMBUFF             (ZLIB_MEM, uchar, ZJC_DAFPAGE);

    rewind(src->file); rewind(dst->file);
    fseek(src->file,           0,        SEEK_CUR);
    fseek(src->file, sizeof(SIG)+offs_i, SEEK_CUR);
    fseek(src->file,           0,        SEEK_CUR);
    fseek(dst->file,           0,        SEEK_CUR);
    fseek(dst->file, sizeof(SIG)+offs_d, SEEK_CUR);
    fseek(dst->file,           0,        SEEK_CUR);

    int  bytes_read; int flush;
    uint dataleft=size_i;

//  - --- - --- - --- - --- -

    while(dataleft) {

        uint readsize = ZJC_DAFPAGE; if(readsize > dataleft) { readsize = dataleft; }

                                            // read next block to compress
        fseek                               (src->file, 0, SEEK_CUR              );
        BINREAD                             (src, bytes_read, uchar, readsize, in);
        fseek                               (src->file, 0, SEEK_CUR              );

        ZLIB_STRM.next_in   = in;
        ZLIB_STRM.avail_in  = bytes_read;

        dataleft      -= bytes_read;
        flush          = (dataleft) ? Z_NO_FLUSH : Z_FINISH;

//  - --- - --- - --- - --- -

        while(ZLIB_STRM.avail_in || !ZLIB_STRM.avail_out) {

            uint have;

            ZLIB_STRM.avail_out = ZJC_DAFPAGE;
            ZLIB_STRM.next_out  = out;

            CALL_ZLIB (deflate (& ZLIB_STRM, flush));

            have           = ZJC_DAFPAGE - ZLIB_STRM.avail_out;
            (*size_d)     += have;

            fseek                           (dst->file, 0, SEEK_CUR           );
            BINWRITE                        (dst, bytes_read, uchar, have, out);
            fseek                           (dst->file, 0, SEEK_CUR           );

        };
    };

    DLDEFL();

//  - --- - --- - --- - --- -

#if KVR_DEBUG & KVR_CALOF
    int finalsize = (int) (100.0 - (( (float) ( *size_d) / (float) (size_i) ) * 100));
    if(finalsize < 0) { CALOUT('F', "BAD DEFLATE: %u/%u | approx. %i%% file size increase\n",
                              *size_d, size_i, -finalsize                                ); }

    else              { CALOUT('F', "GOOD DEFLATE: %u/%u | approx. %i%% file size reduction\n",
                               *size_d, size_i,  finalsize                               ); };
#endif

    return DONE;                                                                            };

//  - --- - --- - --- - --- -