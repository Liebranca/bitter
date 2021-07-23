/*   ---     ---     ---     ---     ---    *
 *  KVR_MEM                                 *
 *                                          *
 *  main memory workhorse for KVR           *
 *  handles allocation and deallocation     *
 *  defines behaviour of base MEM struct    *
 *                                          *
 *   ---     ---     ---     ---     ---    */

#include "kvr_mem.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

//   ---     ---     ---     ---     ---

#define MEMCNT(size, op)                    /* a counter. goes up on MKMEM, down on DLMEM  */\
    USED_MEMORY op##=size; CALOUT("%u KB USED\n\b", USED_MEMORY/1024)

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
        m->fsize=size; MEMCNT(size, +);

        return m;

    }; return NULL;                                                                         };

void DLMEM(void* p)                         {

    if(p) {
        MEM* m=(MEM*) p;
        MEMCNT(m->fsize, -);
        free(p);

    }                                                                                       };

void CLMEM(MEM* m)                          { memset(m->buff, 0, m->bsize);                 };

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

