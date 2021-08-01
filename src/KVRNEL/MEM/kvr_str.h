// *   ---     ---     ---     ---     ---  *
// *  KVR_STR                               *
// *                                        *
// *  -plain mem use                        *
// *  -some funcs for char arrays           *
// *  -can poly, but funcs are byte-base    *
// *                                        *
// *   ---     ---     ---     ---     ---  *

#ifndef __KVR_STR_H__
#define __KVR_STR_H__

#include "kvr_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

MEM* MKSTR (char* buff, uint  ex_alloc,
            uint disc_buff_len        );
MEM* GWSTR (MEM*  str,  uint  mag     );
void ADSTR (MEM** str,  char* tail    );
MEM* PLSTR (MEM*  str,  char* tail    );
void STSTR (MEM** str,  char* buff    );
void RPSTR (MEM** str,  char* tail,   \
            uint  offset              );

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __KVR_STR_H__