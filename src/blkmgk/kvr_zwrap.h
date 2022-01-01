#ifndef __KVR_ZWRAP_H__
#define __KVR_ZWRAP_H__

#include "../KVRNEL/MEM/kvr_bin.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

void NTDEFL(void);                           // kiknit compress
void NTINFL(void);                           // kiknit decompress
void DLDEFL(void);                           // kikdel compress
void DLINFL(void);                           // kikdel decompress

//   ---     ---     ---     ---     ---

int INFLBUF(BIN* src, void* dst      ,
            uint size_i, uint  size_d,
            uint offs_i, uint offs_d );     // decompress bin to buff

int INFLBIN(BIN* src, BIN* dst       ,
            uint size_i, uint  size_d,
            uint offs_i, uint offs_d );     // decompress bin to bin

int DEFLBUF(void* src, BIN* dst      ,
            uint size_i, uint* size_d,
            uint offs_i, uint offs_d );     // compress buf to bin

int DEFLBIN(BIN* src, BIN* dst       ,
            uint size_i, uint* size_d,
            uint offs_i, uint offs_d );     // compress bin to bin

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __KVR_ZWRAP_H__