// *   ---     ---     ---     ---     ---  *
// *  ZJC_JOJ                               *
// *                                        *
// *  -basic bin derivate                   *
// *  -holds image data in joj format       *
// *  -conversions defined at zjc_bintypes  *
// *  -gzipped to the max                   *
// *                                        *
// *   ---     ---     ---     ---     ---  *

#ifndef __ZJC_JOJ_H__
#define __ZJC_JOJ_H__

#include "../KVRNEL/MEM/kvr_bin.h"
#include "../KVRNEL/TYPES/zjc_BinTypes.h"

#define JOJ_ENCODE 0
#define JOJ_DECODE 1

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct JOJH {                       // header for joj files

    char name[KVR_IDK_WIDTH];               // id of this image

    uint dim;                               // dimensions
    uint fracl;                             // color compression level

} JOJH;

//   ---     ---     ---     ---     ---

BIN*    MKJOJ    (char* path, uint mode  ); // inits a joj file
JOJPIX* GTJOJ    (BIN* joj               ); // get start of joj buffer

int     NTJOJENG (int mode               ); // joj encoder start-up
int     DLJOJENG (int mode               ); // joj encoder clean-up

int     ENCJOJ   (uint  dim, uint* size_i); // read next image from pixdump and encode
int     DECJOJ   (JOJH* jojh             ); // read next image from jojdump and decode

int     ZPJOJ   (uint size_i, uint count,\
                 JOJH* jojh              ); // zips encoded joj

int     UZPJOJ  (JOJH* jojh, uint* sizes ); // unzips joj

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ZJC_JOJ_H__