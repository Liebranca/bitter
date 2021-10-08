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
#include <inttypes.h>

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

//   ---     ---     ---     ---     ---
// debug/errcatch stuff

static uint gblevil =  0x00000000;          // global evilstate
static uint rd_line =  1;                   // current line

#define MAMMIT_EV_BEG  0x00010000
#define MAMMIT_EV_DECL 0x00000001
#define MAMMIT_EV_VSIZ 0x00000002

// ommited __shpath(__FILE__)
#define MAMMLOC "TEST", "NON", rd_line
#define GTMAMMLOC __geterrloc(MAMMLOC)

// mammi-oriented errcatch
#define MAMMCTCH(func, retx, errcode, info) { DANG* cal = GTMAMMLOC;                         \
                                                                                             \
    cal->state            = func;                                                            \
    if  ( (cal->state    == FATAL        )                                                   \
        | (cal->state    == ERROR        )                                                   \
        | (retx && (retx != cal->state)) ) {                                                 \
                                                                                             \
            gblevil |= errcode;                                                              \
            __terminator(MAMMIT_EV_BEG+errcode, info); return;                               \
                                                                                             \
    }; retx = cal->state; __popcalreg();                                                     }

//   ---     ---     ---     ---     ---
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

static HASH*  GNAMES_HASH;                  // globals and built-ins
static HASH*  LNAMES_HASH;                  // user-defined symbols

static MEM*   memlng  = NULL;               // big block of memory

//   ---     ---     ---     ---     ---

static uint   rd_cast = 0;                  // for "dynamic" type-casting;
                                            // actually, just cases in a switch

                                            // 0x00 void
                                            // 0x01 nihil
                                            // 0x02 stark

                                            // 0x03 int8_t
                                            // 0x04 int16_t
                                            // 0x05 int32_t
                                            // 0x06 int64_t

                                            // 0x07-0A ^unsigned idem...

                                            // 0x0B float

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

/*
CALOUT(E,"\
base: %s\n\
size: %u\n\
astr: %u\n\
usig: %u\n",

typedata.base,
typedata.arrsize,
typedata.indlvl,

(typedata.flags&0x04)!=0

);*/

                                                                                            };

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

            uchar* box=(uchar*) slot+len;   // use slot as a byte array
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
    }; lars->end=lars->start+len;           // let lars know how many slots to read

                                            // insert in hash for later fetch by key
    HASHSET                                 (LNAMES_HASH, byref(slot->id)         );        };

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
    uchar  lvlb_stack[256];                 // ^idem, for prev start of expression

    uint   larstop;                         // next offset @lvalues that's free
    LARS   lvalsl    [256];                 // yer vars kypr
    LVAL   lvalues   [256];                 // yer vars arrrr

    SYMBOL slots[GNAMES_SZ];                // array of built-ins
    STK    slstack;                         // stack of (free)indices into built-ins array

} MAMMIT; MAMMIT* mammi;

//   ---     ---     ---     ---     ---

#define MAMMIT_LVLA_NXT { mammi->lvla_stack[mammi->lvla]=mammi->cntx; mammi->lvla++;    }
#define MAMMIT_LVLA_PRV { mammi->lvla--; mammi->cntx=mammi->lvla_stack[mammi->lvla];    }

//   ---     ---     ---     ---     ---

#define MAMMIT_LVLB_NXT {                                                                   \
    mammi->lvlb_stack[mammi->lvlb]=flags;                                                   \
    mammi->lvlb++; flags=0;                                                                 \
    lhand+=size; value+=size;                                                               }

#define MAMMIT_LVLB_PRV {                                                                   \
    mammi->lvlb--; flags=mammi->lvlb_stack[mammi->lvlb];                                    \
    lhand-=size; value-=size;                                                               }

//   ---     ---     ---     ---     ---

void REGTP(void);                           // fwd decl

void REGMA(void)                            {
    if(!(mammi->state&MAMMIT_SF_CREG)) {    // if unset, do and ret
        mammi->state |= MAMMIT_SF_CREG;     // set state

        rd_tkx++; uchar* name = tokens[rd_tkx];
        CALOUT(K, "reg %s\n", name);

        return;

    }; mammi->state &=~MAMMIT_SF_CREG;      // effectively, an implicit else
                                                                                            };

//   ---     ---     ---     ---     ---

