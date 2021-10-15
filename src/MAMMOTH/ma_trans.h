#ifndef __MA_TRANS_H__
#define __MA_TRANS_H__

#include "KVRNEL/zjc_CommonTypes.h"
#include "ma_boxes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

void TRHEXVAL(uchar*   src ,
              MEMUNIT* to  ,
              uint     size);               // hexstr to num

void TRBITVAL(uchar*   src ,
              MEMUNIT* to  ,
              uint     size);               // bitstr to num

void TRDECVAL(uchar*   src ,
              MEMUNIT* to  ,
              uint     size);               // decstr to num

void TRFLTVAL(uchar*   src ,
              MEMUNIT* to  ,
              uint     size);               // fltstr to float

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __MA_TRANS_H__