#ifndef __KVR_INTPRT_H__
#define __KVR_INTPRT_H__

#include "../MEM/KVR_EVIL.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

void GTIN(COM* com);                        // console input
void TKIN(COM* com, char* buff);            // lexer
RET  RDIN(COM* com            );            // parser

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif //__KVR_INTPRT_H__
