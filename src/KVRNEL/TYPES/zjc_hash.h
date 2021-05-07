#ifndef __ZJC_HASH_H__
#define __ZJC_HASH_H__

#include "zjc_stack.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct ZJC_HASHSLOT {

    void** nodes;                           // addresses to data

    STK*   stack;                           // available indices
    LKUP*  keycache;                        // stored keys for faster access

} HSLOT;

//   ---     ---     ---     ---     ---

typedef struct ZJC_HASHTABLE {

    MEM    m;                               // mem header

    uint   nslots;                          // number of slots; table bounds
    uint   nitems;                          // number of inserted items
    uint   jmp;                             // space between table entries, in bytes

} HASH;

//   ---     ---     ---     ---     ---

void MKHASH(HASH* h, uint mag, char* id);   // build a new hash table
void DLHASH(void* buff);                    // free a hash table

void STHASH(void* data);                    // insert data into hash @key
void GTHASH(void** to, int pop);            // retrieve data @key

int  INHASH(HASH* h, LKUP* key);            // return key is in table
int  NK4HSLOT(void);                        // pop slot idex from subarray

//   ---     ---     ---     ---     ---

#define HASHSET(h, lkp, data)               { int key_in_hash=INHASH(h, lkp); int retx=0;    \
    if(!key_in_hash) { ERRCATCH(NK4HSLOT(), retx, 72, lkp->key); h->nitems++; };             \
    if(!retx       ) { STHASH(data);                                     };                 }

#define HASHGET(h, lkp, to, type, pop)        { int key_in_hash=INHASH(h, lkp);             \
    if(!key_in_hash) { ERRCATCH(ERROR, key_in_hash, 73, lkp->key); }                        \
    else             { GTHASH((void**) &to, pop); h->nitems-=pop;; }                        }

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ZJC_HASH_H__