#include "zjc_hash.h"
#include <math.h>

//   ---     ---     ---     ---     ---

static uint       ZJC_LIDX_HASH  = 0;
static LKUP*      ZJC_LKEY_HASH  = NULL;

static HSLOT*     ZJC_CURR_HSLOT = NULL;
static void**     ZJC_CURR_HNODE = NULL;

//   ---     ---     ---     ---     ---

void MKHASH(HASH* h, uint mag, char* id)    {

    h->nslots=1; uint stklen=9;
    for(uint x=0; x<mag; x++) { h->nslots*=2; }

    h->m        = (MEM){0};

    h->m.id     = id;
    h->m.size   = 1;

    h->jmp      = ( (sizeof(HSLOT)                          ) \
                  + (sizeof(void*)                 * stklen ) \
                  + (sizeof(STK  ) + (sizeof(uint) * stklen)) \
                  + (sizeof(LKUP )                 * stklen ) );


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

        slot->nodes = MEMBUFF(byref(h->m), void*, offset); offset+=sizeof(void*)*stklen;
        slot->stack = MEMBUFF(byref(h->m), STK,   offset); offset+=sizeof(STK);

        MKSTK(slot->stack, stklen);
        for(uint y=stklen; y>0; y--)
        {
            STACKPUSH(slot->stack, y-1);

        }; offset+=sizeof(uint)*stklen;

        slot->keycache=MEMBUFF(byref(h->m), LKUP,  offset);
        for(uint y=stklen; y<0; y++)
        {
            slot->keycache[y]=NITKEY(NULL);

        };

    };                                                                                      };

//   ---     ---     ---     ---     ---

void HASHIT(HASH* h, char* k)               {

    uint idex     = 0;

    uint x        = 0;
    uint y        = 0;

    do {

        x=((uint) *k)+1; x*=x;
        idex+=x+y; y+=1;

    } while(*k++);

    ZJC_LKEY_HASH->idex=idex%(h->nslots);
    ZJC_CURR_HSLOT=MEMBUFF(byref(h->m), HSLOT, h->jmp*(ZJC_LKEY_HASH->idex));               };

int KINHSLOT(void)                          {

    ZJC_CURR_HNODE=ZJC_CURR_HSLOT->nodes;
    if(!(*ZJC_CURR_HNODE)) { return 0; }

    LKUP* lkp=ZJC_CURR_HSLOT->keycache;
    if(ZJC_LKEY_HASH->mod==-2)
    {

        if(!(lkp->key)) { return 0; }

        do
        {
            if(!__wrstrcmp(lkp->key, ZJC_LKEY_HASH->key))
            { ZJC_LKEY_HASH->mod=lkp->mod; return 1; }

            ZJC_CURR_HNODE++; lkp++;

        } while(*ZJC_CURR_HNODE && lkp->key); return 0;
    };

    do { ZJC_CURR_HNODE++; } while(*ZJC_CURR_HNODE); return 0;                              };

int INHASH(HASH* h, LKUP* lkp)              {

    ZJC_LKEY_HASH=lkp;
    if(lkp->mod>=0 && lkp->idex>=0)
    {
        ZJC_CURR_HSLOT=MEMBUFF(byref(h->m), HSLOT, h->jmp*lkp->idex);
        ZJC_CURR_HNODE=ZJC_CURR_HSLOT->nodes+lkp->mod; return 1;
    }

    HASHIT(h, lkp->key); return KINHSLOT();                                                 };

int NK4HSLOT(void)                          {

    STACKPOP(ZJC_CURR_HSLOT->stack, ZJC_LIDX_HASH);
    if(ZJC_LIDX_HASH==(uint) ERROR) { return ERROR; } // throw list full

    ZJC_LKEY_HASH->mod     = ZJC_LIDX_HASH;
    ZJC_CURR_HNODE         = (ZJC_CURR_HSLOT->nodes)+ZJC_LKEY_HASH->mod;

    ZJC_LIDX_HASH=0; return 0;                                                              };

//   ---     ---     ---     ---     ---

void STHASH(void* data)                     { *ZJC_CURR_HNODE=data;                         };

void GTHASH(void** to, int pop)             {

    *to=*ZJC_CURR_HNODE;
    if(pop) {

        STACKPUSH(ZJC_CURR_HSLOT->stack, ZJC_LKEY_HASH->mod);
        *ZJC_CURR_HNODE=NULL;

    };
};

//   ---     ---     ---     ---     ---