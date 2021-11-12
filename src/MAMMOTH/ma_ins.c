/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    MA_INS                                *
*                                           *
*     -turns numbers into actions           *
*     -"low-level" instructions             *
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

#include "ma_ins.h"
#include "ma_boiler.h"

#include <stdio.h>
#include <string.h>

//   ---     ---     ---     ---     ---

static NIHIL lm_ins_arr[] = {               // table of low-level instructions
    &lmcpy,
    &lmmov,
    &lmwap,
    &lmwed,

    &lmjmp,
    &lmjif,
    &lmeif,
    &lmexit,

    &lmadd,
    &lmsub,
    &lminc,
    &lmdec,
    &lmmul,
    &lmdiv,
    &lmmod,

    &lmand,
    &lmor,
    &lmxor,
    &lmnor,
    &lmnand,
    &lmxnor,

    &lmtil,
    &lmcl,
    &lmclm,
    &lmnot,

    &lmshr,
    &lmshl,

    &lmlis,
    &lmsow,
    &lmreap

};

static CODE* ins          = NULL;           // current instruction

static uint  costk[16];                     // stack of code locations
static uint  costk_top    = 0;              // top of stack

static uint  fetflg       = 0x00;

//   ---     ---     ---     ---     ---

typedef struct INS_SIZES {

    uint    cast;
    uint    cbyte;

    uint    size;
    uint    elems;
    uint    units;
    uint    step;

    MEMUNIT mask_a;
    MEMUNIT mask_b;

} INSZ; static INSZ ins_sz={0};

void svinsz(uint cast_to) {

    ins_sz.cast   = rd_cast;
    ins_sz.cbyte  = rd_cbyte;

    ins_sz.size   = rd_size;
    ins_sz.elems  = rd_elems;
    ins_sz.units  = rd_units;
    ins_sz.step   = rd_step;

    ins_sz.mask_a = szmask_a;
    ins_sz.mask_b = szmask_b;

    rd_cast       = cast_to;                                                                };

void ldinsz(void) {

    rd_cast  = ins_sz.cast;
    rd_cbyte = ins_sz.cbyte;

    rd_size  = ins_sz.size;
    rd_elems = ins_sz.elems;
    rd_units = ins_sz.units;
    rd_step  = ins_sz.step;

    szmask_a = ins_sz.mask_a;
    szmask_b = ins_sz.mask_b;                                                               };

//   ---     ---     ---     ---     ---

void lmpush(uint loc)                       { costk[costk_top]=loc; costk_top++;            };
void lmpop (void    )                       { costk_top--; uint loc=costk[costk_top];       \
                                              ins = (CODE*) (mammi->jmpt[loc]);             \
                                              lm_ins_arr[ins->loc](); ins=NULL;             };

void ldins (uint loc)                       { ins = (CODE*) (mammi->jmpt[loc]);             \
                                              lm_ins_arr[ins->loc](); ins=NULL;             };

//   ---     ---     ---     ---     ---

void lmoff(uint* off)                       {

    off[1]           =                       off[0] / UNITSZ;
    off[0]          -=                       off[1] * UNITSZ;
    off[1]           = FETMASK              (rd_units, off[1]);                             }

//   ---     ---     ---     ---     ---

int lmfet(uintptr_t* dst        ,
          uint*      udr        ,
          uint*      off        ,
          uint       allow      )           {

                                            // cleanup
    RSTPTRS                                 (                                   );

                                            // apply typing to the read
    TPADDR                                  (rd_cast, -1                        );

                                            // fetch target
    lmasl                                   (udr                                );

//   ---     ---     ---     ---     ---

    if(allow&1) {                           // enforce alignment
        off[0]=0;
        while((*rd_result)%16) {
            (*rd_result)--;
            off[0]++;

        };

//   ---     ---     ---     ---     ---

        off[1]       ^= off[1];
        lmoff(off);

    }; *dst           = *rd_result;
    return DONE;                                                                            };

//   ---     ---     ---     ---     ---

#define IF_CONST_ALLOWED(allow, v, src, cbyte, cunit, mask) {                               \
    if  (allow&1) { v=( ((MEMUNIT*) (src))[ cunit ]&(mask<<(cbyte*8))) >> (cbyte*8); }      \
    else          { v=(*rd_result)&mask;                                             }      }

//   ---     ---     ---     ---     ---

#define ONE_FET_OP(ac)                                                                       \
    uintptr_t  addr          = 0;           /* sole operand                    */            \
    uint       udr           = 0;           /* offset into ins->data           */            \
    uint       offsets[2];                  /* [0..2] upos, cbyte              */            \
                                                                                             \
