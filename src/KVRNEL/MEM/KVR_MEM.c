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

#define MEMSET(m, op)                       /* a counter. goes up on MKMEM, down on DLMEM  */\
    USED_MEMORY op##= (m->size * m->count)                                                   \
                    + sizeof(MEM); CALOUT("%u KB USED\n\b", (USED_MEMORY/1024))

static uint64_t USED_MEMORY = 0;            // total bytes used by MEM instances

//   ---     ---     ---     ---     ---

void* __kvrmalloc(int count, int size)      {

    void* buff = malloc                     ( count * size          );
    if(!buff)                               { return NULL;          };

    memset                                  ( buff, 0, count * size );

    return buff;                                                                            };

//   ---     ---     ---     ---     ---

int MKMEM(MEM* m)                           { if(m->buff) { DLMEM(m, NULL); } // dont leak

    m->buff = __kvrmalloc(m->count, m->size);
    if(m->buff != NULL)                     { MEMSET (m, +   ); return 0;                   };
    return FATAL;                                                                           };

void DLMEM(MEM* m, void* buff)              {

    if(buff    != NULL && m->free)          { m->free(buff);                                };
    if(m->buff != NULL           )          { free(m->buff); m->buff = NULL; MEMSET(m, -);  };
                                                                                            };

void FCMEM(MEM* m)                          {

    if(!m->size ) { m->size  = 1;          };
    if(!m->count) { m->count = 1;          };                                               };

//   ---     ---     ---     ---     ---

void* NVMEM(MEM* m, int p)                  {

    int ptr=0;


    if(p >= 0)                              // positive indexing (first element first)
    {
        if  ( p <  m->count)                { ptr = p;                                      }
        else                                { ptr = m->count - 1;                           };
    }
    else                                    // negative indexing (last element first)
    {
        if  (-p <= m->count)                { ptr = m->count + p;                           }
        else                                { ptr = 0;                                      };
    };

                                            // cast to char and return buff @ptr
    return                                  ((char*) m->buff) + (ptr * m->size);            };

//   ---     ---     ---     ---     ---

