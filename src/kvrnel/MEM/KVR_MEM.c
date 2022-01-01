/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    KVR_MEM                               *
*                                           *
*     -memory workhorse                     *
*     -polies off to most things            *
*     -not really oop                       *
*                                           */
// *   ---     ---     ---     ---     ---  *
/*    LIBRE SOFTWARE                        *
*                                           *
*     Licenced under GNU GPL3               *
*     be a bro and inherit                  *
*                                           */
// *   ---     ---     ---     ---     ---  *
/* CONTRIBUTORS                             *
*     lyeb,                                 *
*                                           */
/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/

#include "kvr_mem.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

//   ---     ---     ---     ---     ---

static uint64_t USED_MEMORY  = 0;           // total bytes used by MEM instances

static uint     MEMCNT_KBSIZ = 1024;
static uint     MEMCNT_MBSIZ = 1024*1024;

static char*    MEMCNT_BYTES = "BYTES";
static char*    MEMCNT_KB    = "KB";
static char*    MEMCNT_MB    = "MB";

static char*    MEMCNT_ALLOC = "ALLOC";
static char*    MEMCNT_DLLOC = "DEALLOC";

static char     MEMCNT_NEXT  = ' ';
static uint     MEMCNT_I     = 0;
static uint     MEMCNT_J     = 0;
static uint     MEMCNT_H     = 0;

static char*    MEMCNT_ACT;
static char     MEMCNT_STR[74];

//   ---     ---     ---     ---     ---

#define MEMCNT_STSTR(x, length) {                                                            \
                                                                                             \
    MEMCNT_J = 0; do {                                                                       \
                                                                                             \
        MEMCNT_NEXT          = ((*x)!=0x00) ? (*x) : 0x20;                                   \
        MEMCNT_STR[MEMCNT_I] = MEMCNT_NEXT;                                                  \
        MEMCNT_I++; MEMCNT_J++;                                                              \
        if(MEMCNT_NEXT!=0x20) { *x++; }                                                      \
                                                                                             \
    } while(MEMCNT_J < length);                                                              \
                                                                                             \
    MEMCNT_STR[MEMCNT_I]=0x20; MEMCNT_I++;                                                  }

//   ---     ---     ---     ---     ---

void MEMCNT(MEM* m, char act)               {

    MEMCNT_NEXT = ' ';
    MEMCNT_I    = 0;

    char  snum[8];
    char* snump = snum+0;
    char* idful = m->id.full;

    if(act==0x41) {                         // on alloc
        MEMCNT_ACT=MEMCNT_ALLOC;
        USED_MEMORY+=m->fsize;

    }

    else {                                  // on dealloc
        MEMCNT_ACT=MEMCNT_DLLOC;
        USED_MEMORY-=m->fsize;

    };

    MEMCNT_STSTR                            (MEMCNT_ACT, 7);
    MEMCNT_STSTR                            (idful, 23    );

//   ---     ---     ---     ---     ---

    char* um_obju_name=MEMCNT_BYTES;
    uint  um_obju_size=m->fsize;

    if  (um_obju_size > MEMCNT_MBSIZ) {
        um_obju_size=um_obju_size/MEMCNT_MBSIZ;
        um_obju_name=MEMCNT_MB;

    }

    elif(um_obju_size > MEMCNT_KBSIZ) {
        um_obju_size=um_obju_size/MEMCNT_KBSIZ;
        um_obju_name=MEMCNT_KB;

    };

    __writoa                                (um_obju_size, snum, 10);
    MEMCNT_STSTR                            (snump, 4              );
    MEMCNT_STSTR                            (um_obju_name, 5       );

//   ---     ---     ---     ---     ---

    MEMCNT_STR[MEMCNT_I]='|'; MEMCNT_I++;
    MEMCNT_STR[MEMCNT_I]=' '; MEMCNT_I++;

    char* um_unit_name=MEMCNT_BYTES;
    uint  um_unit_size=USED_MEMORY;

    if  (USED_MEMORY > MEMCNT_MBSIZ) {
        um_unit_size=um_unit_size/MEMCNT_MBSIZ;
        um_unit_name=MEMCNT_MB;

    }

    elif(USED_MEMORY > MEMCNT_KBSIZ) {
        um_unit_size=um_unit_size/MEMCNT_KBSIZ;
        um_unit_name=MEMCNT_KB;

    };

    snum[0] = '\0';
    snump   = snum+0;

    __writoa                                (um_unit_size, snum, 10);
    MEMCNT_STSTR                            (snump, 4              );
    MEMCNT_STSTR                            (um_unit_name, 5       );

//   ---     ---     ---     ---     ---

    MEMCNT_STR[MEMCNT_I]='\0'; MEMCNT_I++;

    CALOUT                                  (M, "0x%" PRIXPTR  " %s USED\n\b"  ,
                                                (uintptr_t) m, MEMCNT_STR      );           };

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
        m->fsize=size;
#if KVR_DEBUG & KVR_CALOM
        MEMCNT(m, 'A');
#endif

        return m;

    }; return NULL;                                                                         };

void DLMEM(void* p)                         {

    if(p) {
        MEM* m=(MEM*) p;
#if KVR_DEBUG & KVR_CALOM
        MEMCNT(m, 'D');
#endif
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

