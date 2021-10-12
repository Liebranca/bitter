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
#include <math.h>

//   ---     ---     ---     ---     ---
// state flags...

#define MAMMIT_SF_PESO 0x000000FF

#define MAMMIT_SF_PESC 0x00000001
#define MAMMIT_SF_PLIT 0x00000002
#define MAMMIT_SF_PSEC 0x00000004

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
    #define MAMMIT_TK_COUNT 64

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
static uint   lngptr  = 0;                  // cur idex into mem, in bytes

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

    uchar  lvla_stack[256      ];           // markers for recalling previous context
    uint   lvlb_stack[256      ];           // ^idem, for prev evalstate of expression

    uint   lvaltop;                         // next offset @lvalues that's free
    uchar  lvalues   [GNAMES_SZ];           // yer vars arrrr

    SYMBOL slots     [GNAMES_SZ];           // array of built-ins
    STK    slstack;                         // stack of (free)indices into built-ins array

} MAMMIT; MAMMIT* mammi;

//   ---     ---     ---     ---     ---

void VALNEW(uchar* type,
            uchar* name,
            uchar* val ,

            uint   size)                    {

    ID id           = IDNEW(type, name);
    uchar* box      = mammi->lvalues+mammi->lvaltop;

    mammi->lvaltop += size;

    for(uint x=0; x<size; x++) {            // copy bytes over
        box[x]=val[x];
                                            // insert in hash for later fetch by key
    }; HASHSET                              (LNAMES_HASH, byref(id));                       };

//   ---     ---     ---     ---     ---

#define MAMMIT_LVLA_NXT { mammi->lvla_stack[mammi->lvla]=mammi->cntx; mammi->lvla++;    }
#define MAMMIT_LVLA_PRV { mammi->lvla--; mammi->cntx=mammi->lvla_stack[mammi->lvla];    }

//   ---     ---     ---     ---     ---

#define MAMMIT_LVLB_NXT {                                                                   \
    mammi->lvlb_stack[mammi->lvlb]=flags;                                                   \
    mammi->lvlb++; flags=0;                                                                 \
    lhand+=size; value+=size; lngptr+=size;                                                 }

#define MAMMIT_LVLB_PRV {                                                                   \
    mammi->lvlb--; flags=mammi->lvlb_stack[mammi->lvlb];                                    \
    lhand-=size; value-=size; lngptr-=size;                                                 }

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

//   ---     ---     ---     ---     ---    wide, same^

void REGWID(void)                           {
    rd_cast =                                 0x04                              \
            +                               ( 0x04 * ((typedata.flags&0x04)!=0) );
    REGTP                                   (                                   );          };

//   ---     ---     ---     ---     ---    long, same^

void REGLNG(void)                           {
    rd_cast =                                 0x05                              \
            +                               ( 0x04 * ((typedata.flags&0x04)!=0) );
    REGTP                                   (                                   );          };

//   ---     ---     ---     ---     ---    quat, same^

void REGQAT(void)                           {
    rd_cast =                                 0x06                              \
            +                               ( 0x04 * ((typedata.flags&0x04)!=0) );
    REGTP                                   (                                   );          };

//   ---     ---     ---     ---     ---    float

void REGFLT(void)                           { rd_cast = 0x0B; REGTP();                      };

//   ---     ---     ---     ---     ---