int NOREDCL(uchar* name)                    {

    void* nulmy = NULL;                     // check if name exists in global scope
    STR_HASHGET                             (GNAMES_HASH, name, nulmy, 0);

    if(nulmy) {                             // freak out if it does;
        return ERROR;

                                            // now check if name exists in *local* scope
    }; STR_HASHGET                          (LNAMES_HASH, name, nulmy, 0);

    if(nulmy) {                             // ... and freak out if it does
        return ERROR;

    }; return DONE;                                                                         };

int NOOVERSZ(uint sz, uint f)               { if(sz%f) { return ERROR; }; return DONE;      };

//   ---     ---     ---     ---     ---

// these funcs below? lazy nihil way to identify type
// without having to do a strcmp or str_hashget...

// it IS a little silly to hardcode it like this
// BUT: aren't base types HARDCODED to BEG with?

// the initial hash-gotten symbol can redirect
// to a single function, that function KNOWS
// how to interpret the data; this is simpler

// repetitive? yes
// does it work? yes
// optimal? depends

// its either this or using arguments
// AND yet more branches per call, mind you
// so no thanks, this mess will do

//   ---     ---     ---     ---     ---    void, nihil, stark

void REGVOI(void)                           { rd_cast = 0x00; REGTP();                      };
void REGNHL(void)                           { rd_cast = 0x01; REGTP();                      };
void REGTRK(void)                           { rd_cast = 0x02; REGTP();                      };

//   ---     ---     ---     ---     ---    char

void REGCHR(void)                           {

                                            // int(bitsize) type + is_unsigned
    rd_cast =                                 0x03                              \
            +                               ( 0x04 * ((typedata.flags&0x04)!=0) );

                                            // evaluate the expression;
    REGTP                                   (                                   );          };

//   ---     ---     ---     ---     ---    short, same^

void REGSHR(void)                           {
    rd_cast =                                 0x04                              \
            +                               ( 0x04 * ((typedata.flags&0x04)!=0) );
    REGTP                                   (                                   );          };

//   ---     ---     ---     ---     ---    int, same^

void REGINT(void)                           {
    rd_cast =                                 0x05                              \
            +                               ( 0x04 * ((typedata.flags&0x04)!=0) );
    REGTP                                   (                                   );          };

//   ---     ---     ---     ---     ---    long, same^

void REGLNG(void)                           {
    rd_cast =                                 0x06                              \
            +                               ( 0x04 * ((typedata.flags&0x04)!=0) );
    REGTP                                   (                                   );          };

//   ---     ---     ---     ---     ---    float

void REGFLT(void)                           { rd_cast = 0x0B; REGTP();                      };

//   ---     ---     ---     ---     ---

void TRHEXVAL(uchar* src ,
              uchar* to  ,
              uint   size)                  {

    uint  cbyte = 0;                        // curent byte
    uint  chxd  = 0;                        // current hex digit
    uint  hxval = 0;                        // value of char, in hex

    uchar c     = 0x00;                     // empty char

//   ---     ---     ---     ---     ---

    do { c=*src;                            // redundant deref for shorts

        if(c<=0x39) {                       // if 0-9
            hxval=c-0x30;

        }

        elif( (0x41<=c)
        &&    (0x46>=c) ) {                 // if A-F
            hxval=c-0x37;

        }

        else {                              // nuuuuuuuuuuuuuuuull!
            break;

        };

//   ---     ---     ---     ---     ---

        if(!chxd) {                         // is first digit
            to[cbyte]  = hxval;
            chxd++;

        } else {                            // is second digit
            to[cbyte] += hxval*16;
            chxd--; cbyte++;

    }} while(*src-- != 0x78);                                                               };

//   ---     ---     ---     ---     ---

void TRBITVAL(uchar* src ,
              uchar* to  ,
              uint   size)                  {

    uint  cbit  = 0;                        // current bit
    uint  cbyte = 0;                        // curent byte

    uchar c     = 0x00;                     // empty char

//   ---     ---     ---     ---     ---

    do { c=*src;                            // redundant deref for shorts

        if(c != 0x31\
        && c != 0x30) {                     // nuuuuuuuuuuull!
            break;

        }; to[cbyte] |= (c==0x31) << cbit;  // easy money

//   ---     ---     ---     ---     ---

        if(cbit==7) {                       // if all bits set move to next byte
            cbyte++; cbit=0;                // ... and go back to first bit!
            continue;

        }; cbit++;                          // else go to next bit

    } while(*src-- != 0x62);                                                                };

//   ---     ---     ---     ---     ---

