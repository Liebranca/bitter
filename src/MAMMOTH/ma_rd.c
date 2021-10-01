/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    MA_RD                                 *
*                                           *
*     -reads pe files                       *
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

#include "KVRNEL/zjc_CommonTypes.h"
#include "KVRNEL/kvr_paths.h"
#include "KVRNEL/MEM/kvr_str.h"

#include "KVRNEL/TYPES/zjc_hash.h"

#include <stdio.h>
#include <string.h>

//   ---     ---     ---     ---     ---
// state flags...

#define MAMMIT_SF_PESO 0x000000FF

#define MAMMIT_SF_PESC 0x00000001
#define MAMMIT_SF_PLIT 0x00000002

#define MAMMIT_SF_CNTX 0x0000FF00

#define MAMMIT_SF_CREG 0x00000100
#define MAMMIT_SF_CDEC 0x00000200
#define MAMMIT_SF_CDEF 0x00000400

#define MAMMIT_SF_CCLA 0x00000800
#define MAMMIT_SF_CFUN 0x00001000

#define MAMMIT_SF_OPS0 0x00FF0000
#define MAMMIT_SF_EQUL 0x00010000

#define MAMMIT_SF_OPS1 0xFF000000
#define MAMMIT_SF_SETR 0x00010000

// (...)

// constants
#ifndef MAMMIT_TK_WIDTH
    #define MAMMIT_TK_WIDTH 64

#endif

#ifndef MAMMIT_TK_COUNT
    #define MAMMIT_TK_COUNT 16

#endif

//   ---     ---     ---     ---     ---

static uchar  tokens[MAMMIT_TK_COUNT][MAMMIT_TK_WIDTH];

static uchar* rd_tk   = tokens[0];          // components of current statement
static uint   rd_tki  = 0;                  // num of tokens
static uint   rd_tkp  = 0;                  // token char idex
static uint   rd_tkx  = 0;                  // (iter) current token idex

static uchar* rd_buff = NULL;               // char buffer containing statements

static uchar  rd_cur  = '\0';               // current char in buff
static uchar  rd_prv  = '\0';               // previous char in buff
static uchar  rd_nxt  = '\0';               // next char in buff

static ushort rd_wid  = 0x0000;             // next | (cur<<8)
static uint   rd_pos  = 0;                  // position into buffer

//   ---     ---     ---     ---     ---

typedef struct MAMM_TYPEVAL_DATA {          // to simplify reading wacky types

    uchar base[MAMMIT_TK_WIDTH];            // type-name

    union {
        struct {
            uchar indlvl;                   // indirection level (ptr depth!)
            uchar arrsize;                  // >1 equals pow(2, arrsize), else 1
            uchar flags;                    // static, const, unsigned...
            uchar pad;                      // idk what to do with the last byte

        };  uint  F;
    };

} TYPEDATA; static TYPEDATA typedata = {0};

//   ---     ---     ---     ---     ---

void UPKTYPE(uchar* typeval)                {

    CLMEM2(                                 // clean-up leftovers from previous use
        (void*) &typedata,
        sizeof(TYPEDATA)

    );

    uchar* base = typedata.base+0;          // point to our static block of chars
    uint   len  = strlen(typeval);          // length of original string

    ushort i    = 0;                        // offset into original string
    ushort j    = 0;                        // offset into __target__ string

    uchar  c    = 0x00;                     // character currently being pointed at

//   ---     ---     ---     ---     ---

    TOP:                                    // read through original string and decompose

        c=typeval[i];                       // get next char

        if((c >= 0x30) && (0x39 >= c)) {    // if char is number
            typedata.arrsize*=10;           // left-shift
            typedata.arrsize+=c-0x30;       // add unit
            goto BOT;                       // skip to end

        }; if(j || i>2) { goto MID; };      // skip if char > original[0-2]

//   ---     ---     ---     ---     ---

    switch(c) {

        case 0x7A:                          // z, zzzzstatic (lots o' volts)
            typedata.flags|=0x01; goto BOT;
        case 0x71:                          // q, qqqqconstant
            typedata.flags|=0x02; goto BOT;
        case 0x75:                          // u, uuuuunsigned
            typedata.flags|=0x04; goto BOT;

        default:
            break;

    };

//   ---     ---     ---     ---     ---

MID:switch(c) {

        case 0x2A:                          // ptr to ptr to ptr to ptr to ptpr to what
            typedata.indlvl++; break;

        default:                            // everything else goes
            base[j]=c; j++; break;

    }; BOT: i++; if(i<len) { goto TOP; }

CALOUT(E,"\
Base: %s\n\
Size: %u\n\
Astr: %u\n\
Usig: %u\n",

typedata.base,
typedata.arrsize,
typedata.indlvl,
typedata.flags&0x04

);

                                                    };