/*   ---     ---     ---     ---     ---  */                                                 \
                                                                                             \
    if(ac&2) {                                                                               \
        svinsz(0x06);                                                                        \
                                                                                             \
    };                                                                                       \
                                                                                             \
    lmfet(&addr, &udr, offsets, ac);                                                         \
                                                                                             \
    MEMUNIT value;                                                                           \
    IF_CONST_ALLOWED(ac, value, addr, offsets[0], offsets[1], szmask_a)                      \
                                                                                             \
    if(ac&2) {                                                                               \
        ldinsz();                                                                            \
                                                                                             \
    }

#define MNY_FET_OP                                                                           \
    uintptr_t  addr          = 0;           /* sole operand                    */            \
    uint       udr           = 0;           /* offset into ins->data           */            \
    uint       offsets[2];                  /* [0..2] upos, cbyte              */            \
    MEMUNIT    value;                                                                        \
                                                                                             \

#define FET_NXT(ac)                                                                          \
    addr       ^= addr;                                                                      \
    value      ^= value;                                                                     \
    offsets[0] ^= offsets[0];                                                                \
    offsets[1] ^= offsets[1];                                                                \
                                                                                             \
    if(ac&2) {                                                                               \
        svinsz(0x06);                                                                        \
                                                                                             \
    };                                                                                       \
                                                                                             \
    lmfet(&addr, &udr, offsets, ac);                                                         \
                                                                                             \
    IF_CONST_ALLOWED(ac, value, addr, offsets[0], offsets[1], szmask_a)                      \
                                                                                             \
    if(ac&2) {                                                                               \
        ldinsz();                                                                            \
                                                                                             \
    }

//   ---     ---     ---     ---     ---

#define TWO_FET_OP(aca, acb)                                                                 \
                                                                                             \
    uintptr_t  addr_a        = 0;           /* dst operand                     */            \
    uintptr_t  addr_b        = 0;           /* src operand                     */            \
                                                                                             \
    uint       udr           = 0;           /* offset into ins->data           */            \
    uint       offsets[4];                  /* [0..2] upos_a, cbyte_a          */            \
                                            /* [2..4] upos_b, cbyte_b          */            \
                                                                                             \
/*   ---     ---     ---     ---     ---  */                                                 \
                                                                                             \
    if(aca&2) {                                                                              \
        svinsz(0x06);                                                                        \
                                                                                             \
    };                                                                                       \
                                                                                             \
    lmfet(&addr_a, &udr, offsets, aca);                                                      \
                                                                                             \
    MEMUNIT value_a;                                                                         \
    IF_CONST_ALLOWED(aca, value_a, addr_a, offsets[0], offsets[1], szmask_a);                \
                                                                                             \
    if(aca&2) {                                                                              \
        ldinsz();                                                                            \
                                                                                             \
    };                                                                                       \
                                                                                             \
    szmask_b                 = szmask_a;                                                     \
                                                                                             \
/*   ---     ---     ---     ---     ---                                       */            \
                                                                                             \
    if(acb&2) {                                                                              \
        svinsz(0x06);                                                                        \
                                                                                             \
    };                                                                                       \
                                                                                             \
    lmfet(&addr_b, &udr, offsets+2, acb);                                                    \
                                                                                             \
    {   ulong tmp            = szmask_a;                                                     \
        szmask_a             = szmask_b;                                                     \
        szmask_b             = tmp;                                                         }\
                                                                                             \
    MEMUNIT value_b;                                                                         \
    IF_CONST_ALLOWED(acb, value_b, addr_b, offsets[2], offsets[3], szmask_b);                \
                                                                                             \
    if(acb&2) {                                                                              \
        ldinsz();                                                                            \
                                                                                             \
    }

//   ---     ---     ---     ---     ---

