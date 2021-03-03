#ifndef __KVR_KLS_H__
#define __KVR_COM_H__

#include "KVR_EVIL.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct KVR_KLS {                    // a basic class descriptor

    const char* id;                         // unique id for this class

    stark       mkr;                        // pointer to constructor
    stark       ukr;                        // pointer to destructor

    int         size;                       // size in bytes

} KLS;

extern KLS KVR_CLASSES[64];

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif //__KVR_KLS_H__