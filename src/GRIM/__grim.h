#ifndef __GRIM_H__
#define __GRIM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

//   ---     ---     ---     ---     ---

void fr_rmode(uint64_t in);
void fr_wmode(uint64_t in);
void fr_cmode(uint64_t in);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __GRIM_H__