void lmcpy(void)                            {

    TWO_FET_OP(0b01, 0);

    if(typedata.flags&0x10) {               // corner case: strcpy

        uint     len;                       // bytes to read from source/copy to destination

        uchar*   s   = NULL;                // the source string itself
        MEMUNIT* src = NULL;

//   ---     ---     ---     ---     ---

        if(fetflg&0x10) {                   // this bit means it's a string fetch from a var

            fetflg  ^= 0x10;
            uint loc = ADDRTOLOC(value_b);  // get nearest 16-aligned address in table
                                            // get location of that within jump table

            if(loc!=FATAL) {

                // the label contains decl typedata for the var
                LABEL* l = mammi->jmpt_h+loc;

                s        = (uchar*) ((uintptr_t) value_b);
                src      = mammi->jmpt+loc;

                uint pos = (((uintptr_t) value_b)+offsets[0]) - mammi->jmpt[loc];

                len      = l->meta.strus-(pos*(l->meta.strus!=0));

            } else {
                CALOUT(E, "BAD PTR: %s can't find addr <0x%" PRIXPTR ">\n",
                __func__, value_b); return;

            };

//   ---     ---     ---     ---     ---

        } else {                            // it's a calcus string constant
                                            // ie a long value hardcoded into the instruction

            CTOK* t = (CTOK*) ((uintptr_t) value_b);

            len     = t->vsize;
            s       = (uchar*) &(t->value);

        };

//   ---     ---     ---     ---     ---

        if(!s) {
            CALOUT(E, "BAD PTR: %s could not fetch srcstr from addr <0x%" PRIXPTR ">\n",
            __func__, value_b);

        }; uchar term = 0;

        if(len>1) {
            term      = (s[len-1] | (s[len-2]<<8)) == 0x5C30;
            len      -= term*2;

        };

        MEMUNIT  mask = 0x00LL;
        MEMUNIT  sstr = 0x00LL;

        MEMUNIT* dst  = ((MEMUNIT*) addr_a)+offsets[1];

//   ---     ---     ---     ---     ---

        uint  mxchars;
        uint  old_len;
        uint* dst_len; {

            uint loc = ADDRTOLOC(addr_a);   // get nearest

            if(loc!=FATAL) {
                LABEL* l = mammi->jmpt_h+loc;
                dst_len  = &(l->meta.strus);
                old_len  = *dst_len;

                mxchars  = GTUNITCNT(1, l->meta.strsz);
                uint pos = (((uintptr_t) dst)+offsets[0]) - mammi->jmpt[loc];

//   ---     ---     ---     ---     ---

                if((mammi->jmpt+loc)==src) {
                    len=addrdist( ((uchar*) dst)+offsets[0], s);

                };

                if((len+pos)>mxchars) {
                    len=mxchars-pos;

                };

                if( ((len+pos)>(*dst_len)            ) \
                ||  ((term) && ((len+pos)<(*dst_len))) ) {
                    *dst_len=len+pos;

                };

//   ---     ---     ---     ---     ---

            } else {
                CALOUT(E, "BAD PTR: %s can't find addr <0x%" PRIXPTR ">\n",
                __func__, addr_a); return;

            };

        };

//   ---     ---     ---     ---     ---

        for(uint x=0; x<len; x++) {

            // move char to byte offset
            MEMUNIT c  = ((MEMUNIT) s[x]) << (offsets[0]*8);

            // add byte offset to mask
            // append shifted char to substr
            mask      |= (0xFFLL << (offsets[0]*8));
            sstr      |= c;

            offsets[0]++;                   // go to next byte

            if(offsets[0] && !(offsets[0]%UNITSZ)) {

                *dst &=~ mask; mask^=mask;  // clear out jic and flip substr bits
                *dst |=  sstr; sstr^=sstr;  // clear mask and substr, go to next unit

                offsets[0]^=offsets[0]; dst++;

            };

//   ---     ---     ---     ---     ---

        } if(mask) {                        // copy leftovers
            *dst &=~ mask;
            *dst |=  sstr;

        } if(term) {

            mask ^=  mask;

            for(uint x=len; x<old_len; x++) {
                mask |= (0xFFLL << (offsets[0]*8));
                offsets[0]++;

                if(offsets[0] && !(offsets[0]%UNITSZ)) {
                    *dst &=~ mask; mask^=mask;
                    offsets[0]^=offsets[0]; dst++;

                };

            } if(mask) {
                *dst &=~ mask;

            };
        }; return;
    };

//   ---     ---     ---     ---     ---

                                            // clean masked section jic and set
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_b     << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      value_b       << (offsets[0]*8);                };

