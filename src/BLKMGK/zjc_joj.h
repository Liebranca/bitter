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

int  NTJOJENG (int mode               );    // joj encoder start-up
int  DLJOJENG (int mode               );    // joj encoder clean-up
int  ENCJOJ   (float* src, HRW* h     );    // read next image from pixdump and encode
int  DECJOJ   (                       );    // read next image from jojdump and decode
int  ZPJOJ    (HRW* h                 );    // zips encoded joj
int  UZPJOJ   (uint* sizes            );    // unzips joj

void STJOJDIM (uint dim               );    // uniform image dimentions for current file
void GTJOJLST (void                   );    // get n repr of last enc/dec fun

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ZJC_JOJ_H__