void TRDECVAL(uchar* src ,
              uchar* to  ,
              uint   size)                  {

    uint decval = 0;                        // value in decimal
    uchar c     = 0x00;                     // empty char

//   ---     ---     ---     ---     ---

    do { c=*src;                            // redundant deref for shorts
        if(!c) { break; }                   // lazy while

        decval *= 10;                       // left shift
        decval += c - 0x30;

    } while(*src++);

//   ---     ---     ---     ---     ---

    for(uint x=0; x<size; x++) {            // copy bytes over
        to[x]=(decval&(0xFF<<(x*8))) >> (x*8);

    };                                                                                      };

//   ---     ---     ---     ---     ---

void TRFLTVAL(uchar* src ,
              uchar* to  ,
              uint   size)                  {

    float  whole = 0.0f;                    // integer portion of number
    float  fract = 0.0f;                    // fraction portion of number
    float  fval  = 0.0f;                    // value at current char

    uint   dotd  = 0;                       // right shift multiplier
    uchar  c     = 0x00;                    // blank char

//   ---     ---     ---     ---     ---

    do { c=*src;

        if(!c) { break; }                   // nuuuuuuuuuuull!

        fval=(float) c-0x30;                // fval be in (0,9), else is dot

        if(c==0x2E) {                       // dot spotted, do fractions now
            dotd=1;                         // start fractions at 0.1 and advance that
            continue;                       // skip to next char...

        }

//   ---     ---     ---     ---     ---

        elif(!dotd) {
            whole*=10;                      // left shift
            whole+=fval;                    // add to total

            continue;                       // skip!

        };

//   ---     ---     ---     ---     ---

        dotd  *= 10;                        // up the right shift factor
        fract += fval*(1.0f/dotd);          // right shift value and add

    } while(*src++); whole+=fract;

//   ---     ---     ---     ---     ---

    uint uval = *((uint*) &whole);          // read these bytes as an int
    for(uint x=0; x<size; x++) {            // copy them over
        to[x]=(uval&(0xFF<<(x*8))) >> (x*8);

    };                                                                                      };

//   ---     ---     ---     ---     ---

#define MAMMIT_OPSWITCH {                   \
                                            \
    if(flags&0x04) {                        \
        (*r)*=(*v); break;                  \
                                            \
    } elif(flags&0x08) {                    \
        (*r)/=(*v); break;                  \
                                            \
    }; (*r)+=(*v); break;                   }

//   ---     ---     ---     ---     ---

