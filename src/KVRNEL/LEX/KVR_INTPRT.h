// *   ---     ---     ---     ---     ---  *
// *  KVR_INTPRT                            *
// *                                        *
// *  -read user coms                       *
// *  -turns peso into c                    *
// *                                        *
// *   ---     ---     ---     ---     ---  *

#ifndef __KVR_INTPRT_H__
#define __KVR_INTPRT_H__

#include "../zjc_evil.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

void PESONIT();                             // fire up the PE$O translator
void PESOEND();                             // cleanup

void SETINBUFF(char* src);                  // for testing
void SETINCOM(COM* c);

void GTIN();                                // console input
void TKIN();                                // lexer

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif //__KVR_INTPRT_H__
