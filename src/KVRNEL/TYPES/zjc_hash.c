#include "zjc_hash.h"
#include <math.h>
#include <stdio.h>

//   ---     ---     ---     ---     ---

#define REHASH_STEP 8

static uint   ZJC_LIDX_HASH  = 0;
static char*  ZJC_LKEY_HASH  = NULL;

static HSLOT* ZJC_CURR_HSLOT = NULL;
static HNODE* ZJC_CURR_HNODE = NULL;
static HNODE* ZJC_PREV_HNODE = NULL;

//   ---     ---     ---     ---     ---

void MKHASH(HASH* h, uint size, char* id)   {

    h->m       = (MEM){0};

    h->m.id    = id;
    h->m.size  = sizeof(HSLOT);
    h->m.count = size;
    h->m.free  = &DLHASH;

    MEMGET((&(h->m)), h->table, HSLOT);
    h->nitems=0; HSLOT* slot=NULL;

    for(uint x=0; x<size; x++)
    {

        slot=(h->table)+x;

        slot->m.id    = "HASHSLOT";
        slot->m.size  = sizeof(HNODE);
        slot->m.count = REHASH_STEP;

        MEMGET(byref(slot->m), slot->nodes, HNODE);

        MKSTK (byref(slot->stack), REHASH_STEP, "HASHSSTK");
        for(uint y=slot->m.count; y>0; y--)
        { STACKPUSH(byref(slot->stack), y-1); }

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

void HASHIT(HASH* h, char* k)               {

    ZJC_LKEY_HASH = k;
    uint idex     = 0;

    uint x        = 0;
    uint y        = 0;

    do {

        x=((uint) *k)+1; x*=x;
        idex+=x+y; y+=1;

    } while(*k++);

    ZJC_CURR_HSLOT=h->table+(idex%(h->m.count));                                            };

int KINHSLOT()                              {

    ZJC_CURR_HNODE=ZJC_CURR_HSLOT->head;
    if(!ZJC_CURR_HNODE->key) { return 0; }

    ZJC_PREV_HNODE=NULL;
    do {                                    // loop through list until key is found

        if(!__wrstrcmp(ZJC_CURR_HNODE->key, ZJC_LKEY_HASH))    { return 1; }

        elif(ZJC_PREV_HNODE)                { // if both nodes are empty assume empty list
            if(!ZJC_CURR_HNODE->idex && !ZJC_PREV_HNODE->idex) { return 0; }                };

        ZJC_PREV_HNODE=ZJC_CURR_HNODE; ZJC_CURR_HNODE=ZJC_CURR_HNODE->next;

    } while(ZJC_CURR_HNODE); return 0;                                                      };

int INHASH(HASH* h, char* key)              { HASHIT(h, key); return KINHSLOT();            };

int NK4HSLOT()                              {

    STACKPOP(byref(ZJC_CURR_HSLOT->stack), ZJC_LIDX_HASH);
    if(ZJC_LIDX_HASH==(uint) ERROR) { return ERROR; } // throw list full

    ZJC_CURR_HNODE       = (ZJC_CURR_HSLOT->nodes)+ZJC_LIDX_HASH;
    ZJC_CURR_HNODE->key  = ZJC_LKEY_HASH;
    ZJC_CURR_HNODE->idex = ZJC_LIDX_HASH;

    if(ZJC_PREV_HNODE) { ZJC_PREV_HNODE->next = ZJC_CURR_HNODE; }
    else               { ZJC_CURR_HSLOT->head = ZJC_CURR_HNODE; };

    ZJC_LIDX_HASH=0; return 0;                                                              };

//   ---     ---     ---     ---     ---

int STHASH(void* data)                      { ZJC_CURR_HNODE->data=data;                    };

void* GTHASH(int pop)                       {

    if(pop) {

        STACKPUSH(byref(ZJC_CURR_HSLOT->stack), ZJC_CURR_HNODE->idex);

        /* if not first node, connect next to prev **
        ** if first node and next, make next head  **
        ** else list is empty                      */

        if  (ZJC_PREV_HNODE      )          { ZJC_PREV_HNODE->next=ZJC_CURR_HNODE->next;    }
        elif(ZJC_CURR_HNODE->next)          { ZJC_CURR_HSLOT->head=ZJC_CURR_HNODE->next;    }
        else                                { ZJC_CURR_HSLOT->head=ZJC_CURR_HSLOT->nodes+0; };

        ZJC_CURR_HNODE->key=NULL; ZJC_CURR_HNODE->next=NULL;
        void* data=ZJC_CURR_HNODE->data; ZJC_CURR_HNODE->data=NULL;

        ZJC_CURR_HNODE=NULL; return data;

    }; return ZJC_CURR_HNODE->data;                                                         };

//   ---     ---     ---     ---     ---