void REGTP(void)                            {

    uchar* type = typedata.base;  rd_tkx++; // fetch, move to next token
    uchar* name = tokens[rd_tkx];           // fetch, stay put

    uint   size = 4;

    switch(rd_cast) {                       // for 'dynamic' type-casting
                                            // we set size to sizeof x C type!
                                            // wait, you dunno whats a ulong??
                                            // look at KVRNEL/zjc_CommonTypes.h

        case 0x00: size=sizeof(void  ); break;
        case 0x01: size=sizeof(NIHIL ); break;
        case 0x02: size=sizeof(STARK ); break;

        case 0x03: size=sizeof(schar ); break;
        case 0x04: size=sizeof(sshort); break;
        case 0x05: size=sizeof(sint  ); break;
        case 0x06: size=sizeof(slong ); break;

        case 0x07: size=sizeof(uchar ); break;
        case 0x08: size=sizeof(ushort); break;
        case 0x09: size=sizeof(uint  ); break;
        case 0x0A: size=sizeof(ulong ); break;

        default  : size=sizeof(float ); break;

    };

//   ---     ---     ---     ---     ---

                                            // redeclaration block
    int    evil       = 0; MAMMCTCH         (NOREDCL(name), evil, MAMMIT_EV_DECL, name);
    CALOUT                                  (K, "DECL: %s %s", type, name             );

    uint ex_f         = rd_tkx+1;           // idex to first token in expression
    uchar* result     = (uchar*) memlng->buff+0;
    uchar* lhand      = result;

    // pop next, so to speak
    // just get slot from idex and set start
    uint  off         = mammi->larstop;
    LARS* lars        = mammi->lvalsl+off;
    LVAL* slot        = mammi->lvalues+off;
    lars->start       = mammi->larstop;

//   ---     ---     ---     ---     ---

    uint   flags      = 0;                  // 0x01 l min r
                                            // 0x02 floating point
                                            // 0x04 l mul r
                                            // 0x08 l div r

    EVAL_EXP: rd_tkx++;                     // read next token in expression
    if(!(rd_tkx<rd_tki)) { goto RESULT; }   // ... or jump to end if all tokens read

    uchar* value      = lhand+size;
    CLMEM2(value, size);

    uchar* raw_value  = tokens[rd_tkx];

    flags            &=~0x02;               // clear is_float part

//   ---     ---     ---     ---     ---

    POP_OPERATORS:                          // if operator chars in token, eval and pop them
    switch(raw_value[0]) {

        default: break;

        case 0x2B:
        case 0x28: MAMMIT_LVLB_NXT
            goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x2D:
            flags |= 0x01;

            goto POP_OPSTOP;

        case 0x2A:
            flags |= 0x04;
            flags &=~0x08;

            goto POP_OPSTOP;

        case 0x2F:
            flags &=~0x04;
            flags |= 0x08;

            goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x3D:
            // wat

//   ---     ---     ---     ---     ---

        POP_OPSTOP:
            raw_value++; goto POP_OPERATORS;

    };

//   ---     ---     ---     ---     ---

    uint   len        = strlen              (raw_value                    );

    POP_TERMINATORS:                        // same as oppies, but at end of token
    switch(raw_value[len-1]) {

        default: break;

        case 0x29: MAMMIT_LVLB_PRV
            goto POP_TESTOP;

        POP_TESTOP:
            len--; goto POP_TERMINATORS;

    };

//   ---     ---     ---     ---     ---

    uint   vlen       = len-1;
    flags            |=                     (strstr(type, "float") != NULL) << 1;

//   ---     ---     ---     ---     ---

    if( (0x30 <= raw_value[0])
    &&  (0x39 >= raw_value[0]) ) {

        if(len>1) {                         // not a single digit

            if(strstr(raw_value, ".")
            || flags&0x02           ) {     // is float
                flags|=0x02; goto RD_ASFLTP;

            }

            elif(raw_value[0]==0x30) {      // starts with a zero and is not 0.****

                if  (raw_value[1]==0x78) {  // is hexlit
                    TRHEXVAL                (raw_value+vlen, value, size        );
                }

                elif(raw_value[1]==0x62) {  // is bitlit
                    TRBITVAL                (raw_value+vlen, value, size        );

                };
            }

//   ---     ---     ---     ---     ---

            else {                          // string -> decimal
                TRDECVAL                    (raw_value, value, size             );

            }
        }

//   ---     ---     ---     ---     ---

        elif(len==1) {                      // boring corner case: single decimal digit
            if(flags&0x02) {
                goto RD_ASFLTP;

            }; value[0]=raw_value[0]-0x30;

        };
    }

//   ---     ---     ---     ---     ---

    elif(raw_value[0]==0x2E) {              // cool corner case: floats

                                            // catch incorrect data size
        RD_ASFLTP: MAMMCTCH                 (NOOVERSZ(size, sizeof(float)), evil,
                                             MAMMIT_EV_VSIZ, type               );

        TRFLTVAL                            (raw_value, value, size             );
    }

//   ---     ---     ---     ---     ---

    //elif(....) reserved for fetching value from names

//   ---     ---     ---     ---     ---

    BOT:

    if(flags&0x01) {                        // if negative, do the bit flipping

        if(flags&0x02) {                    // floats
            value[3] |= 0x80;

        }

//   ---     ---     ---     ---     ---

        else {                              // everything else

            for(uint x=0, carry=0;
                x<size; x++      ) {        // take two's
                value[x]=(~value[x]);       // flip bits
                if(!x || carry) {
                    if(value[x]==0xFF) {
                        value[x] += 1;      // overflows. add one and set carry
                        carry     = 1;
                    }

                    else {
                        value[x] += 1;      // won't overflow, so add and no carry
                        carry     = 0;

                    };
                };
            };
        };

//   ---     ---     ---     ---     ---

    };

    COL_LHAND: switch(rd_cast) {

        case 0x03: {
            schar* r = (schar*) lhand;
            schar* v = (schar*) value;
            MAMMIT_OPSWITCH;

        } case 0x07: {
            uchar* r = (uchar*) lhand;
            uchar* v = (uchar*) value;
            MAMMIT_OPSWITCH;

//   ---     ---     ---     ---     ---

        } case 0x04: {
            sshort* r = (sshort*) lhand;
            sshort* v = (sshort*) value;
            MAMMIT_OPSWITCH;

        } case 0x08: {
            ushort* r = (ushort*) lhand;
            ushort* v = (ushort*) value;
            MAMMIT_OPSWITCH;

//   ---     ---     ---     ---     ---

        } case 0x05: {
            sint* r = (sint*) lhand;
            sint* v = (sint*) value;

            MAMMIT_OPSWITCH;

        } case 0x09: {
            uint* r = (uint*) lhand;
            uint* v = (uint*) value;
            MAMMIT_OPSWITCH;

//   ---     ---     ---     ---     ---

        } case 0x06: {
            slong* r = (slong*) lhand;
            slong* v = (slong*) value;
            MAMMIT_OPSWITCH;

        } case 0x0A: {
            ulong* r = (ulong*) lhand;
            ulong* v = (ulong*) value;
            MAMMIT_OPSWITCH;

//   ---     ---     ---     ---     ---

        } default: {
            float* r = (float*) lhand;
            float* v = (float*) value;

            if(flags&0x04) {
                CALOUT(E, "%f : %f * %f | %u\n", *result, *r, *v, mammi->lvlb);

            } elif(flags&0x08) {
                CALOUT(E, "%f : %f / %f | %u\n", *result, *r, *v, mammi->lvlb);

            } else {
                CALOUT(E, "%f : %f + %f | %u\n", *result, *r, *v, mammi->lvlb);

            };

            MAMMIT_OPSWITCH;

        };
    };

    goto EVAL_EXP;

//   ---     ---     ---     ---     ---

    RESULT: if(mammi->lvlb>0) {             // collapse expression if unresolved
        MAMMIT_LVLB_PRV                     // this doesn't account for unclosed () parens
        goto COL_LHAND;                     // so beware! PE$O will not care for that mistake

    };

//   ---     ---     ---     ---     ---

    VALNEW(type, name, result, size, lars, slot);
    uchar* vtest = (uchar*) slot->box;

    switch(rd_cast) {

        case 0x00: break;                   // we'll see how to handle these later
        case 0x01: break;
        case 0x02: break;

//   ---     ---     ---     ---     ---

        case 0x03:
            CALOUT(
                K, " = \x27%c\x27 : 0x%02X (",
                *((schar*) vtest),
                *((schar*) vtest),
                tokens[rd_tkx]

            ); break;

        case 0x04:
            CALOUT(
                K, " = %" PRId16 ": 0x%" PRIX16 "(",
                *((sshort*) vtest),
                *((sshort*) vtest),
                tokens[rd_tkx]

            ); break;

        case 0x05:
            CALOUT(
                K, " = %" PRId32 ": 0x%" PRIX32 "(",
                *((sint*) vtest),
                *((sint*) vtest),
                tokens[rd_tkx]

            ); break;

        case 0x06:
            CALOUT(
                K, " = %" PRId64 ": 0x%" PRIX64 "(",
                *((slong*) vtest),
                *((slong*) vtest),
                tokens[rd_tkx]

            ); break;

//   ---     ---     ---     ---     ---

        case 0x07:
            CALOUT(
                K, " = \x27%c\x27 : 0x%02X (",
                *((uchar*) vtest),
                *((uchar*) vtest),
                tokens[rd_tkx]

            ); break;

        case 0x08:
            CALOUT(
                K, " = %" PRIu16 ": 0x%" PRIX16 "(",
                *((ushort*) vtest),
                *((ushort*) vtest),
                tokens[rd_tkx]

            ); break;

        case 0x09:
            CALOUT(
                K, " = %" PRIu32 ": 0x%" PRIX32 "(",
                *((uint*) vtest),
                *((uint*) vtest),
                tokens[rd_tkx]

            ); break;

        case 0x0A:
            CALOUT(
                K, " = %" PRIu64 ": 0x%" PRIX64 "(",
                *((ulong*) vtest),
                *((ulong*) vtest),
                tokens[rd_tkx]

            ); break;

//   ---     ---     ---     ---     ---

        default:

            CALOUT(
                K, " = %f : 0x%" PRIX32 "(",
                *((float*) vtest),
                *((uint*) vtest),
                tokens[rd_tkx]

            ); break;

    }; for(uint x=ex_f; x<rd_tki; x++) {
        CALOUT(K, "%s", tokens[x]);

    }; CALOUT(K, ")\n");                                                                    };

