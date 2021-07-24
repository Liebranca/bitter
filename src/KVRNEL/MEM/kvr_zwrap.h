#ifndef __KVR_ZWRAP_H__
#define __KVR_ZWRAP_H__

#include "kvr_bin.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

int INFLBIN(BIN* src, BIN* dst, uint size_i, uint  size_d);
int DEFLBIN(BIN* src, BIN* dst, uint size_i, uint* size_d);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ZJC_ZWRAP_H__