#ifndef __KVR_MEM_H__
#define __KVR_MEM_H__

#include "../zjc_evil.h"
#include "../zjc_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct KVR_MEM {                    // generic holding some dynamic memory address

    char* id;                               // name for this block

    void* buff;                             // malloc'd memory location
    STARK free;                             // pointer to a free func

    int   count;                            // element count
    int   size;                             // element size

} MEM;

//   ---     ---     ---     ---     ---

int   MKMEM(MEM* m       );                 // allocates buffer of a MEM block
void  DLMEM(MEM* m, void* buff );           // frees a MEM block
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
    ERRCATCH(MKMEM(m), retx, 0x00, sizestr);  /* catch malloc fail                         */\
    buff = MEMBUFF(m, type, 0);               /* set buff to first element in array        */}

#define MEMFREE(m, buff)                      /* just a macro for prettycode's sake        */\
    DLMEM(m, buff);

//   ---     ---     ---     ---     ---


#ifdef __cplusplus
}
#endif

#endif // __KVR_MEM_H__