//   ---     ---     ---     ---     ---

void NTNAMES(void)                          {

                                            // interpreter nit
    ID id = IDNEW                           ("MAMM", "I"                               );
    MEMGET                                  (MAMMIT, mammi, GNAMES_SZ*sizeof(uint), &id);
    MKSTK                                   (byref(mammi->slstack), GNAMES_SZ          );

    for(int x=GNAMES_SZ-1; x>0; x--) {      // fill stack with indices
        STACKPUSH(byref(mammi->slstack), x);

                                            // nit the hashes
    }; GNAMES_HASH = MKHASH                 (7, "gnames_hash"                          );
       LNAMES_HASH = MKHASH                 (5, "lnames_hash"                          );

//   ---     ---     ---     ---     ---


    SYMBOL base_dtypes[]={                  // note how each type has it's own lazy call ;>

        SYMNEW("TYPE", "void",   REGVOI),
        SYMNEW("TYPE", "stark",  REGTRK),
        SYMNEW("TYPE", "nihil",  REGNHL),

        SYMNEW("TYPE", "char",   REGCHR),
        SYMNEW("TYPE", "short",  REGSHR),
        SYMNEW("TYPE", "int",    REGINT),
        SYMNEW("TYPE", "long",   REGLNG),

        SYMNEW("TYPE", "float",  REGFLT)

    };

    for(uint x=0, y=0; x<ARRSIZE(base_dtypes); x++) {

                                            // insert basic types into table
        STACKPOP                            (byref(mammi->slstack), y                  );

        mammi->slots[y] = base_dtypes[x];   // copy data to array and insert in lkp table
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

void DLNAMES(void)                          { DLMEM(LNAMES_HASH);                           \
                                              DLMEM(GNAMES_HASH); DLMEM(mammi);             };

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
                for(x=0; x<len; x++) {       // now copy
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
            /*CALOUT                          (K, "%u: %s %s\n",                         \
                                             rd_tkx, seq_k, tokens[rd_tkx]             );*/

            if(sym) { if(sym->onrd) {       // this is why I want if x then y syntax
                sym->onrd();

            }};
        }

        else {                              // very much the same as above
            /*CALOUT                          (K, "%u: INVALID %s: '%s'\n",              \
                                             rd_tkx, seq_k, tokens[rd_tkx]             );*/

    }};                                                                                     };