/*

sh SHT_
st TRK_

v  VO__
n  NHL_

c  CHR_
i  INT_
l  LNG_

f  FLT_
d  DBL_

*/


//   ---     ---     ---     ---     ---

typedef struct MAMM_SYNTX_SYMBOL {          // used for string to funcall mambo

    ID    id;                               // polyheader, makes this block hashable
    NIHIL onrd;                             // links block with a given funcall

} SYMBOL; SYMBOL SYMNEW(uchar* fam,
                        uchar* key,
                        NIHIL  fun)         {

    SYMBOL sym = {0};                       // simple 'constructor', so to speak

    sym.id     = IDNEW(fam, key);           // fill out the id
    sym.onrd   = fun;                       // when !fun, you're bored and null

    return sym;                                                                             };

//   ---     ---     ---     ---     ---

typedef struct MAMM_WIDE_LVAL {             // a big box for values

    ID    id;                               // identifier
    uchar box[32];                          // put your stuff here

} LVAL;                                     // now let's explain: ID is 32 bytes,
                                            // +32 bytes from box, you have 64
                                            // that's enough for most common uses
                                            // if you have a bigger value (or an array... )
                                            // then you'll simply take up the next slot
                                            // so, read next 64 bytes as (u)chars
                                            // the problem with this is, of course, ABSPITA
                                            // but we're willing to make sacrifices

//   ---     ---     ---     ---     ---

typedef struct MAMM_LVAL_ARRSLOT {          // a block of big boxes

    uint start;                             // offset to first box
    uint end;                               // offset to last box

    uint bytesize;                          // total size of the read
    uint pad;                               // manual padding

} LARS;

//   ---     ---     ---     ---     ---

LVAL VALNEW(uchar* type,
            uchar* name,
            uchar* val ,

            uint   size,

            LARS*  lars,
            LVAL*  slot)                    {

    LVAL v         = {0};
    slot->id       = IDNEW(type, name);

    uint len       = 1;                     // how many slots this value takes up
    uint off       = 0;                     // offset in bytes into val, from first slot

    lars->bytesize = size;                  // let lars know how big val is

//   ---     ---     ---     ---     ---

    if(size>32) {                           // bigger values take up more slots

        for(uint x=0; x<32; x++) {          // fill out the first box
            slot->box[x]=val[x];

        }; off=32;

        uint left=size-32;                  // discount the first box
        while(left) {                       // maybe we could do without loop: LAZYSOL
            uint nx = (64>left) ? left : 64;

            LVAL* box=(uchar*) slot+len;    // use slot as a byte array
            for(uint x=0; x<nx; x++) {      // walk through it and copy from val
                 box[x]=val[off+x];

            }; left-=nx; len++; off+=nx;    // move to next slot
        };
    }

//   ---     ---     ---     ---     ---

    else {
        for(uint x=0; x<size; x++) {        // fill out the box...
            slot->box[x]=val[x];

        };
    }; lars->end=lars->start+len;           /* let lars know how many slots to read */      };

