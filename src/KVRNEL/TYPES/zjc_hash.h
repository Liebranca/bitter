#ifndef __ZJC_HASH_H__
#define __ZJC_HASH_H__

#include "zjc_stack.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

#define ZJC_HASH_STKLEN 8

typedef struct ZJC_HASHSLOT {

    LKUP  keycache[ZJC_HASH_STKLEN];        // stored keys for faster access
    void* nodes   [ZJC_HASH_STKLEN];        // addresses to data

    STK   stack;                            // available indices

} HSLOT;

//   ---     ---     ---     ---     ---

typedef struct ZJC_HASHTABLE {

    MEM  m;                                 // mem header

    uint nslots;                            // number of slots; table bounds
    uint nitems;                            // number of inserted items

} HASH;

//   ---     ---     ---     ---     ---

HASH* MKHASH(uint mag, char* id);           // build a new hash table
void DLHASH(void* buff);                    // free a hash table
void CLHASH(HASH* h);                       // wipe a hash clean

void STHASH(void* data);                    // insert data into hash @key
void GTHASH(void** to, int pop);            // retrieve data @key

int  INHASH(HASH* h, LKUP* key);            // return key is in table
int  NK4HSLOT(void);                        // pop slot idex from subarray

void FLHASH(int flags);                     // sets flags, used for internals

//   ---     ---     ---     ---     ---

#define HASHSET(h, lkp, data)               { FLHASH(1); /* set mode to insertion */         \
    int key_in_hash=INHASH(h, lkp); int retx=0;                                              \
    if(!key_in_hash) { ERRCATCH(NK4HSLOT(), retx, 72, lkp->key); h->nitems++; };             \
    if(!retx       ) { STHASH(data);                                     };                 }

#define HASHGET(h, lkp, to, pop)            { FLHASH(0); /* set mode to fetch     */         \
    int key_in_hash=INHASH(h, lkp);                                                          \
    if(!key_in_hash) { ERRCATCH(ERROR, key_in_hash, 73, lkp->key); to=NULL; }                \
    else             { GTHASH((void**) &to, pop); h->nitems-=pop;; }                        }

#define STR_HASHSET(h, key, data)           { FLHASH(1); /* set mode to insertion */         \
    LKUP lkp={ key, -1, -2 }; int key_in_hash=INHASH(h, &lkp); int retx=0;                   \
    if(!key_in_hash) { ERRCATCH(NK4HSLOT(), retx, 72,    key); h->nitems++; };               \
    if(!retx       ) { STHASH(data);                                        };              }

#define STR_HASHGET(h, key, to, pop)        { FLHASH(0); /* set mode to fetch     */         \
    LKUP lkp={ key, -1, -2 }; int key_in_hash=INHASH(h, &lkp);                               \
    if(!key_in_hash) { ERRCATCH(ERROR, key_in_hash, 73,  key); to=NULL; }                    \
    else             { GTHASH((void**) &to, pop); h->nitems-=pop;; }                        }

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ZJC_HASH_H__