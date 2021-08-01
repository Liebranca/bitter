#include "kvr_mem.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

//   ---     ---     ---     ---     ---

#if KVR_DEBUG

    #define MEMCNT(act, m, op) {            /* a counter. goes up on MKMEM, down on DLMEM  */\
        USED_MEMORY op##=m->fsize;                                                           \
        char* um_unit_name="BYTES";                                                          \
        uint  um_unit_size=USED_MEMORY;                                                      \
        char* um_obju_name="BYTES";                                                          \
        uint  um_obju_size=m->fsize;                                                         \
                                                                                             \
                                                                                             \
        if  (USED_MEMORY > (1024*1024)) {                                                    \
            um_unit_size=um_unit_size/(1024*1024);                                           \
            um_unit_name="MB";                                                               \
        }                                                                                    \
                                                                                             \
        elif(USED_MEMORY > 1024       ) {                                                    \
            um_unit_size=um_unit_size/1024;                                                  \
            um_unit_name="KB";                                                               \
        };                                                                                   \
                                                                                             \
        if  (um_obju_size > (1024*1024)) {                                                   \
            um_unit_size=um_obju_size/(1024*1024);                                           \
            um_obju_name="MB";                                                               \
        }                                                                                    \
                                                                                             \
        elif(um_obju_size > 1024       ) {                                                   \
            um_obju_size=um_obju_size/1024;                                                  \
            um_obju_name="KB";                                                               \
        };                                                                                   \
                                                                                             \
        CALOUT                              ("%s %s: %u %s | %u %s USED\n\b",                \
                                             act, m->id.full,                                \
                                             um_obju_size, um_obju_name,                     \
                                             um_unit_size, um_unit_name                 );  }

#else

    #define MEMCNT(act, m, op) USED_MEMORY op##=m->fsize

#endif

static uint64_t USED_MEMORY = 0;            // total bytes used by MEM instances

//   ---     ---     ---     ---     ---

void* __kvrmalloc(uint size)                {
    void* buff=malloc(size); if(!buff) { return NULL; };
    memset(buff, 0, size); return buff;                                                     };

//   ---     ---     ---     ---     ---

MEM* MKMEM(uint size, ID* id)               { void* buff = __kvrmalloc(size);

    if(buff != NULL)
    {
        MEM* m=(MEM*) buff;

        m->id=*id;
        m->fsize=size; MEMCNT("ALLOC", m, +);

        return m;

    }; return NULL;                                                                         };

void DLMEM(void* p)                         {

    if(p) {
        MEM* m=(MEM*) p;
        MEMCNT("DEALLOC", m, -);
        free(p);

    }                                                                                       };

//   ---     ---     ---     ---     ---

void CLMEM(MEM* m)                          { memset(m->buff, 0, m->bsize);                 };

void CLMEM2(void* buff, uint size)          { memset(buff, 0, size);                        };

//   ---     ---     ---     ---     ---

void* NVMEM(MEM* m, int p)                  { uint ptr=0;

    if(p >= 0)                              // positive indexing (first element first)
    {
        if  ( p < m->bsize)                 { ptr = p;                                      }
        else                                { ptr = m->bsize - 1;                           };
    }

    else                                    // negative indexing (last element first)
    {
        if  (-p <= m->bsize)                { ptr = m->bsize + p;                           }
        else                                { ptr = 0;                                      };
    };

                                            // cast to char and return buff @ptr
    return                                  ((char*) m->buff) + ptr;                        };

//   ---     ---     ---     ---     ---