void lmmov(void)                            {

    TWO_FET_OP(0b01, 0b01);

    if(typedata.flags&0x10) {               // corner case: strmov
                                            // preeetty similar to strcpy, but not equal

        uint     len;

        uchar    term = 0;
        uchar*   s    = NULL;

        MEMUNIT* src  = ((MEMUNIT*) addr_b)+offsets[3];

//   ---     ---     ---     ---     ---

        if(fetflg&0x10) {

            fetflg  ^= 0x10;
            uint loc = ADDRTOLOC(addr_b);

            if(loc!=FATAL) {
                LABEL* l      = mammi->jmpt_h+loc;

                s             = ((uchar*) (src));
                uint pos      = ((addr_b)+offsets[3]) - mammi->jmpt[loc];

                len           = l->meta.strus;

                if(len>1) {
                    term      = (s[len-1] | (s[len-2]<<8)) == 0x5C30;
                    s[len-1]  = (!term)*(s[len-1]);
                    s[len-2]  = (!term)*(s[len-2]);

                };

                s            += offsets[2];

                len          -= ((term*2)+pos)*(len!=0);
                l->meta.strus = pos*(len!=0);

                src           = mammi->jmpt+loc;

            } else {
                CALOUT(E, "BAD PTR: %s can't find addr <0x%" PRIXPTR ">\n",
                __func__, addr_b); return;

            };

//   ---     ---     ---     ---     ---

        } else {
            CALOUT(E, "BAD PTR: %s source must be a valid addr\n", __func__); return;

        };

//   ---     ---     ---     ---     ---

        if(!s) {
            CALOUT(E, "BAD PTR: %s could not fetch srcstr from addr <0x%" PRIXPTR ">\n",
            __func__, src);

        };

        MEMUNIT  mask_a = 0x00LL;
        MEMUNIT  mask_b = 0x00LL;
        MEMUNIT  sstr   = 0x00LL;

        MEMUNIT* dst    = ((MEMUNIT*) addr_a)+offsets[1];

//   ---     ---     ---     ---     ---

        uint  mxchars;
        uint  old_len;
        uint* dst_len; {

            uint loc = ADDRTOLOC(addr_a);

            if(loc!=FATAL) {
                LABEL* l = mammi->jmpt_h+loc;
                dst_len  = &(l->meta.strus);
                old_len  = *dst_len;

                mxchars  = GTUNITCNT(1, l->meta.strsz);
                uint pos = (((uintptr_t) dst)+offsets[0]) - mammi->jmpt[loc];

//   ---     ---     ---     ---     ---

                if((mammi->jmpt+loc)==src) {
                    len=addrdist( ((uchar*) dst)+offsets[0], s);

                };

                if((len+pos)>mxchars) {
                    len=mxchars-pos;

                };

                if( ((len+pos)>(*dst_len)            ) \
                ||  ((term) && ((len+pos)<(*dst_len))) ) {
                    *dst_len=len+pos;

                };

//   ---     ---     ---     ---     ---

            } else {
                CALOUT(E, "BAD PTR: %s can't find addr <0x%" PRIXPTR ">\n",
                __func__, addr_a); return;

            };

        };

//   ---     ---     ---     ---     ---

        src = ((MEMUNIT*) addr_b)+offsets[3];

        for(uint x=0; x<len; x++) {

            MEMUNIT c  = ((MEMUNIT) s[x]) << (offsets[0]*8);

            mask_a    |= (0xFFLL << (offsets[0]*8));
            mask_b    |= (0xFFLL << (offsets[2]*8));

            sstr      |= c;

            offsets[0]++;
            offsets[2]++;

            if(offsets[0] && !(offsets[0]%UNITSZ)) {

                *dst &=~ mask_a; mask_a ^= mask_a;
                *dst |=  sstr;   sstr   ^= sstr;

                // clear out source
                *src &=~ mask_b; mask_b ^= mask_b;

                offsets[0]^=offsets[0]; dst++;
                offsets[2]^=offsets[2]; src++;

            };

//   ---     ---     ---     ---     ---

        } if(mask_a) {                      // copy leftovers
            *src &=~ mask_b;
            *dst &=~ mask_a;
            *dst |=  sstr;

//   ---     ---     ---     ---     ---

        } if(term) {

            mask_a ^=  mask_a;

            for(uint x=len; x<old_len; x++) {
                mask_a |= (0xFFLL << (offsets[0]*8));
                offsets[0]++;

                if(offsets[0] && !(offsets[0]%UNITSZ)) {
                    *dst &=~ mask_a; mask_a^=mask_a;
                    offsets[0]^=offsets[0]; dst++;

                };

            } if(mask_a) {
                *dst &=~ mask_a;

            };
        }; return;
    };

//   ---     ---     ---     ---     ---

                                            // ^same as cpy
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_b     << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      value_b       << (offsets[0]*8);

                                            // then clean src
    ((MEMUNIT*) addr_b)[offsets[3]] &=~     (szmask_a     << (offsets[2]*8));               };

