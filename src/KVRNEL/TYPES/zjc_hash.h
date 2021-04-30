#ifndef __ZJC_HASH_H__
#define __ZJC_HASH_H__

#include "zjc_stack.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct ZJC_HASHNODE {

    void* data;                             // ptr to block, typecasted by table get
    char* key;                              // key that generated the table index
    uint  idex;                             // idex into subarray

    struct ZJC_HASHNODE* next;              // jump-to

} HNODE;

typedef struct ZJC_HASHSLOT {

    MEM    m;                               // mem header

    HNODE* head;                            // ptr to first block

    STK    stack;                           // available indices
    HNODE* nodes;                           // array of key:data pairs

} HSLOT;

//   ---     ---     ---     ---     ---

typedef struct ZJC_HASHTABLE {

    MEM    m;                               // mem header

    uint   nitems;                          // number of inserted items
    HSLOT* table;                           // subarrays containing pseudo linked lists

} HASH;

//   ---     ---     ---     ---     ---

void MKHASH(HASH* h, uint size, char* id);  // build a new hash table
void DLHASH(void* buff);                    // free a hash table

void ADHASH(HASH* h, char* key, void* data);// insert key:data into hash
void* RMHASH(HASH* h, char* key);           // remove key:data from hash, return data

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ZJC_HASH_H__