#ifndef __KVR_INTPRT_H__
#define __KVR_INTPRT_H__

#include "../MEM/KVR_EVIL.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

void SETINBUFF(char* src);                  // for testing
void SETINCOM(COM* c);

void GTIN();                                // console input
void TKIN();                                // lexer

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif //__KVR_INTPRT_H__
