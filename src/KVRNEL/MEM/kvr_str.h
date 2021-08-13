#ifndef __KVR_STR_H__
#define __KVR_STR_H__

#include "kvr_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

MEM* MKSTR (char* buff, uint  ex_alloc,
            uint disc_buff_len        );    // alloc new byte array
MEM* GWSTR (MEM*  str,  uint  mag     );    // expand array
void ADSTR (MEM** str,  char* tail    );    // add substring at end of array
MEM* PLSTR (MEM*  str,  char* tail    );    // make copy of array, then add tail
void STSTR (MEM** str,  char* buff    );    // set contents of array
void RPSTR (MEM** str,  char* tail,   \     // set contents of array @offset
            uint  offset              );

void LDLNG (uint size                 );    // set lng to width size
MEM* GTLNG (void                      );    // get addr of lng

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __KVR_STR_H__