#ifndef __ZJC_MEMLNG__
#define __ZJC_MEMLNG__

#include "../zjc_CommonTypes.h"
#include "kvr_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADLINE_WIDTH                 \
    xELEMS(ZJC_DAFSIZE, ID)/sizeof(ID)

#define ADLINE_HEIGHT 4


//   ---     ---     ---     ---     ---

typedef struct ZJC_MEMORY_STICK {           // managed big mem

    MEM m;                                  // mem header
    ID  pool[ADLINE_WIDTH*ADLINE_HEIGHT];

} MEMLNG;

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ZJC_MEMLNG__