void TRHEXVAL(uchar* src ,
              uchar* to  ,
              uint   size)                  {

    uchar cbyte = 0x00;                     // curent byte
    uchar chxd  = 0x00;                     // current hex digit
    uchar hxval = 0x00;                     // value of char, in hex

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

    uchar cbit  = 0x00;                     // current bit
    uchar cbyte = 0x00;                     // curent byte
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

    ulong decval = 0x0000000000000000;      // value in decimal
    uchar c      = 0x00;                    // empty char

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

#define OP_MINUS 0x00000001
#define OP_RADIX 0x00000002
#define OP_MUL   0x00000004
#define OP_DIV   0x00000008

#define OP_GT    0x00000010
#define OP_LT    0x00000020
#define OP_EQUL  0x00000040
#define OP_EQUR  0x00000080

#define OP_BANG  0x00000100
#define OP_TILDE 0x00000200
#define OP_MONEY 0x00000400
#define OP_KUSH  0x00000800

#define OP_MODUS 0x00001000
#define OP_AMPER 0x00002000
#define OP_PIPE  0x00004000
#define OP_QUEST 0x00008000

#define OP_COLON 0x00010000
#define OP_DPIPE 0x00020000
#define OP_DAMPR 0x00040000
#define OP_XORUS 0x00080000

#define OP_LSHFT 0x00100000
#define OP_RSHFT 0x00200000
#define OP_AT    0x00400000
#define OP_POINT 0x00800000

#define OP_W_EQUR                           \
    OP_GT    | OP_LT                        \
  | OP_BANG  | OP_MODUS                     \
  | OP_AMPER | OP_XORUS                     \
  | OP_MINUS | OP_MUL                       \
  | OP_DIV   | OP_PIPE                      \
  | OP_EQUL

//   ---     ---     ---     ---     ---

#define OPSWITCH_MINUSX                     \
    if(flags&OP_MINUS) {                    \
        (*r)=-(*r);                         \
        flags&=~OP_MINUS;                   \
    }; break

#define OP_FORCEBIN(op)                     \
    (*r)=((uint)(*r))op((uint)(*v))

#define OP_FORCEUNA(op)                     \
    (*r)=op(uint)(*v)

#define CALCUS_OPSWITCH {                   \
                                            \
    switch(flags&0xFFFFFFFC) {              \
                                            \
    case OP_MUL:                            \
        (*r)*=(*v); OPSWITCH_MINUSX;        \
                                            \
    case OP_DIV:                            \
        (*r)/=(*v); OPSWITCH_MINUSX;        \
                                            \
    case OP_MODUS:                          \
        OP_FORCEBIN(%); OPSWITCH_MINUSX;    \
                                            \
    case OP_RSHFT:                          \
        OP_FORCEBIN(>>); OPSWITCH_MINUSX;   \
                                            \
    case OP_GT:                             \
        (*r)=(*r)>(*v);  OPSWITCH_MINUSX;   \
                                            \
    case OP_GT | OP_EQUR:                   \
        (*r)=(*r)>=(*v); OPSWITCH_MINUSX;   \
                                            \
    case OP_LSHFT:                          \
        OP_FORCEBIN(<<); OPSWITCH_MINUSX;   \
                                            \
    case OP_LT:                             \
        (*r)=(*r)<(*v);  OPSWITCH_MINUSX;   \
                                            \
    case OP_LT | OP_EQUR:                   \
        (*r)=(*r)<=(*v); OPSWITCH_MINUSX;   \
                                            \
    case OP_BANG:                           \
        (*r)=!(*v); OPSWITCH_MINUSX;        \
                                            \
    case OP_BANG | OP_EQUR:                 \
        (*r)=(*r)!=(*v); OPSWITCH_MINUSX;   \
                                            \
    case OP_EQUL | OP_EQUR:                 \
        (*r)=(*r)==(*v); OPSWITCH_MINUSX;   \
                                            \
    case OP_DAMPR:                          \
        OP_FORCEBIN(&&); OPSWITCH_MINUSX;   \
                                            \
    case OP_AMPER:                          \
        OP_FORCEBIN(&); OPSWITCH_MINUSX;    \
                                            \
    case OP_DPIPE:                          \
        OP_FORCEBIN(||); OPSWITCH_MINUSX;   \
                                            \
    case OP_PIPE:                           \
        OP_FORCEBIN(|); OPSWITCH_MINUSX;    \
                                            \
    case OP_XORUS:                          \
        OP_FORCEBIN(^); OPSWITCH_MINUSX;    \
                                            \
    case OP_TILDE:                          \
        OP_FORCEUNA(~); OPSWITCH_MINUSX;    \
                                            \
    default:                                \
        (*r)+=(*v); break;                  \
                                            \
    }; (*v)=0; break;                       }