//   ---     ---     ---     ---     ---

#define GNAMES_SZ 1024

typedef struct MAMM_INTERPRETER {           // smach for pe-text input

    MEM m;                                  // mem header

    union {                                 // state, subdivided
        struct {
            uchar ctrl;                     // control chars affect reading
            uchar cntx;                     // context symbols affect interpreting
            uchar lvla;                     // x depth into context
            uchar lvlb;                     // y depth into statement or expression

        };  uint  state;                    // ^all four as one uint

    };

    uchar  lvla_stack[256];                 // markers for recalling previous context

    uint   larstop;                         // next offset @lvalues that's free
    LARS   lvalsl    [256];                 // yer vars kypr
    LVAL   lvalues   [256];                 // yer vars arrrr

    SYMBOL slots[GNAMES_SZ];                // array of built-ins
    STK    slstack;                         // stack of (free)indices into built-ins array

} MAMMIT; MAMMIT* mammi;

//   ---     ---     ---     ---     ---

#define MAMMIT_LVLA_NXT { mammi->lvla_stack[mammi->lvla]=mammi->cntx; mammi->lvla++; }
#define MAMMIT_LVLA_PRV { mammi->lvla++; mammi->cntx=mammi->lvla_stack[mammi->lvla]; }

//   ---     ---     ---     ---     ---

void REGMA(void)                            {
    if(!(mammi->state&MAMMIT_SF_CREG)) {    // if unset, do and ret
        mammi->state |= MAMMIT_SF_CREG;     // set state

        rd_tkx++; uchar* name = tokens[rd_tkx];
        CALOUT(K, "reg %s\n", name);

        return;

    }; mammi->state &=~MAMMIT_SF_CREG;      /* effectively, an implicit else */             };

