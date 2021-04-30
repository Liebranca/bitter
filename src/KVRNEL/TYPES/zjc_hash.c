#include "zjc_hash.h"
#include <math.h>

#define REHASH_STEP 8

//   ---     ---     ---     ---     ---

void MKHASH(HASH* h, uint size, char* id)   {

    //h->m       = {0};
    h->m.id    = id;

    h->m.size  = sizeof(HSLOT);
    h->m.count = size;
    h->m.free  = &DLHASH;

    MEMGET((&(h->m)), h->table, HSLOT);
    h->nitems=0; HSLOT* slot=NULL;

    for(uint x=0; x<size; x++)
    {

        slot=(h->table)+x;

        //slot->m       = {0};
        slot->m.id    = "HASHSLOT";
        slot->m.size  = sizeof(HNODE);
        slot->m.count = REHASH_STEP;

        MEMGET(byref(slot->m), slot->nodes, HNODE);
        MKSTK (byref(slot->stack), REHASH_STEP, "HASHSSTK");
        for(uint y=slot->m.count-1; y>-1; y--) { STACKPUSH(byref(slot->stack), y); }

        slot->head=slot->nodes+0;
        slot->head->next=NULL;

    };                                                                                      };

void DLHASH(void* buff)                     {

    HASH* h = (HASH*) buff;

    for(HSLOT* slot=h->table+0; slot!=h->table+h->m.count; slot++)
    {
        MEMFREE(byref(slot->stack.m), NULL);
        MEMFREE(byref(slot->m),       NULL);

    };                                                                                      };

//   ---     ---     ---     ---     ---

uint HASHIT(HASH* h, char* k)               {

    uint idex = 0;

    uint x    = 0;
    uint y    = 0;

    do {

        x=((int) *k)+1; x*=x;
        idex+=x+y; y+=1;

    } while(*k++);

    return x%(h->m.count);                                                                  };

//   ---     ---     ---     ---     ---

void ADHASH(HASH* h, char* key, void* data) {

    uint   idex    = HASHIT(h, key);
    uint   subidex = 0;

    HSLOT* slot    = h->table+idex;
    STACKPOP(byref(slot->stack), subidex);

    HNODE* node    = slot->nodes+subidex;
    node->data     = data;
    node->key      = key;
    node->idex     = subidex;

    HNODE* tail    = slot->head;
    if(!tail) { slot->head=node; return; };
    while(tail->next) { tail=tail->next; };

    tail->next     = node;                                                                  };

void* RMHASH(HASH* h, char* key)            {

    uint   idex    = HASHIT(h, key);
    uint   subidex = 0;
    uint   found   = 0;

    HSLOT* slot    = h->table+idex;
    HNODE* tail    = slot->head;

    HNODE* prev    = NULL;

    if(!tail) { return NULL; }

    do {

        if(!__wrstrcmp(tail->key, key)) { found=1; break; };
        prev=tail; tail=tail->next;

    } while(tail->next);

    if(found)
    {
        STACKPUSH(byref(slot->stack), tail->idex);
        if(prev) { prev->next=NULL; }

        return tail->data;

    }; return NULL;                                                                         };

//   ---     ---     ---     ---     ---