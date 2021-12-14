#ifndef __KVR_STR_H__
#define __KVR_STR_H__

#include "kvr_mem.h"

#ifdef __cplusplus
extern"C" {


#endif

//   ---     ---     ---     ---     ---
// #:0x0;>

// alloc new byte array
MEM* MKSTR(
  const char* buff,
  uint ex_alloc,
  uint disc_buff_len

);

// expand array
MEM* GWSTR(MEM* str,uint len);

// add substring at end of array
void CTSTR(MEM** str,const char* tail);

// make copy of array, then add tail
MEM* CCTSTR(MEM* str,const char* tail);

// set contents of array
void STSTR(MEM** str,const char* buff);

// get contents of mem as a char array
extern inline char* GTSTR(MEM* str);

// set contents of array @offset
void RPSTR(
  MEM** str,
  const char* tail,
  uint offset

);


void LDLNG(uint size);      // set lng to width size
MEM* GTLNG(void);           // get addr of lng

//   ---     ---     ---     ---     ---
// #:0x1;>


#ifdef __cplusplus
}
#endif

#endif // __KVR_STR_H