void lmwap(void)                            {

    TWO_FET_OP(0b01, 0b01);

    if(typedata.flags&0x10) {               // corner case: strwap
                                            // really nothing like strcpy...

        uint*    src_len;
        uint     src_pos;
        uint     src_mxchars;

        uchar*   src_s = NULL;
        MEMUNIT* src   = ((MEMUNIT*) addr_b)+offsets[3];

//   ---     ---     ---     ---     ---

        if(fetflg&0x10) {

            fetflg  ^= 0x10;
            uint loc = ADDRTOLOC(addr_b);

            if(loc!=FATAL) {

                LABEL* l      = mammi->jmpt_h+loc;

                src_s         = ((uchar*) src)+offsets[2];
                src_pos       = ((addr_b)+offsets[2]) - mammi->jmpt[loc];
                src_len       = &(l->meta.strus);

                src_mxchars   = GTUNITCNT(1, l->meta.strsz);

                src           = mammi->jmpt+loc;

            } else {
                CALOUT(E, "BAD PTR: %s can't find addr <0x%" PRIXPTR ">\n",
                __func__, addr_b); return;

            };

//   ---     ---     ---     ---     ---

        } else {
            CALOUT(E, "BAD PTR: %s source must be a valid addr\n", __func__); return;

        };

//   ---     ---     ---     ---     ---

        if(!src_s) {
            CALOUT(E, "BAD PTR: %s could not fetch srcstr from addr <0x%" PRIXPTR ">\n",
            __func__, src);

        };

        MEMUNIT  mask = 0x00LL;
        MEMUNIT  sstr = 0x00LL;

        MEMUNIT* dst  = ((MEMUNIT*) addr_a)+offsets[1];

//   ---     ---     ---     ---     ---

        uint   src_slen = 0;                // num of chars to copy from src to dst
        uint   dst_slen = 0;                // ^viceversa

        uchar* dst_s    = NULL;

        uint   dst_mxchars;
        uint   dst_pos;
        uint*  dst_len; {

            uint loc = ADDRTOLOC(addr_a);

            if(loc!=FATAL) {

                LABEL* l    = mammi->jmpt_h+loc;
                dst_len     = &(l->meta.strus);

                dst_mxchars = GTUNITCNT(1, l->meta.strsz);
                dst_pos     = (((uintptr_t) dst)+offsets[0]) - mammi->jmpt[loc];

//   ---     ---     ---     ---     ---

                src_slen    = ( (*src_len) - (src_pos*(*src_len!=0)) );
                dst_slen    = ( (*dst_len) - (dst_pos*(*dst_len!=0)) );

                if((mammi->jmpt+loc)==src) {
                    src_slen=addrdist( ((uchar*) dst)+offsets[0], src_s);
                    dst_slen=src_slen;

                };


                if((src_slen+dst_pos)>dst_mxchars) {
                    src_slen=dst_mxchars-dst_pos;

                };

                if(((dst_slen+src_pos)>src_mxchars)) {
                    dst_slen=src_mxchars-src_pos;

                };

                *src_len=src_pos+dst_slen;
                *dst_len=dst_pos+src_slen;

//   ---     ---     ---     ---     ---

            } else {
                CALOUT(E, "BAD PTR: %s can't find addr <0x%" PRIXPTR ">\n",
                __func__, addr_a); return;

            };

        };

//   ---     ---     ---     ---     ---    // src to dst

        // lazy use of calcus stack for temporal string stor ^^
        RSTPTRS(); MEMUNIT* dst_tmp = rd_result;
        uint usteps=0; src=((MEMUNIT*) addr_b)+offsets[3];

        for(uint x=0; x<src_slen; x++) {

            MEMUNIT c  = ((MEMUNIT) src_s[x]) << (offsets[0]*8);
            mask      |= (0xFFLL << (offsets[0]*8));
            sstr      |= c;

            offsets[0]++;

            if(offsets[0] && !(offsets[0]%UNITSZ)) {

                *dst_tmp |= (*dst) & mask;  // copy dst before copying

                *dst     &=~ mask; mask^=mask;
                *dst     |=  sstr; sstr^=sstr;

                offsets[0]^=offsets[0]; dst++;
                dst_tmp++; usteps++;

            };

//   ---     ---     ---     ---     ---

        } if(mask) {                        // copy leftovers
            *dst_tmp |= (*dst) & mask;
            *dst     &=~ mask;
            *dst     |=  sstr;

        };

//   ---     ---     ---     ---     ---    // repeat process for dst to src

        mask  ^= mask;
        sstr  ^= sstr;

        dst_s  = (uchar*) (dst_tmp-usteps);

        for(uint x=0; x<dst_slen; x++) {

            MEMUNIT c  = ((MEMUNIT) dst_s[x]) << (offsets[2]*8);
            mask      |= (0xFFLL << (offsets[2]*8));
            sstr      |= c;

            offsets[2]++;

            if(offsets[2] && !(offsets[2]%UNITSZ)) {
                *src     &=~ mask; mask^=mask;
                *src     |=  sstr; sstr^=sstr;

                offsets[2]^=offsets[2]; src++;

            };

//   ---     ---     ---     ---     ---

        } if(mask) {                        // copy leftovers
            *src     &=~ mask;
            *src     |=  sstr;

        }; return;
    };

//   ---     ---     ---     ---     ---

                                            // ^same as cpy, but b = a
    ((MEMUNIT*)addr_b)[offsets[3]] &=~      (szmask_a     << (offsets[2]*8));
    ((MEMUNIT*)addr_b)[offsets[3]] |=       value_a       << (offsets[2]*8);

                                            // then a = old_b
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_b     << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      value_b       << (offsets[0]*8);                };