//   ---     ---     ---     ---     ---

void RDNXT(void)                            {

    uchar op[16]; CLMEM2(op, 16);           // operator storage ;>
    uint  opi=0;                            // idex into opstor

    TOP:

        if(gblevil) { return; }             // something went wrong last pass

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

        case 0x0A: rd_line++; goto APTOK;   // inc line counter on \n ;)

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

//   ---     ---     ---     ---     ---    OPERATORS L

        case 0x21:
        case 0x22:
        case 0x23:

        case 0x24:                          // handle str/iter here...

        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:

        case 0x2A:                          // handle pointers here...
        case 0x2B:
        case 0x2C:
        case 0x2D:
        case 0x2F:
        case 0x3A:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:

        case 0x40:                          // handle @idex here...

        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x5E:

        case 0x7C:
        case 0x7E:
            op[opi]=rd_cur; opi++;
            goto APTOK;

//   ---     ---     ---     ---     ---    OPERATORS R

        case 0x29:
            rd_tk[rd_tkp]=rd_cur; rd_tkp++;
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
            rd_tk=tokens[rd_tki];

            // clean non-context stateflags
            // ruins multi-statement peso escapes, ill fix that later
            mammi->state &=~MAMMIT_SF_PESO;

            break;

//   ---     ---     ---     ---     ---    CHARACTERS

        default:                            // 'cat' char to cur token

            // prefix token with operator(s)
            if(opi) { uint y=0; while(op[y]) {
                rd_tk[rd_tkp]=op[y]; rd_tkp++;
                op[y]=0x00; y++;

            } opi=0; }; rd_tk[rd_tkp]=rd_cur; rd_tkp++; break;

    }; if(rd_nxt) { goto TOP; };                                                            };

//   ---     ---     ---     ---     ---

int main(void)                              {

    NTNAMES();
    MEM* s  = MKSTR("MAMM_RD", 1024, 1); CLMEM(s);
    LDLNG(ZJC_DAFPAGE); memlng = GTLNG(); CLMEM(memlng);

    RPSTR(&s, "reg vars {\n float x=2+2*3;\n}\n", 0);
    rd_buff = MEMBUFF(s, uchar, 0);

    CALOUT(E, "\e[38;2;128;255;128m\n$PEIN:\n%s\n\e[0m\e[38;2;255;128;128m$OUT:", rd_buff);

    RDNXT();
    CALOUT(E, "\e[0m");

    DLMEM(memlng);
    DLMEM(s);
    DLNAMES();

    return 0;

};

//   ---     ---     ---     ---     ---