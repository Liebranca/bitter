#ifndef __KVR_COM_H__
#define __KVR_COM_H__

#include "KVR_EVIL.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct KVR_COM {                    // a command

    int  ins;                               // numerical instruction idex
    char val[8][64];                        // value array, alike to arguments

} COM;

typedef struct KVR_RET {                    // a command's return

    int   state;                            // return state of the command
    void* ob;                               // handle to object or NULL for destructors

} RET;

//   ---     ---     ---     ---     ---

RET RDCOM(COM* c);                          // reads and execs command

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif //__KVR_COM_H__