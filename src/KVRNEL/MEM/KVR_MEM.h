#ifndef __KVR_MEM_H__
#define __KVR_MEM_H__

#include "KVR_EVIL.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct KVR_MEM {                    // generic holding some dynamic memory address

    void* buff;                             // malloc'd memory location
    stark free;                             // pointer to destructor

    int   count;                            // element count
    int   size;                             // element size
    int   ptr;                              // idex into current element

    int   pad;                              // manual padding

} MEM;

//   ---     ---     ---     ---     ---

int   MKMEM(MEM* m       );                 // allocates a MEM block
void  DLMEM(MEM* m       );                 // frees a MEM block
void* NVMEM(MEM* m, int p);                 // get ptr to MEM @offset

void  FCMEM(MEM* m       );                 // force valid block

//   ---     ---     ---     ---     ---

#define MEMBUFF(m, type, offset)            /* get type-casted ptr to MEM @offset          */\
    ((type*) (NVMEM(m, offset)))

#define MEMGET(m, buff, type)               {                                                \
                                                                                             \
    FCMEM(m);                                 /* ensure block is valid                     */\
                                                                                             \
    static char sizestr[33]; int retx = 0;    /* requested size to char array              */\
    __writoa(m->size * m->count, sizestr, 10);                                               \
                                                                                             \
    CALL(MKMEM(m), retx, 0x00, sizestr);      /* catch malloc fail                         */\
                                                                                             \
    buff = MEMBUFF(m, type, 0);               /* assign address to buff                    */}

#define MEMFREE(m, buff)                      /* just a macro for prettycode's sake        */\
    DLMEM(m); buff = NULL

//   ---     ---     ---     ---     ---


#ifdef __cplusplus
}
#endif

#endif // __KVR_MEM_H__