//   ---     ---     ---     ---     ---

void CALCUS_COLLAPSE(uchar** lhand_ptr,
                     uchar** value_ptr,
                     uint*   flags_ptr)     {

    uchar* lhand = *lhand_ptr;
    uchar* value = *value_ptr;
    uint   flags = *flags_ptr;

    switch(rd_cast) {

        case 0x03: {
            schar* r = (schar*) lhand;
            schar* v = (schar*) value;
            CALCUS_OPSWITCH;

        } case 0x07: {
            uchar* r = (uchar*) lhand;
            uchar* v = (uchar*) value;
            CALCUS_OPSWITCH;

//   ---     ---     ---     ---     ---

        } case 0x04: {
            sshort* r = (sshort*) lhand;
            sshort* v = (sshort*) value;
            CALCUS_OPSWITCH;

        } case 0x08: {
            ushort* r = (ushort*) lhand;
            ushort* v = (ushort*) value;
            CALCUS_OPSWITCH;

//   ---     ---     ---     ---     ---

        } case 0x05: {
            sint* r = (sint*) lhand;
            sint* v = (sint*) value;

            CALCUS_OPSWITCH;

        } case 0x09: {
            uint* r = (uint*) lhand;
            uint* v = (uint*) value;
            CALCUS_OPSWITCH;

//   ---     ---     ---     ---     ---

        } case 0x06: {
            slong* r = (slong*) lhand;
            slong* v = (slong*) value;
            CALCUS_OPSWITCH;

        } case 0x0A: {
            ulong* r = (ulong*) lhand;
            ulong* v = (ulong*) value;
            CALCUS_OPSWITCH;

//   ---     ---     ---     ---     ---

        } default: {
            float* r = (float*) lhand;
            float* v = (float*) value;
            CALCUS_OPSWITCH;

        };
    };

//   ---     ---     ---     ---     ---

    *flags_ptr=flags;
    *lhand_ptr=lhand;
    *value_ptr=value;                                                                       };

//   ---     ---     ---     ---     ---

