// *   ---     ---     ---     ---     ---  *
// *  KVR_MEM                               *
// *                                        *
// *  -memory workhorse                     *
// *  -alloc, dealloc                       *
// *  -is base struct to most things        *
// *  -does cool funky poly                 *
// *                                        *
// *   ---     ---     ---     ---     ---  *

#ifndef __KVR_MEM_H__
#define __KVR_MEM_H__

#include "../zjc_evil.h"
#include "../TYPES/zjc_id.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct KVR_MEM {                    // generic holding dynamic memory

    ID    id;                               // universal block header

    uint  fsize;                            // total space used
    uint  bsize;                            // usable, non-header space

    void* buff;                             // start of non-header memory

} MEM; CASSERT                              ( sizeof(MEM)==      \

                                                (sizeof(ID   )   )
                                            +   (sizeof(uint )* 2)
                                            +   (sizeof(void*)   ),

                                            "Bad MEM size"       );

//   ---     ---     ---     ---     ---

MEM*  MKMEM(uint size, ID* id    );         // create a new MEM block
void  DLMEM(void* p              );         // frees a MEM block
void* NVMEM(MEM* m, int p        );         // get ptr to MEM @offset
void  CLMEM(MEM* m               );         // flood a block with zeroes

//   ---     ---     ---     ---     ---

#define MEMBUFF(m, type, offset)              /* get type-casted ptr to MEM @offset        */\
    ((type*) (NVMEM(m, offset)))

                                              /* checks for allocation failure             */
#define MEMGET(type, to, size, id)          { uint mem_reqsize=(size)+sizeof(type);          \
                                                                                             \
                                                                                             \
    static char sizestr[33]; int retx = 0;    /* requested size to char array              */\
    __writoa(mem_reqsize, sizestr, 10);                                                      \
                                                                                             \
    MEM* new_mem=MKMEM(mem_reqsize, id); if(!new_mem)                                        \
    { ERRCATCH(FATAL, retx, 0x00, sizestr); } /* catch malloc fail                         */\
                                                                                             \
    new_mem->buff=((char*) new_mem) + sizeof(type);                                          \
    new_mem->bsize=new_mem->fsize-sizeof(type);                                              \
    to=(type*) new_mem;                                                                     }

//   ---     ---     ---     ---     ---


#ifdef __cplusplus
}
#endif

#endif // __KVR_MEM_H__