void lmwed(void)                            {

    ONE_FET_OP(0b10);
    SYMBOL* sym = mammi->gvalues+value;

    if( (*((uint*) sym->id.type)==0x45505954)
    ||  (*((uint*) sym->id.type)==0x47414C46) ) {
        sym->onrd();

    };                                                                                      };

//   ---     ---     ---     ---     ---

void lmjmp(void)                            {

    ONE_FET_OP(0b10);

    uint  loc = ADDRTOLOC(value);
    mammi->next=loc;                                                                        };

void lmjif(void)                            {

    TWO_FET_OP(0b10,0);

    if(value_b) {
        uint  loc = ADDRTOLOC(value_a);
        mammi->next=loc;

    };                                                                                      };

void lmeif(void)                            {

    TWO_FET_OP(0b10,0);

    if(!value_b) {
        uint  loc = ADDRTOLOC(value_a);
        mammi->next=loc;

    };                                                                                      };

void lmexit(void)                           {

    ONE_FET_OP(0);

                                            /* cleanup any sentinels */
    mammi->lvalues[mammi->lvaltop] ^=       mammi->lvalues[mammi->lvaltop];

    mammi->lvalues[mammi->lvaltop]  = value;/* set return code       */
    mammi->next = mammi->jmpt_i;            /* force program to quit */                     };

//   ---     ---     ---     ---     ---