void TRNVAL(uchar* raw_value,
            uchar* value    ,
            uint*  flags_ptr,

            uint   len      ,
            uint   size     )               {

    int  evil  = 0;

    uint flags = *flags_ptr;
    uint vlen  = len-1;

                                            // set OP_RADIX
    flags |=                                (strstr(typedata.base, "float") != NULL) << 1;

//   ---     ---     ---     ---     ---


    if( (0x30 <= raw_value[0])
    &&  (0x39 >= raw_value[0]) ) {

        if(len>1) {                         // not a single digit

            if(strstr(raw_value, ".")
            || flags&OP_RADIX       ) {     // is float
                goto RD_ASFLTP;

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
            if(flags&OP_RADIX) {
                goto RD_ASFLTP;

            }; value[0]=raw_value[0]-0x30;

        };
    }

//   ---     ---     ---     ---     ---

    elif(raw_value[0]==0x2E) {              // cool corner case: floats

                                            // catch incorrect data size
        RD_ASFLTP: MAMMCTCH                 (NOOVERSZ(size, sizeof(float)), evil,
                                             MAMMIT_EV_VSIZ, typedata.base      );

        flags|=OP_RADIX;

        TRFLTVAL                            (raw_value, value, size             );
    }

//   ---     ---     ---     ---     ---

    //elif(....) reserved for fetching value from names

//   ---     ---     ---     ---     ---

    BOT:

    if(flags&OP_MINUS) {                    // if negative, do the bit flipping

        if(flags&OP_RADIX) {                // floats
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

    }; *flags_ptr=flags;                                                                    };

//   ---     ---     ---     ---     ---

void MAEXPS(uchar** raw_value,
            uchar** lhand_ptr,
            uchar** value_ptr,

            uint*   flags_ptr,
            uint    size     )              {

    uchar* lhand = *lhand_ptr;
    uchar* value = *value_ptr;
    uint   flags = *flags_ptr;

    TOP:                                    // if operator chars in token, eval and pop them
    switch((*raw_value)[0]) {

        default: break;

//   ---     ---     ---     ---     ---

        case 0x26:
            if(flags&OP_AMPER) {
                flags &=~OP_AMPER;
                flags |= OP_DAMPR;

                MAMMIT_LVLB_NXT;

                goto POP_OPSTOP;

            };  flags |= OP_AMPER;

            goto POP_OPSTOP;

        case 0x7c:
            if(flags&OP_PIPE) {
                flags &=~OP_PIPE;
                flags |= OP_DPIPE;

                MAMMIT_LVLB_NXT;

                goto POP_OPSTOP;

            };  flags |= OP_PIPE;

            goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x24:
            flags |= OP_MONEY;

            goto POP_OPSTOP;

        case 0x25:
            flags |= OP_MODUS;

            goto POP_OPSTOP;

        case 0x5e:
            flags |= OP_XORUS;

            goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x2B:
            flags &=~OP_MUL;
            flags &=~OP_DIV;

        case 0x28: MAMMIT_LVLB_NXT;
            goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x2D:
            flags |= OP_MINUS;

            goto POP_OPSTOP;

        case 0x2A:
            flags |= OP_MUL;
            flags &=~OP_DIV;

            goto POP_OPSTOP;

        case 0x2F:
            flags &=~OP_MUL;
            flags |= OP_DIV;

            goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x21: MAMMIT_LVLB_NXT;
            flags  = mammi->lvlb_stack[mammi->lvlb-1]&OP_MINUS;
            mammi->lvlb_stack[mammi->lvlb-1] &=~OP_MINUS;
            flags |= OP_BANG;
            MAMMIT_LVLB_NXT;

            goto POP_OPSTOP;

        case 0x7e: MAMMIT_LVLB_NXT;
            flags  = mammi->lvlb_stack[mammi->lvlb-1]&OP_MINUS;
            mammi->lvlb_stack[mammi->lvlb-1] &=~OP_MINUS;
            flags |= OP_TILDE;
            MAMMIT_LVLB_NXT;

            goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x3D:

            if( flags&OP_W_EQUR \
            &&  mammi->lvlb     ) {
                mammi->lvlb_stack[mammi->lvlb-1] |= OP_EQUR;

                goto POP_OPSTOP;

            };

            flags |= OP_EQUL;
            MAMMIT_LVLB_NXT;

            goto POP_OPSTOP;

//   ---     ---     ---     ---     ---

        case 0x3C:

            if(mammi->lvlb) {
                if(mammi->lvlb_stack[mammi->lvlb-1]&OP_LT) {
                    mammi->lvlb_stack[mammi->lvlb-1] &=~OP_LT;
                    mammi->lvlb_stack[mammi->lvlb-1] |= OP_LSHFT;

                    goto POP_OPSTOP;
                };
            };

            flags |= OP_LT;
            MAMMIT_LVLB_NXT;

            goto POP_OPSTOP;

        case 0x3E:

            if(mammi->lvlb) {
                if(mammi->lvlb_stack[mammi->lvlb-1]&OP_GT) {
                    mammi->lvlb_stack[mammi->lvlb-1] &=~OP_GT;
                    mammi->lvlb_stack[mammi->lvlb-1] |= OP_RSHFT;

                    goto POP_OPSTOP;
                };
            };

            flags |= OP_GT;
            MAMMIT_LVLB_NXT;

//   ---     ---     ---     ---     ---

        POP_OPSTOP:
            (*raw_value)++; goto TOP;

    };

//   ---     ---     ---     ---     ---

    uint len = strlen(*raw_value);
    if(!len) { goto END; }

    POP_TERMINATORS:                        // same as oppies, but at end of token
    switch((*raw_value)[len-1]) {

        default: break;

        case 0x29:

            if(mammi->lvlb) {
                MAMMIT_LVLB_PRV;            // lonely parens >;

            }; mammi->state &=~MAMMIT_SF_PSEC;

        POP_TESTOP:
            (*raw_value)[len-1]=0x00;
            len--; if(!len) { goto END; }
            goto POP_TERMINATORS;

    };

//   ---     ---     ---     ---     ---

                                            // translate string into value
    TRNVAL                                  (*raw_value, value, &flags, len, size);

    END:
    *flags_ptr=flags;
    *lhand_ptr=lhand;
    *value_ptr=value;                                                                       };

//   ---     ---     ---     ---     ---

#define PRCAST_CHR                         \
    CALOUT(K, " = 0x");                    \
    for(uint x=0; x<elems; x++) {          \
        CALOUT(                            \
            K, "%02X ",                    \
            *((uchar*) vtest)              \
                                           \
        ); vtest+=size; };                 \
                                           \
    CALOUT(K, "(")

#define PRCAST_WID                         \
    CALOUT(K, " = 0x");                    \
    for(uint x=0; x<elems; x++) {          \
        CALOUT(                            \
            K, "%04X ",                    \
            *((ushort*) vtest)             \
                                           \
        ); vtest+=size; };                 \
                                           \
    CALOUT(K, "(")

#define PRCAST_LNG                         \
    CALOUT(K, " = 0x");                    \
    for(uint x=0; x<elems; x++) {          \
        CALOUT(                            \
            K, "%08X ",                    \
            *((uint*) vtest)               \
                                           \
        ); vtest+=size; };                 \
                                           \
    CALOUT(K, "(")

#define PRCAST_FLT                         \
    CALOUT(K, " = 0x");                    \
    for(uint x=0; x<elems; x++) {          \
        CALOUT(                            \
            K, "%08X ",                    \
            *((float*) vtest)              \
                                           \
        ); vtest+=size; };                 \
                                           \
    CALOUT(K, "(")

#define PRCAST_QAT                         \
    CALOUT(K, " = 0x");                    \
    for(uint x=0; x<elems; x++) {          \
        CALOUT(                            \
            K, "%16X ",                    \
            *((ulong*) vtest)              \
                                           \
        ); vtest+=size; };                 \
                                           \
    CALOUT(K, "(")

#define PRCAST(T) PRCAST##_##T

//   ---     ---     ---     ---     ---

void REGTP(void)                            {

    uchar* type       = typedata.base;      // fetch
    rd_tkx++;                               // move to next

    uchar* name       = tokens[rd_tkx];     // fetch, stay put

    uint   size       = 4;                  // ensure elem count is always a power of 2
    uint   elems      =                     (uint) (pow(2, typedata.arrsize)+0.5      );

    uint   parsed     = 0;                  // how many *expressions* have been read

//   ---     ---     ---     ---     ---

    ulong szmask_a    = 0x0000000000000000;
    ulong szmask_b    = 0x0000000000000000;

    switch(rd_cast) {                       // for 'dynamic' type-casting
                                            // we set size to sizeof x C type!
                                            // wait, you dunno whats a ulong??
                                            // look at KVRNEL/zjc_CommonTypes.h

        case 0x00: szmask_a = 0xFFFFFFFFFFFFFFFF;
            size=sizeof(void* ); break;

        case 0x01:
        case 0x02: szmask_a = 0xFFFFFFFFFFFFFFFF;
                   szmask_b = 0xFFFFFFFFFFFFFFFF;

            size=sizeof(STARK ); break;

//   ---     ---     ---     ---     ---

        case 0x03:
        case 0x07: szmask_a = 0x00000000000000FF;
            size=sizeof(uchar ); break;

        case 0x04:
        case 0x08: szmask_a = 0x000000000000FFFF;
            size=sizeof(ushort); break;

        case 0x05:
        case 0x09: szmask_a = 0x00000000FFFFFFFF;
            size=sizeof(uint  ); break;

        case 0x06:
        case 0x0A: szmask_a = 0xFFFFFFFFFFFFFFFF;
            size=sizeof(ulong ); break;

//   ---     ---     ---     ---     ---

        default  : szmask_a = 0x00000000FFFFFFFF;
            size=sizeof(float ); break;

    };

//   ---     ---     ---     ---     ---

                                            // redeclaration block
    int    evil       = 0; MAMMCTCH         (NOREDCL(name), evil, MAMMIT_EV_DECL, name);
    CALOUT                                  (K, "DECL: %s %s[%u]", type, name, elems  );

    uint ex_f         = rd_tkx+1;           // idex to first token in expression
    uchar* result     = (uchar*) memlng->buff+0;
    uchar* lhand      = result;

    lngptr            = 0;

    uint  sec_beg     = lngptr;
    uint  sec_end     = elems*size;

    CLMEM2(lhand, size);

//   ---     ---     ---     ---     ---

    EVAL_EXP: rd_tkx++;                     // read next token in expression

    if( !(rd_tkx<rd_tki) \
    ||   (parsed>=elems ) ) { goto RESULT; }// ... or jump to end if all tokens read

    uint   flags      = 0;                  // values defined above MAMMIT_OPSWITCH
    uchar* raw_value  = tokens[rd_tkx];     // current token

    if(mammi->state&MAMMIT_SF_PSEC) {
        goto SECEVAL;

    };

    uchar* value      = lhand+size;
    CLMEM2(value, size);

//   ---     ---     ---     ---     ---

    if(raw_value[0]==0x2C) {

        while(mammi->lvlb) {
            MAMMIT_LVLB_PRV;
            CALCUS_COLLAPSE(&lhand, &value, &flags);

        }; parsed++;

        result += size;
        lhand   = result;
        value   = lhand+size;
        CLMEM2(value, size);

        lngptr += size;

//   ---     ---     ---     ---     ---

        raw_value++;
        if(raw_value[0]==0x28) {

            sec_beg          = lngptr;
            sec_end          = elems*size;
            raw_value++;

            SECEVAL:

            uchar* old_value = value+size;

            for(uint x=0; x<size; x++) {
                old_value[x]=value[x];

            }; mammi->state |= MAMMIT_SF_PSEC;

                                            // expand token (parse operators, fetch values)
            MAEXPS                          (&raw_value, &lhand, &value, &flags, size);

//   ---     ---     ---     ---     ---

            /*
            @(sec) syntax:

                $ lower bound                           x
                * ptr                                   x
                & upper bound

                ptr<N decrease ptr by N                 x
                ptr>N increase ptr by N                 x

                *>> move ptr to upper bound             x
                *<< move ptr to lower bound             x

                ptr#v exchange values betwen ptr && v
                ptr=v set *ptr to v
                =v    flood fill $ to Ç with v          x
                =     blank out $ to Ç                  x
                :     separate expressions              x

                */

//   ---     ---     ---     ---     ---

            uint  sflags_i = 0;
            uint* sflags   = mammi->lvlb_stack+0;
            ulong sec_val  = (*((ulong*) value)) & szmask_a;

            for(uint x=0; x<size; x++) {
                value[x]=old_value[x];

            };

//   ---     ---     ---     ---     ---

            SECTOP: switch(sflags[sflags_i]) {

                case OP_GT | OP_MONEY: sflags[sflags_i] &=~ (OP_GT | OP_MONEY);
                    if(!sec_val) {
                        sec_beg+=size;

                    } else {
                        sec_beg+=sec_val*size;

                    }; break;

                case OP_LT | OP_MONEY: sflags[sflags_i] &=~ (OP_LT | OP_MONEY);
                    if(!sec_val) {
                        sec_beg-=size;

                    } else {
                        sec_beg-=sec_val*size;

                    }; if(sec_beg>=sec_end) {
                        sec_beg=sec_end-size;

                    }; break;

//   ---     ---     ---     ---     ---

                case OP_GT | OP_AMPER: sflags[sflags_i] &=~ (OP_GT | OP_AMPER);
                    if(!sec_val) {
                        sec_end+=size;

                    } else {
                        sec_end+=sec_val*size;

                    }; if(sec_end>elems*size) {
                        sec_end=elems*size;

                    }; break;

                case OP_LT | OP_AMPER: sflags[sflags_i] &=~ (OP_LT | OP_AMPER);
                    if(!sec_val) {
                        sec_end-=size;

                    } else {
                        sec_end-=sec_val*size;

                    }; if(sec_beg>=sec_end) {
                        sec_end=sec_beg+size;

                    }; break;

//   ---     ---     ---     ---     ---

                case OP_GT | OP_MUL: sflags[sflags_i] &=~ (OP_GT | OP_MUL);
                    if(!sec_val) { lngptr+=size; break; }
                    lngptr += sec_val*size; break;

                case OP_LT | OP_MUL: sflags[sflags_i] &=~ (OP_LT | OP_MUL);
                    if(!sec_val) { lngptr-=size; break; }
                    lngptr -= sec_val*size; break;

//   ---     ---     ---     ---     ---

                case OP_LSHFT | OP_MUL: sflags[sflags_i] &=~ (OP_LSHFT | OP_MUL);
                    lngptr = 0; break;

                case OP_RSHFT | OP_MUL: sflags[sflags_i] &=~ (OP_RSHFT | OP_MUL);
                    lngptr = sec_end-size; break;

//   ---     ---     ---     ---     ---

                case OP_EQUL | OP_MUL: { sflags[sflags_i] &=~ (OP_EQUL | OP_MUL);

                    uchar* addr=((uchar*) memlng->buff)+lngptr;

                    if(!sec_val) {
                        CLMEM2(addr, size);

                    } else {
                        for(uint x=0; x<size; x++) {
                            addr[x]=sec_val>>(x*8);

                        };
                    };

                    break;
                }

//   ---     ---     ---     ---     ---

                case OP_EQUL: { sflags[sflags_i] &=~ OP_EQUL;
                    uchar* addr=((uchar*) memlng->buff)+sec_beg;

                    if(!sec_val) {
                        CLMEM2(addr, sec_end-sec_beg);

                    } else {
                        for(uint x=sec_beg; x<sec_end; x+=size, addr+=size) {
                            for(uint y=0; y<size; y++) {
                                addr[y]=sec_val>>(y*8);

                            };
                        };
                    };

                    break;
                }

//   ---     ---     ---     ---     ---

            }; sflags_i++; if(sflags_i<=mammi->lvlb) { goto SECTOP; }

            lhand       = ((uchar*) memlng->buff)+lngptr;

            result      = lhand;
            value       = lhand+size;

            mammi->lvlb = 0;

        }; goto EVAL_EXP;

                                            // no @(sec) so just expand...
    }; MAEXPS                               (&raw_value, &lhand, &value, &flags, size);

                                            // collapse arithmetic-wise
    SOLVE: CALCUS_COLLAPSE                  (&lhand, &value, &flags                  );
    goto EVAL_EXP;

//   ---     ---     ---     ---     ---

    RESULT: if(mammi->lvlb>0) {             // collapse expression if unresolved
        MAMMIT_LVLB_PRV;                     // this doesn't account for unclosed () parens
        goto SOLVE;                         // so beware! PE$O will not care for that mistake

    };

//   ---     ---     ---     ---     ---

    uchar* vtest = mammi->lvalues+mammi->lvaltop;
    VALNEW(type, name, (uchar*) memlng->buff+0, size*elems);

    switch(rd_cast) {

        case 0x00: break;                   // we'll see how to handle these later
        case 0x01: break;
        case 0x02: break;

//   ---     ---     ---     ---     ---

        case 0x09:
        case 0x05: PRCAST(LNG); break;

        case 0x03:
        case 0x07: PRCAST(CHR); break;

        case 0x04:
        case 0x08: PRCAST(WID); break;

        case 0x06:
        case 0x0A: PRCAST(QAT); break;

        default  : PRCAST(FLT); break;

//   ---     ---     ---     ---     ---

    }; for(uint x=ex_f; x<rd_tki; x++) {
        CALOUT(K, ": %s ", tokens[x]);

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

        SYMNEW("TYPE", "void",   REGVOI),   // funptr or rawptr

        SYMNEW("TYPE", "stark",  REGTRK),   // funptr
        SYMNEW("TYPE", "nihil",  REGNHL),   // funptr

        SYMNEW("TYPE", "char",   REGCHR),   // 1
        SYMNEW("TYPE", "wide",   REGWID),   // 2
        SYMNEW("TYPE", "long",   REGLNG),   // 4
        SYMNEW("TYPE", "quat",   REGQAT),   // 8

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

        case 0x3A:
            goto APTOK;

        case 0x21:                          // @(sec) operators
        case 0x24:                          // used for memlng hackery
        case 0x26:
        case 0x28:
        case 0x2A:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x40:
            if(mammi->state&MAMMIT_SF_PSEC) {
                goto FORCE_INSERT;

            };

//   ---     ---     ---     ---     ---    non @(sec) operators

        case 0x22:
        case 0x23:

        case 0x25:
        case 0x27:

        case 0x2B:
        case 0x2D:
        case 0x2F:
        case 0x3F:

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
            mammi->state  &=~MAMMIT_SF_PSEC;
            rd_tk[rd_tkp]  = rd_cur; rd_tkp++;
            goto APTOK;

//   ---     ---     ---     ---     ---    TERMINATORS

        case 0x2C:

            // APTOK copy-paste
            if(!(  0x01 <= rd_prv \
                && 0x20 >= rd_prv ) ) {     // if previous is not *also* control char {1,32}

                rd_tkp = 0; rd_tki++;       // reset position && advance slot
                rd_tk  = tokens[rd_tki];    // push token

            };

            rd_tk[rd_tkp] = rd_cur;         // set comma
            rd_tkp++;

            if(rd_nxt!=0x28) {              // check for special sequence ,(
                rd_tkp    = 0; rd_tki++;    // if not found, just leave the comma alone
                rd_tk     = tokens[rd_tki];

            } else {
                mammi->state |= MAMMIT_SF_PSEC;

            }; break;

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

            } opi=0; };

            FORCE_INSERT:
                rd_tk[rd_tkp]=rd_cur; rd_tkp++; break;

    }; if(rd_nxt) { goto TOP; };                                                            };

//   ---     ---     ---     ---     ---

int main(void)                              {

    NTNAMES();
    MEM* s  = MKSTR("MAMM_RD", 1024, 1); CLMEM(s);
    LDLNG(ZJC_DAFPAGE); memlng = GTLNG(); CLMEM(memlng);

    RPSTR(&s,

"reg vars {\n\
 long2 x 1,(*>>:*=10);\n\
 long2 y 2,(=0xFFFF);\n\
}\n",
0);

    rd_buff = MEMBUFF(s, uchar, 0);

    CALOUT(E, "\e[38;2;128;255;128m\n$PEIN:\n%s\n\e[0m\e[38;2;255;128;128m$OUT:", rd_buff);

    RDNXT();
    CALOUT(E, "\e[0m");

    /* post parse fetch-test

    uint* pe_x = (uint*) (mammi->lvalues +  0);
    uint* pe_y = (uint*) (mammi->lvalues + 16);

    for(uint i=0;i<4;i++) {
        CALOUT(K, "%u: x%u \t y%u\n", i, pe_x[i], pe_y[i]);

    };

    */

    DLMEM(memlng);
    DLMEM(s);
    DLNAMES();

    return 0;

};

//   ---     ---     ---     ---     ---