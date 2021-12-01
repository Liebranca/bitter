
#ifndef __ZJC_GOPS_H__
#define __ZJC_GOPS_H__

#include "KVRNEL/zjc_CommonTypes.h"

#ifdef __cplusplus
extern"C" {

#endif

//   ---     ---     ---     ---     ---
// #:0x0;>


uint mxui(uint a,uint b);   // return bigger
uint mnui(uint a,uint b);   // return smaller
uint sbmxui(uint a,uint b); // sub bigger from smaller
uint sbmnui(uint a,uint b); // sub smaller from bigger


// clamp x to {mn, mx}
uint clampui(uint x,uint mn,uint mx);

// get 0xRRGGBBAA element mapped to 0-1f range
float pal_utof(uint x,uint off);

//   ---     ---     ---     ---     ---
// #:0x1;>


#ifdef __cplusplus
}
#endif

#endif                      // __ZJC_GOPS_H
