#include "zjc_hash.h"
#include <math.h>

//   ---     ---     ---     ---     ---

static uint   ZJC_LIDX_HASH  = 0;
static char*  ZJC_LKEY_HASH  = NULL;

static HSLOT* ZJC_CURR_HSLOT = NULL;
static HNODE* ZJC_CURR_HNODE = NULL;
static HNODE* ZJC_PREV_HNODE = NULL;

//   ---     ---     ---     ---     ---

void MKHASH(HASH* h, uint mag, char* id)    {

    h->nslots=1; uint stklen=1;
    for(uint x=0; x<mag; x++) { h->nslots*=2; stklen+=1; }

    h->m        = (MEM){0};

    h->m.id     = id;
    h->m.size   = 1;

    h->jmp      = ( (sizeof(HSLOT)                          ) \
                  + (sizeof(STK  ) + (sizeof(uint) * stklen)) \
                  + (sizeof(HNODE)                 * stklen ) );

    h->m.count  = h->jmp*h->nslots;
    h->m.free   = NULL;

    h->nitems   = 0;

    uint offset = 0;
    HSLOT* slot = NULL;

    MEMGET(byref(h->m), slot, HSLOT);

    for(uint x=0; x<h->nslots; x++)
    {
        offset      = x*h->jmp;

        slot        = MEMBUFF(byref(h->m), HSLOT, offset); offset+=sizeof(HSLOT);
        slot->stack = MEMBUFF(byref(h->m), STK,   offset);
        offset+=sizeof(STK); MKSTK(slot->stack, stklen);

        for(uint y=stklen; y>0; y--)
        {
            STACKPUSH(slot->stack, y-1);

        }; offset+=sizeof(uint)*stklen;

        slot->nodes=MEMBUFF(byref(h->m), HNODE, offset);
        slot->head=slot->nodes+0; slot->head->next=NULL;

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

    ZJC_CURR_HSLOT=MEMBUFF(byref(h->m), HSLOT, h->jmp*(idex%(h->nslots)));                  };

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

    STACKPOP(ZJC_CURR_HSLOT->stack, ZJC_LIDX_HASH);
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

        STACKPUSH(ZJC_CURR_HSLOT->stack, ZJC_CURR_HNODE->idex);

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