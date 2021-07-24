#include "kvr_zwrap.h"
#include "ZLIB/zlib.h"

#include <string.h>
#include <stdio.h>

//  - --- - --- - --- - --- -

#define windowBits 15
#define GZIP_ENCODING 16
#define ENABLE_ZLIB_GZIP 32

#define CHUNK 0x4000

#define CALL_ZLIB(x) {                                                      \
        int status;                                                         \
        status = x;                                                         \
        if (status < 0) {                                                   \
            fprintf       (stderr,                                          \
                           "%s:%d: %s returned a bad status of %d.\n",      \
                           __FILE__, __LINE__, #x, status);                 \
            __terminator(0, "Bruh, zLib went PLOP!");                       \
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
            uint size_i, uint size_d)       {

    uchar    in [CHUNK];
    uchar    out[CHUNK];

    uint     readsize = CHUNK;
    uint     dataleft = size_d;

    z_stream strm;
    infstrm_init(&strm);

    rewind(src->file); rewind(dst->file);

    strm.next_in      = in;
    strm.avail_in     = 0;

    int  bytes_read;

//  - --- - --- - --- - --- -

    while(dataleft) {

                                            // read next block to decompress
        if(readsize > dataleft)             { readsize = dataleft;                };
        fseek                               (src->file, 0, SEEK_CUR               );
        BINREAD                             (src, bytes_read, uchar, readsize, in );
        fseek                               (src->file, 0, SEEK_CUR               );

        dataleft      -= readsize;
        strm.avail_in  = bytes_read;
        strm.next_in   = in;

//  - --- - --- - --- - --- -

        while(strm.avail_out == 0) {        // decompress block and write to dst

            uint have;

            strm.avail_out = CHUNK;
            strm.next_out  = out;
            CALL_ZLIB(inflate(&strm, Z_NO_FLUSH));

            have = CHUNK - strm.avail_out;

            fseek                           (dst->file, 0, SEEK_CUR           );
            BINWRITE                        (dst, bytes_read, uchar, have, out);
            fseek                           (dst->file, 0, SEEK_CUR           );

        };
    };

//  - --- - --- - --- - --- -

    inflateEnd (& strm);

    return 0;                                                                               };

//  - --- - --- - --- - --- -

int DEFLBIN(BIN* src,    BIN* dst,
            uint size_i, uint* size_d)      {

    uchar    in [CHUNK];
    uchar    out[CHUNK];

    z_stream strm;

    defstrm_init(&strm);
    rewind(src->file); rewind(dst->file);

    uint dataleft = size_i;

    int  bytes_read;
    strm.avail_out=0;

//  - --- - --- - --- - --- -

    while(dataleft) {

        uint readsize = CHUNK; if(readsize > dataleft) { readsize = dataleft; }

                                            // read next block to compress
        fseek                               (src->file, 0, SEEK_CUR              );
        BINREAD                             (src, bytes_read, uchar, readsize, in);
        fseek                               (src->file, 0, SEEK_CUR              );

        strm.next_in   = in;
        strm.avail_in  = readsize;

        dataleft -= readsize;

//  - --- - --- - --- - --- -

        while (strm.avail_out == 0) {       // compress block and write to dst

            CALOUT("FUCK", bytes_read);

            uint have;
            strm.avail_out = CHUNK;
            strm.next_out  = out;

            CALL_ZLIB (deflate (& strm, Z_FINISH));

            have           = CHUNK - strm.avail_out;
            (*size_d)     += have;

            CALOUT("DUCK %i\n\b", bytes_read);

            fseek                           (dst->file, 0, SEEK_CUR           );
            BINWRITE                        (dst, bytes_read, uchar, have, out);
            fseek                           (dst->file, 0, SEEK_CUR           );

        };
    };

//  - --- - --- - --- - --- -

    deflateEnd (&strm);

    int finalsize = (int) (100.0 - (( (float) ( *size_d) / (float) (size_i) ) * 100));

    if(finalsize < 0) { CALOUT("BAD DEFLATE: %u/%u | approx. %i%% file size increase\n",
                              *size_d, size_i, -finalsize                                ); }

    else              { CALOUT("GOOD DEFLATE: %u/%u | approx. %i%% file size reduction\n",
                               *size_d, size_i,  finalsize                               ); };

    return 0;                                                                               };

//  - --- - --- - --- - --- -