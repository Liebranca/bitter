#ifndef __ZJC_JOJ_H__
#define __ZJC_JOJ_H__

#include "../KVRNEL/MEM/kvr_bin.h"
#include "../KVRNEL/TYPES/zjc_BinTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

BIN*    MKJOJ    (char* path, uint mode  ); // inits a joj file
JOJPIX* GTJOJ    (BIN* joj               ); // get start of joj buffer

int     NTJOJENG (void                   ); // joj encoder start-up
int     DLJOJENG (void                   ); // joj encoder clean-up

int     ENCJOJ   (uint  dim, uint* size_i); // read next image from pixdump and encode
//void    DECJOJ   (BIN* src, float* pixels); // read next image from jojdump and decode

int     ZPJOJ   (uint size_i            );  // zips encoded joj

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ZJC_JOJ_H__