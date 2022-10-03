#ifndef __SH_LINE_H__
#define __SH_LINE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sh_gbl.h"

//   ---     ---     ---     ---     ---

typedef struct {

  int32_t x;                // h-position
  int32_t y;                // v-position

  int32_t us;               // space utilized
  int32_t sz;               // total space

} SH_LINE;

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
};
#endif

#endif // __SH_LINE_H__