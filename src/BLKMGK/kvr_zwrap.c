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

static int ZLIB_STATUS=0;

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
        if(KVR_DEBUG) { CALOUT("FAILED %s\n\b", #x); }                      \
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

int INFLBIN(BIN* src,    BIN* dst,
            uint size_i, uint size_d,
            uint offs_i, uint offs_d)       {

    uchar    in [ZJC_DAFPAGE];
    uchar    out[ZJC_DAFPAGE];

    uint     readsize = ZJC_DAFPAGE;
    uint     dataleft = size_d;

    z_stream strm     = {0};
    strm.next_in      = Z_NULL;
    strm.avail_in     = 0;

    infstrm_init(&strm);

    rewind(src->file); rewind(dst->file);
    fseek(src->file,           0,        SEEK_CUR);
    fseek(src->file, sizeof(SIG)+offs_d, SEEK_CUR);
    fseek(src->file,           0,        SEEK_CUR);
    fseek(dst->file,           0,        SEEK_CUR);
    fseek(dst->file, sizeof(SIG)+offs_i, SEEK_CUR);
    fseek(dst->file,           0,        SEEK_CUR);

    int  bytes_read; dataleft;

//  - --- - --- - --- - --- -

    while(dataleft) {
                                            // read next block to decompress
        if(readsize > dataleft)             { readsize = dataleft;                };
        fseek                               (src->file, 0, SEEK_CUR               );
        BINREAD                             (src, bytes_read, uchar, readsize, in );
        fseek                               (src->file, 0, SEEK_CUR               );

        strm.avail_in = readsize;
        strm.next_in  = in;

        dataleft     -= readsize;

//  - --- - --- - --- - --- -

        while(strm.avail_in) {              // decompress block and write to dst

            uint have;

            strm.avail_out = ZJC_DAFPAGE;
            strm.next_out  = out;
            CALL_ZLIB(inflate(&strm, Z_NO_FLUSH));

            have           = ZJC_DAFPAGE - strm.avail_out;

            fseek                           (dst->file, 0, SEEK_CUR           );
            BINWRITE                        (dst, bytes_read, uchar, have, out);
            fseek                           (dst->file, 0, SEEK_CUR           );

        };
    };

//  - --- - --- - --- - --- -

    inflateEnd (& strm);

    return DONE;                                                                            };

//  - --- - --- - --- - --- -

int DEFLBIN(BIN* src,    BIN* dst,
            uint size_i, uint* size_d,
            uint offs_i, uint offs_d)      {

    uchar    in [ZJC_DAFPAGE];
    uchar    out[ZJC_DAFPAGE];

    z_stream strm={0};

    defstrm_init(&strm);

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

        strm.next_in   = in;
        strm.avail_in  = bytes_read;

        dataleft      -= bytes_read;
        flush          = (dataleft) ? Z_NO_FLUSH : Z_FINISH;

//  - --- - --- - --- - --- -

        while(strm.avail_in) {

            uint have;

            strm.avail_out = ZJC_DAFPAGE;
            strm.next_out  = out;

            CALL_ZLIB (deflate (& strm, flush));

            have           = ZJC_DAFPAGE - strm.avail_out;
            (*size_d)     += have;

            fseek                           (dst->file, 0, SEEK_CUR           );
            BINWRITE                        (dst, bytes_read, uchar, have, out);
            fseek                           (dst->file, 0, SEEK_CUR           );

        };
    };

//  - --- - --- - --- - --- -

    deflateEnd (&strm);

    int finalsize = (int) (100.0 - (( (float) ( *size_d) / (float) (size_i) ) * 100));

#if KVR_DEBUG
    if(finalsize < 0) { CALOUT("BAD DEFLATE: %u/%u | approx. %i%% file size increase\n",
                              *size_d, size_i, -finalsize                                ); }

    else              { CALOUT("GOOD DEFLATE: %u/%u | approx. %i%% file size reduction\n",
                               *size_d, size_i,  finalsize                               ); };
#endif

    return DONE;                                                                            };

//  - --- - --- - --- - --- -