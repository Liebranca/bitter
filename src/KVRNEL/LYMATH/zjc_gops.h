#ifndef __ZJC_GOPS_H__
#define __ZJC_GOPS_H__

#include "KVRNEL/zjc_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

uint mxui  (uint a, uint b);                // return bigger
uint mnui  (uint a, uint b);                // return smaller
uint sbmxui(uint a, uint b);                // sub bigger from smaller
uint sbmnui(uint a, uint b);                // sub smaller from bigger

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ZJC_GOPS_H__