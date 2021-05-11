#include "zjc_hash.h"
#include <math.h>

//   ---     ---     ---     ---     ---

static uint   ZJC_LIDX_HASH  = 0;
static ID*    ZJC_LKEY_HASH  = NULL;

static HSLOT* ZJC_CURR_HSLOT = NULL;
static void** ZJC_CURR_HNODE = NULL;
static int    ZJC_HASH_GRAVE = -4;
static int    ZJC_HASH_FLAGS = 0;

static cuint  ZJC_HASH_JMP   = (sizeof(HSLOT)                    )  // header
                             + (sizeof(uint ) * (ZJC_HASH_STKLEN)); // slotstack

//   ---     ---     ---     ---     ---
/* for now, insert is our only check...   **
** in the future we can use this to have  **
** some more states, maybe?               */

#define ZJC_HASH_FINSRT 1
void FLHASH(int flags)                      { ZJC_HASH_FLAGS=flags;                         }

//   ---     ---     ---     ---     ---

void HASHIT(HASH* h, char* k)               {

    uint idex     = 0;

    uint x        = 0;
    uint y        = 0;

    do {

        x=((uint) *k)+1; x*=x;
        idex+=x+y; y+=1;

    } while(*k++);

    ZJC_LKEY_HASH->x=idex%(h->nslots);
    ZJC_CURR_HSLOT=MEMBUFF(byref(h->m), HSLOT, ZJC_HASH_JMP*(ZJC_LKEY_HASH->x));            };

//   ---     ---     ---     ---     ---

void NITHASH(HASH* h)                       {

    for(uint x=0; x<h->nslots; x++)
    {
        uint offset=x*ZJC_HASH_JMP;
        HSLOT* slot=MEMBUFF(byref(h->m), HSLOT, offset);

        MKSTK(byref(slot->stack), ZJC_HASH_STKLEN);
        for(uint y=ZJC_HASH_STKLEN; y>0; y--)
        {
            STACKPUSH(byref(slot->stack), y-1);

        };

    };                                                                                      };

//   ---     ---     ---     ---     ---

HASH* MKHASH(uint mag, char* name)            {

    uint nslots=1; ID id=IDNEW("HST*", name);
    for(uint x=0; x<mag; x++) { nslots*=2; }

    HASH* h; MEMGET(HASH, h, (nslots*ZJC_HASH_JMP), &id);
    h->nslots=nslots; NITHASH(h); return h;                                                 };

HASH* GWHASH(HASH* h)                       {

    uint nslots=h->nslots; nslots*=2;
    HASH* new_hash; MEMGET(HASH, new_hash, (nslots*ZJC_HASH_JMP), byref(h->m.id));
    new_hash->nslots=nslots; NITHASH(new_hash);

    for(uint x=0; x<h->nslots; x++)
    {

        uint offset=x*ZJC_HASH_JMP;
        HSLOT* src_slot=MEMBUFF(byref(h->m), HSLOT, offset);

        if(&(src_slot->nodes[0]) == NULL) { continue; }; int i=0;
        for(void** src_node = &(src_slot->nodes[0]); i<ZJC_HASH_STKLEN; src_node++) {

            if(*src_node==NULL) { break; }
            if(*src_node != &ZJC_HASH_GRAVE) {

                ID* lkp=(ID*) (*src_node);
                lkp->x=-1; lkp->y=-1;

                ZJC_LKEY_HASH=lkp; HASHIT(new_hash, lkp->key);
                NK4HSLOT(); STHASH(*src_node);

            }; i++;

        };

    }; DLMEM(h); return new_hash;                                                           };

void CLHASH(HASH* h)                        { h->nitems=0; CLMEM(byref(h->m)); NITHASH(h);  };

//   ---     ---     ---     ---     ---

int KINHSLOT(void)                          {

    ZJC_CURR_HNODE=&(ZJC_CURR_HSLOT->nodes[0]);
    if(!(*ZJC_CURR_HNODE)) { return 0; }

    ID* lkp=(ID*) (*ZJC_CURR_HNODE);

//   ---     ---     ---     ---     ---    find by key alone

    if(ZJC_LKEY_HASH->y==-2)
    {

        if(!(lkp->key)) { return 0; }

        for(int i=0; i<ZJC_HASH_STKLEN; i++) {

            if(*ZJC_CURR_HNODE==NULL || lkp->key==NULL) { break; }

            // skip deleted node on fetch
            if(*ZJC_CURR_HNODE == &ZJC_HASH_GRAVE)
            {
                if(ZJC_HASH_FLAGS & ZJC_HASH_FINSRT) { return 0; }

            } elif(!__wrstrcmp(lkp->key, ZJC_LKEY_HASH->key))
            { ZJC_LKEY_HASH->y=lkp->y; return 1; }

            ZJC_CURR_HNODE++; lkp=(ID*) (*ZJC_CURR_HNODE);

        }; return 0;
    };

//   ---     ---     ---     ---     ---    find by token

    for(int i=0; i<ZJC_HASH_STKLEN; i++) {

        if(*ZJC_CURR_HNODE==NULL || lkp->key==NULL) { break; }

        // skip deleted node on fetch
        if(*ZJC_CURR_HNODE==&ZJC_HASH_GRAVE)
        {
            if(ZJC_HASH_FLAGS & ZJC_HASH_FINSRT) { return 0; }

        }

        // new lookup token for existing key
        elif( !(ZJC_HASH_FLAGS & ZJC_HASH_FINSRT) \
        &&     (ZJC_LKEY_HASH->y<0              ) )
        {
            // if keys match, copy data to token
            if(!__wrstrcmp(lkp->key, ZJC_LKEY_HASH->key))
            { *ZJC_LKEY_HASH=*lkp; return 1; }

        }; ZJC_CURR_HNODE++; lkp=(ID*) (*ZJC_CURR_HNODE);

    }; return 0;                                                                            };

//   ---     ---     ---     ---     ---

int INHASH(HASH* h, ID* lkp)                {

    ZJC_LKEY_HASH=lkp;

    if(lkp->y>=0 && lkp->x>=0)
    {
        ZJC_CURR_HSLOT=MEMBUFF(byref(h->m), HSLOT, ZJC_HASH_JMP*lkp->x);
        ZJC_CURR_HNODE=&(ZJC_CURR_HSLOT->nodes[lkp->y]);
        return *ZJC_CURR_HNODE!=&ZJC_HASH_GRAVE;

    }; HASHIT(h, lkp->key); return KINHSLOT();                                              };

int NK4HSLOT(void)                          {

    STACKPOP(byref(ZJC_CURR_HSLOT->stack), ZJC_LIDX_HASH);
    if(ZJC_LIDX_HASH==(uint) ERROR) { return ERROR; } // throw list full

    ZJC_LKEY_HASH->y = ZJC_LIDX_HASH;
    ZJC_CURR_HNODE   = &(ZJC_CURR_HSLOT->nodes[ZJC_LKEY_HASH->y]);

    ZJC_LIDX_HASH=0; return 0;                                                              };

//   ---     ---     ---     ---     ---

void STHASH(void* data)                     { *ZJC_CURR_HNODE=data;                         };

void GTHASH(void** to, int pop)             { *to=*ZJC_CURR_HNODE;

    if(pop) {

        STACKPUSH(byref(ZJC_CURR_HSLOT->stack), ZJC_LKEY_HASH->y);
        ID* id=(ID*) (*ZJC_CURR_HNODE); id->x=-1; id->y=-1;
        *ZJC_CURR_HNODE=&ZJC_HASH_GRAVE;

    };                                                                                      };

//   ---     ---     ---     ---     ---