void REGTP(void)                            {

    uchar* type = typedata.base;  rd_tkx++; // fetch, move to next token
    uchar* name = tokens[rd_tkx]; rd_tkx++; // fetch, move moar

    CALOUT(K, "%s %s", type, name);
    if(rd_tkx<rd_tki) {
        CALOUT(K, " %s\n", tokens[rd_tkx]);

        // pop next, so to speak
        // just get slot from idex and set start
        uint  off        = mammi->larstop;
        LARS* lars       = mammi->lvalsl+off;
        LVAL* slot       = mammi->lvalues+off;
        lars->start      = mammi->larstop;

        // set aside some memory, measure...
        uchar* value     = "\x00\x00\x00\x00";
        uchar* raw_value = tokens[rd_tkx];
        uint   len       = strlen(raw_value);

//   ---     ---     ---     ---     ---    STRING->INTEGER

        if( (0x30 <= raw_value[0])
        &&  (0x39 >= raw_value[0]) {
            if(len>1) {                     // go through string...
                uchar c=raw_value[0];       // try not to die before the gate opens

                if(c==0x30) {

                    // is hexlit
                    if  (raw_value[1] == 0x78) {
                        goto RD_ASHEXN;
                    }

                    // is bitlit
                    elif(raw_value[1] == 0x62) {
                        goto RD_ASBITS;

                    };

                    return;                 // zero in front and it's not a lit? insolence!

//   ---     ---     ---     ---     ---

                    RD_ASHEXN:              // string -> hex

                    raw_value += 2;         // skip the 0x
                    uint cbyte = 0;         // current hex digit
                    uint hxval = 0;         // value of char, in hex

                    do { c=*raw_value;      // redundant deref for shorts

                        if(c<=0x39) {       // if 0-9
                            hxval=c-0x30;

                        }
//ihatehadoukens
                        elif( (0x41<=c)
                        &&    (0x46>=c) ) { // if A-F
                            hxval=c-0x37;
// cleanup this rubbish
                        }; if(cbyte%2) {    // is first digit
                            value[cbyte/2]  = hxval;
// or im gonna be mad
                        } else {            // is second digit
                            value[cbyte/2] += hxval*16;

                        }; cbyte++;         // move to next
                    } while(*raw_value++); goto DONE;

                }

                else {
                    

                }

            }

//   ---     ---     ---     ---     ---

            else {                          // boring corner case: single digit
                value[0]=raw_value[0]-0x30;

            };

        };

//   ---     ---     ---     ---     ---

        DONE: VALNEW(type, name, val, size, lars, slot);

    };

                                                                                };

//   ---     ---     ---     ---     ---

static HASH* GNAMES_HASH;

void NTNAMES(void)                          {

                                            // interpreter nit
    ID id = IDNEW                           ("MAMM", "I"                               );
    MEMGET                                  (MAMMIT, mammi, GNAMES_SZ*sizeof(uint), &id);
    MKSTK                                   (byref(mammi->slstack), GNAMES_SZ          );

    for(int x=GNAMES_SZ-1; x>0; x--) {      // fill stack with indices
        STACKPUSH(byref(mammi->slstack), x);

                                            // nit the hash
    }; GNAMES_HASH = MKHASH                 (7, "gnames_hash"                          );

//   ---     ---     ---     ---     ---

    char* base_dtypes[]={                   // basic types

        "void" ,
        "stark",
        "nihil",

        "char" ,
        "short",
        "int"  ,
        "long" ,

        "float",
        "double"

    };

    for(uint x=0, y=0; x<ARRSIZE(base_dtypes); x++) {

                                            // insert basic types into table
        STACKPOP                            (byref(mammi->slstack), y                  );
        mammi->slots[y] = SYMNEW            ("TYPE", base_dtypes[x], REGTP             );
        HASHSET                             (GNAMES_HASH, byref(mammi->slots[y].id)    );

    };

//   ---     ---     ---     ---     ---

    SYMBOL contexts[]={                     // names of valid block-types

        SYMNEW("CNTX", "reg",  REGMA),
        SYMNEW("CNTX", "hed",  NULL ),
        SYMNEW("CNTX", "src",  NULL ),

        SYMNEW("CNTX", "defn", NULL ),
        SYMNEW("CNTX", "decl", NULL ),
        SYMNEW("CNTX", "clan", NULL ),
        SYMNEW("CNTX", "func", NULL )

    };

    for(uint x=0, y=0; x<ARRSIZE(contexts); x++) {

                                            // get next slot idex
        STACKPOP                            (byref(mammi->slstack), y                  );

        mammi->slots[y] = contexts[x];      // copy data to array and insert in lkp table
        HASHSET                             (GNAMES_HASH, byref(mammi->slots[y].id)    );

    };                                                                                      };

void DLNAMES(void)                          { DLMEM(GNAMES_HASH); DLMEM(mammi);             };

//   ---     ---     ---     ---     ---

void CHKTKNS(void)                          {

    uchar* SEQ[MAMMIT_TK_COUNT];

    uchar* SEQN;
    uint   SEQI;

//   ---     ---     ---     ---     ---

    if(!(mammi->state&MAMMIT_SF_CNTX)) {    // no current context

        SEQN   = "CNON";
        SEQ[0] = "CNTX\x01";
        SEQI   = 1;

    }

//   ---     ---     ---     ---     ---

    elif(mammi->state&MAMMIT_SF_CREG) {     // context == reg

        SEQN   = "CREG";
        SEQ[0] = "TYPE\x02";
        SEQ[1] = "<OP>\x01";
        SEQ[2] = "EXPR\xFF";
        SEQI   = 1;

    };

//   ---     ---     ---     ---     ---

    if(SEQI > (rd_tki)) {
        CALOUT(E, "Invalid token sequence number for context %s\n", SEQN);
        return;

    };

//   ---     ---     ---     ---     ---

    for(rd_tkx=0; rd_tkx<rd_tki; rd_tkx++) {

        uchar seq_k[5];                     // symbol-type key used as filter into table
        uchar seq_i;                        // number of inputs taken by symbol

        for(uint y=0; y<4; y++) {           // unpack SEQ into k and i 
            seq_k[y]=SEQ[rd_tkx][y];

        }; seq_k[4]='\0';                   // put that nullterm there
        seq_i=SEQ[rd_tkx][4];               // fetch argcount

//   ---     ---     ---     ---     ---

        int       valid = 1;
        SYMBOL*   sym   = NULL;

        if(strcmp(seq_k, "NAME")) {         // names can be anything, live the danger

            void* nulmy = NULL;             // non-naming tokens can carry funcalls
                                            // they must exist within our gnames table, so
                                            // in-invoking any one of them we must fetch
                                            // strictly by key, since strings is all we have
                                            // wait, wat? embedding commands in text? in C?
                                            // well, who would've thought...

//   ---     ---     ---     ---     ---

            uchar key[MAMMIT_TK_WIDTH];     // fetch this from table

            uint  len=strlen(
                    tokens[rd_tkx]

            );                              // just so I don't have to do {cpy} while(*t++)

//   ---     ---     ---     ---     ---

            if(!strcmp(seq_k, "TYPE")) {    // corner case: type flags, arrays, pointers

                UPKTYPE(tokens[rd_tkx]);    // decompose type descriptor

                uint x;                     // like a fashion designer, take measures ;>
                len = strlen                (typedata.base                             );

                for(x=0; x<len; x++) {      // now copy
                    key[x]=typedata.base[x];// key == base typename

                }; key[x]=0x00;             // put the nullterm there...
            }

            else { uint x;
                for(x=0; x<len; x++) {      // now copy
                    key[x]=tokens[rd_tkx][x];// key == base typename

                }; key[x]=0x00;

            };

//   ---     ---     ---     ---     ---

                                            // fetch from table __by_key__ (slowest)
            STR_HASHGET                     (GNAMES_HASH, key, nulmy, 0                );

            valid = nulmy!=NULL;            // success on fetch means we might have to
            if(valid) {                     // modify mammit state based on symbols read
                sym = (SYMBOL*) nulmy;      // no sneaky funcalls on invalid input, please

        }};                                 // now hope you didn't make any silly mistakes

//   ---     ---     ---     ---     ---

        if(valid) {                         // debugger's falacy:
                                            // "if it prints something, it does something!"
            CALOUT                          (K, "%u: %s %s\n",                         \
                                             rd_tkx, seq_k, tokens[rd_tkx]             );

            if(sym) { if(sym->onrd) {       // this is why I want if x then y syntax
                sym->onrd();

            }};
        }

        else {                              // very much the same as above
            CALOUT                          (K, "%u: INVALID %s: '%s'\n",              \
                                             rd_tkx, seq_k, tokens[rd_tkx]             );

    }};                                                                                     };

//   ---     ---     ---     ---     ---

void RDNXT(void)                            {

    // operators left to write cases for...
    // | ^ \ < > * - % + . , ; ( ) [ ] { } / & : = @ $ ? ! # ~ ' "

    TOP:
        rd_prv=rd_cur;                      // remember current
        rd_cur=rd_buff[rd_pos];             // advance to next

        rd_pos++; rd_nxt=rd_buff[rd_pos];
        rd_wid = rd_nxt | (rd_cur<<8);

//   ---     ---     ---     ---     ---

    switch(rd_wid) {                        // check for special wide chars

        case 0x0000: return;                // double nullterm should never happen

        case 0x243A:
            mammi->state |= MAMMIT_SF_PESC;
            rd_tk[rd_tkp]=rd_cur; rd_tkp++; rd_pos++; goto TOP;

        case 0x3B3E:
            mammi->state &=~MAMMIT_SF_PESC; rd_pos++; goto TOP;

        case 0x3D3D:
            mammi->state |= MAMMIT_SF_EQUL; rd_pos++; goto TOP;

    };

    switch(rd_cur) {

//   ---     ---     ---     ---     ---    BLANKS 0-15

        case 0x00: return;                  // end on nullterm \0

        case 0x01: goto APTOK;              // skip control chars
        case 0x02: goto APTOK;              // some or all of these could be used in the future
        case 0x03: goto APTOK;              // for now they are meaningless, so skip all
        case 0x04: goto APTOK;              // keeping the cases for when the day comes
        case 0x05: goto APTOK;
        case 0x06: goto APTOK;
        case 0x07: goto APTOK;
        case 0x08: goto APTOK;
        case 0x09: goto APTOK;
        case 0x0A: goto APTOK;
        case 0x0B: goto APTOK;
        case 0x0C: goto APTOK;
        case 0x0D: goto APTOK;
        case 0x0E: goto APTOK;
        case 0x0F: goto APTOK;

//   ---     ---     ---     ---     ---    BLANKS 16-31

        case 0x10: goto APTOK;              // same as above, meaningless for now
        case 0x11: goto APTOK;              // reason why they're here, above as well
        case 0x12: goto APTOK;
        case 0x13: goto APTOK;
        case 0x14: goto APTOK;
        case 0x15: goto APTOK;
        case 0x16: goto APTOK;
        case 0x17: goto APTOK;
        case 0x18: goto APTOK;
        case 0x19: goto APTOK;
        case 0x1A: goto APTOK;
        case 0x1B: goto APTOK;
        case 0x1C: goto APTOK;
        case 0x1D: goto APTOK;
        case 0x1E: goto APTOK;
        case 0x1F: goto APTOK;

//   ---     ---     ---     ---     ---    BLANK 32

        case 0x20: APTOK:                   // for space and skipped (see above x2)

            if(!rd_tkp) { break; }

            if(!(  0x01 <= rd_prv \
                && 0x20 >= rd_prv ) ) {     // if previous is not *also* control char {1,32}

                rd_tkp = 0; rd_tki++;       // reset position && advance slot
                rd_tk  = tokens[rd_tki];    // push token

            }; break;

//   ---     ---     ---     ---     ---    OPERATORS

        case 0x3D:
            mammi->state |= MAMMIT_SF_SETR;
            goto APTOK;

//   ---     ---     ---     ---     ---    TERMINATORS

        case 0x7B: MAMMIT_LVLA_NXT
            goto PROCST;

        case 0x7D: MAMMIT_LVLA_PRV
            goto PROCST;

        case 0x3B: PROCST:

            if(rd_tkp) {                    // >only when theres chars left in slot
                rd_tkp = 0; rd_tki++;       // reset position && advance slot
                rd_tk  = tokens[rd_tki];    // push token

            }; if(!rd_tki) { break; }       // no tokens, no checking

            CHKTKNS();                      // process token sequence and clean up

            for(uint i=0; i<MAMMIT_TK_COUNT; i++) {
                CLMEM2(tokens[i], MAMMIT_TK_WIDTH);

            }; rd_tkp=0; rd_tki=0;
            rd_tk=tokens[rd_tki]; break;

//   ---     ---     ---     ---     ---    CHARACTERS

        default:                            // 'cat' char to cur token
            rd_tk[rd_tkp]=rd_cur; rd_tkp++; break;

    }; if(rd_nxt) { goto TOP; };                                                            };

//   ---     ---     ---     ---     ---

int main(void)                              {

    NTNAMES();
    MEM* s=MKSTR("MAMM_RD", 1024, 1); CLMEM(s);

    RPSTR(&s, "reg vars { int x=1; }", 0);
    rd_buff = MEMBUFF(s, uchar, 0);

    RDNXT();
    DLMEM(s);
    DLNAMES();

    return 0;

};

//   ---     ---     ---     ---     ---