void lmadd(void)                            { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a+value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmsub(void)                            { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a-value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lminc(void)                            { ONE_FET_OP(0b01);

    MEMUNIT result                  =       (value+1)&szmask_a;
    ((MEMUNIT*) addr)[offsets[1]]  &=~      (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr)[offsets[1]]  |=       result    << (offsets[0]*8);                    };

void lmdec(void)                            { ONE_FET_OP(0b01);

    MEMUNIT result                  =       (value-1)&szmask_a;
    ((MEMUNIT*) addr)[offsets[1]]  &=~      (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr)[offsets[1]]  |=       result    << (offsets[0]*8);                    };

void lmmul (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a*value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmdiv (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a/value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmmod (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a%value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

//   ---     ---     ---     ---     ---

void lmand (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a&value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmor  (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a|value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmxor (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a^value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmnor (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (~(value_a|value_b))&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmnand(void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (~(value_a&value_b))&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmxnor(void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (~(value_a^value_b))&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

//   ---     ---     ---     ---     ---

void lmtil (void)                           { ONE_FET_OP(0b01);

    MEMUNIT result                 =        (~value)&szmask_a;
    ((MEMUNIT*) addr)[offsets[1]] &=~       (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr)[offsets[1]] |=        result    << (offsets[0]*8);                    };

void lmcl  (void)                           { ONE_FET_OP(0b01);

    ((MEMUNIT*) addr)[offsets[1]] &=~       (szmask_a << (offsets[0]*8));                   };

void lmclm (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a&(~value_b))&szmask_a;

    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmnot (void)                           { ONE_FET_OP(0b01);

    MEMUNIT result                 =        (!value)&szmask_a;
    ((MEMUNIT*) addr)[offsets[1]] &=~       (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr)[offsets[1]] |=        result    << (offsets[0]*8);                    };


//   ---     ---     ---     ---     ---

void lmshr (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a>>value_b)&szmask_a;

    if(typedata.flags&1) {                  // handle signed

        if( (value_a&szmask_a) > (szmask_a/2)) {

            result                   =      ((0xFFLL<<value_b)|result)&szmask_a;

        };
    };

    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

void lmshl (void)                           { TWO_FET_OP(0b01, 0);

    MEMUNIT result                   =      (value_a<<value_b)&szmask_a;
    ((MEMUNIT*) addr_a)[offsets[1]] &=~     (szmask_a << (offsets[0]*8));
    ((MEMUNIT*) addr_a)[offsets[1]] |=      result    << (offsets[0]*8);                    };

//   ---     ---     ---     ---     ---

void lmlis (void)                           { MNY_FET_OP;

    FET_NXT(0);
    CTOK* t = (CTOK*) *rd_result;

    FET_NXT(0); MEMUNIT* s = (MEMUNIT*) &(t->value);
    for(uint x=0; x<1+(t->vsize/UNITSZ); x++) {
        rd_result[x]=s[x];

    };

//   ---     ---     ---     ---     ---

    MACRO* m     = NULL;
    void*  nulmy = NULL;

    STR_HASHGET(MNAMES_HASH, (uchar*) rd_result, nulmy, 0);
    if(nulmy!=NULL) {
        m = (MACRO*) nulmy;

    }; m->value = value;
    HASHSET(MNAMES_HASH, byref(m->id));                                                     };

#define SOW_BUFF_ASZ 256

static size_t  SOW_BUFF_APOS            = 0;
static MEMUNIT SOW_BUFF_A[SOW_BUFF_ASZ] = {0};

void lmsow (void)                           { ONE_FET_OP(0);

    if(typedata.flags&0x10) {

        uint     len;
        uchar*   s   = NULL;

//   ---     ---     ---     ---     ---

        if(fetflg&0x10) {

            fetflg  ^= 0x10;
            uint loc = ADDRTOLOC(value);

            if(loc!=FATAL) {

                LABEL* l = mammi->jmpt_h+loc;
                s        = (uchar*) ((uintptr_t) value);
                uint pos = (((uintptr_t) value)+offsets[0]) - mammi->jmpt[loc];
                len      = l->meta.strus-(pos*(l->meta.strus!=0));

            } else {
                CALOUT(E, "BAD PTR: %s can't find addr <0x%" PRIXPTR ">\n",
                __func__, value); return;

            };

//   ---     ---     ---     ---     ---

        } else {

            CTOK* t = (CTOK*) ((uintptr_t) value);

            len     = t->vsize;
            s       = (uchar*) &(t->value);

        };

//   ---     ---     ---     ---     ---

        if(!s) {
            CALOUT(E, "BAD PTR: %s could not fetch srcstr from addr <0x%" PRIXPTR ">\n",
            __func__, value);

        };

        /* TODO: terminators
        uchar term = 0;

        if(len>1) {
            term      = (s[len-1] | (s[len-2]<<8)) == 0x5C30;
            len      -= term*2;

        };*/

//   ---     ---     ---     ---     ---

        len = 1+(len/UNITSZ);
        if((len+SOW_BUFF_APOS)>SOW_BUFF_ASZ) {
            // TODO: flush
            len=SOW_BUFF_ASZ-SOW_BUFF_APOS;

        };

        for(uint x=0;x<len;x++) {
            MEMUNIT c = *((MEMUNIT*) s);
            SOW_BUFF_A[SOW_BUFF_APOS]=c;
            SOW_BUFF_APOS++; s+=UNITSZ;

        }; return;
    };

//   ---     ---     ---     ---     ---

    SOW_BUFF_A[SOW_BUFF_APOS]=value;
    SOW_BUFF_APOS++;                                                                        };

void lmreap(void)                           {

    // TODO: formatting...

    printf("%s\n", (uchar*) SOW_BUFF_A);
    CLMEM2(SOW_BUFF_A, SOW_BUFF_ASZ*UNITSZ);

};

//   ---     ---     ---     ---     ---

void lmasl(uint* udr)                       {

    CTOK* t;                                // current token

    uchar buff[UNITSZ*2];                   // for loading operator symbols
    uchar c = 0x00;                         // for reading one operator at a time

    uchar force_solve = 0;                  // quick exit flag

    mammi->ctrl^=mammi->ctrl;
    fetflg     ^=fetflg;

    EVAL_EXP: if( ((*udr)>=ins->size) \
              ||  (force_solve      ) )     { goto RESULT; }

    rd_value  = rd_lhand+rd_step;
    CLMEM2(rd_value, rd_step);

    CLMEM2(buff, UNITSZ*2);

    t         = (CTOK*) (ins->data+(*udr));
    rd_rawv   = buff+0;

//   ---     ---     ---     ---     ---

    uint w=0;
    for(uint y=0; y<ARRSIZE(t->lops); y++) { // paste leftside operators into leftside of str

        c = t->lops[y]; if(!c) {
            break;

        }; rd_rawv[y]  = c; w++;

    };                                      // ^idem, rightside

    for(int y=(ARRSIZE(t->rops)-1), z=0;
        y>-1; y--, z++                 ) {

        c = t->rops[z]; if(!c) {
            break;

        } rd_rawv[w] = c; w++;

                                            // now pop 'em to get evalstate
    }; rd_rawv[w]=0x00; POPOPS              (                             );

//   ---     ---     ---     ---     ---

    *rd_value = t->value;

    if(t->ttype==CALCUS_SEPAR) {
        force_solve=1; goto RESULT;

    } elif(t->ttype==CALCUS_CHSTR) {

        mammi->state     |= MAMMIT_SF_PSTR;
        uint slen         = 1+(t->vsize/UNITSZ);
        *rd_result        = (uintptr_t) t;

        while(slen%(sizeof(CTOK)/UNITSZ)) {
            slen++;

        }; *udr          += slen+sizeof(CTOK)/UNITSZ;

        goto EVAL_EXP;

    } elif(t->ttype==CALCUS_FETCH) {
        mammi->state |= MAMMIT_SF_PFET;
        fetflg       |= typedata.flags&0x10;

    } elif(t->ttype==CALCUS_MACRO) {
        *rd_value = ((MACRO*) ((uintptr_t) t->value))->value;

    };

//   ---     ---     ---     ---     ---    // compress expanded tokens into final value

    (*udr)+=sizeof(CTOK)/UNITSZ;

    if(mammi->state&MAMMIT_SF_PSTR) {
        mammi->state &=~MAMMIT_SF_PSTR;
        goto EVAL_EXP;

    };

    SOLVE: CALCUS_COLLAPSE();

    goto EVAL_EXP;

    RESULT: if(mammi->lvlb>0) {
        MAMMIT_LVLB_PRV;
        goto SOLVE;

    }; mammi->state &=~MAMMIT_SF_PFET;

    if(force_solve) { goto ALT_EXIT; }
    return;

    ALT_EXIT: (*udr)+=sizeof(CTOK)/UNITSZ;                                                  };

//   ---     ---     ---     ---     ---

void swcpy (void)                           { ins_code = 0x00; ins_argc = 2;                };
void swmov (void)                           { ins_code = 0x01; ins_argc = 2;                };
void swwap (void)                           { ins_code = 0x02; ins_argc = 2;                };
void swwed (void)                           { ins_code = 0x03; ins_argc = 1;                };

void swjmp (void)                           { ins_code = 0x04; ins_argc = 1;                };
void swjif (void)                           { ins_code = 0x05; ins_argc = 2;                };
void sweif (void)                           { ins_code = 0x06; ins_argc = 2;                };
void swexit(void)                           { ins_code = 0x07; ins_argc = 1;                };

void swadd (void)                           { ins_code = 0x08; ins_argc = 2;                };
void swsub (void)                           { ins_code = 0x09; ins_argc = 2;                };
void swinc (void)                           { ins_code = 0x0A; ins_argc = 1;                };
void swdec (void)                           { ins_code = 0x0B; ins_argc = 1;                };
void swmul (void)                           { ins_code = 0x0C; ins_argc = 2;                };
void swdiv (void)                           { ins_code = 0x0D; ins_argc = 2;                };
void swmod (void)                           { ins_code = 0x0E; ins_argc = 2;                };

void swand (void)                           { ins_code = 0x0F; ins_argc = 2;                };
void swor  (void)                           { ins_code = 0x10; ins_argc = 2;                };
void swxor (void)                           { ins_code = 0x11; ins_argc = 2;                };
void swnor (void)                           { ins_code = 0x12; ins_argc = 2;                };
void swnand(void)                           { ins_code = 0x13; ins_argc = 2;                };
void swxnor(void)                           { ins_code = 0x14; ins_argc = 2;                };

void swtil (void)                           { ins_code = 0x15; ins_argc = 1;                };
void swcl  (void)                           { ins_code = 0x16; ins_argc = 1;                };
void swclm (void)                           { ins_code = 0x17; ins_argc = 2;                };
void swnot (void)                           { ins_code = 0x18; ins_argc = 1;                };

void swshr (void)                           { ins_code = 0x19; ins_argc = 2;                };
void swshl (void)                           { ins_code = 0x1A; ins_argc = 2;                };

void swlis (void)                           { ins_code = 0x1B; ins_argc = 2;                };

void swsow (void)                           { ins_code = 0x1C; ins_argc = 1;                };
void swreap(void)                           { ins_code = 0x1D; ins_argc = 1;                };

//   ---     ---     ---     ---     ---

void stentry(void)                          {

    rd_tkx++; uchar* s=tokens[rd_tkx];
              int    x=0;

    for(; x<strlen(s); x++) {
        mammi->entry[x]=s[x];

    }; mammi->entry[x]=0x00;                                                                };

//   ---     ---     ---     ---     ---