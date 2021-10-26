#ifndef __MA_BOXES_H__
#define __MA_BOXES_H__

#include "KVRNEL/TYPES/zjc_hash.h"
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---
// constants

#ifndef MAMMIT_TK_WIDTH
    #define MAMMIT_TK_WIDTH 64

#endif

#ifndef MAMMIT_TK_COUNT
    #define MAMMIT_TK_COUNT 64

#endif

#ifndef FRAMESZ
    #define FRAMESZ 256

#endif

#ifndef NAMESZ
    #define NAMESZ 2048

#endif

#ifndef MEMUNIT
    #define MEMUNIT ulong

#endif

#define UNITSZ sizeof(MEMUNIT)

#define FREE_BLOCK      0x104EBABEF9EEB10CLL
#define FRBLK           (NAMESZ-8)

#define DEAD_BLOCK      0xF0CCBABEDEADBEEFLL

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

} TYPEDATA; extern TYPEDATA typedata;

//   ---     ---     ---     ---     ---

typedef struct MAMM_SYNTX_SYMBOL {          // used for string to funcall mambo

    ID    id;                               // polyheader, makes this block hashable
    NIHIL onrd;                             // links block with a given funcall

} SYMBOL;
                               
//   ---     ---     ---     ---     ---

#define CALCUS_CONST 0xC0457A47
#define CALCUS_FETCH 0xFE7C4B01
#define CALCUS_SEPAR 0x5E9A9A7E

typedef struct MAMM_CALCUS_TOKEN {          // breaks down expressions into maleable data

    union {
        struct {
           uint    ttype;                   // token type
           uint    vtype;                   // value type

        }; MEMUNIT prefix;                  // describes the block
    };

    MEMUNIT lops;                           // left-hand operators
    MEMUNIT rops;                           // right-hand operators

    MEMUNIT value;                          // some number; can be an address

} CTOK; extern CTOK* rd_ctok;

//   ---     ---     ---     ---     ---

typedef struct MAMM_CNTX {

    uint    elems;                          // number of labels
    uint    size;                           // total size, measured in memunits

    MEMUNIT state;                          // internal state flags

} CNTX; extern CNTX* cur_cntx;

//   ---     ---     ---     ---     ---

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

    uchar     lvla_stack[FRAMESZ];          // markers for recalling previous context
    MEMUNIT   lvlb_stack[FRAMESZ];          // ^idem, for prev evalstate of expression

    uintptr_t vaddr;                        // next fetch at
    uint      vtype;                        // next fetch typedata

    uint      lvaltop;                      // next offset @lvalues that's free
    MEMUNIT   lvalues   [NAMESZ ];          // yer vars arrrr

    SYMBOL    slots     [NAMESZ ];          // array of built-ins
    STK       slstack;                      // stack of (free)indices into built-ins array
    uint      slstack_i [NAMESZ ];          // stack space

    uint      jmpt_i;                       // top of jump table
    uintptr_t jmpt      [       ];          // offsets into lvalues

} MAMMIT; extern MAMMIT* mammi;

//   ---     ---     ---     ---     ---

typedef struct MAMM_BLOCK_ACC {             // byte access helper

    uint base;                              // base unit index
    sint cbyte;                             // byte offset into base

} BLK;

//   ---     ---     ---     ---     ---

typedef struct MAMM_CODE {                  // operations as data

    uint    loc;                            // offset to instruction header
    uint    size;                           // volume of data, measured in memunits

    MEMUNIT data[];                         // bunch of bits read by instruction

} CODE;

//   ---     ---     ---     ---     ---

extern uint     gblevil;

extern uchar    tokens[MAMMIT_TK_COUNT][MAMMIT_TK_WIDTH];

extern uchar*   rd_tk;
extern uint     rd_tki;
extern uint     rd_tkp;
extern uint     rd_tkx;

extern uchar*   rd_buff;

extern uchar    rd_cur;
extern uchar    rd_prv;
extern uchar    rd_nxt;
extern uint     rd_cast;
extern uint     rd_line;

extern ushort   rd_wid;
extern uint     rd_pos;

//   ---     ---     ---     ---     ---

extern MEMUNIT  rd_flags;

extern uint     rd_size;
extern uint     rd_elems;
extern uint     rd_cbyte;
extern uint     rd_step;
extern uint     rd_units;

extern uchar*   rd_rawv;

extern MEMUNIT* rd_result;
extern MEMUNIT* rd_lhand;
extern MEMUNIT* rd_value;
extern MEMUNIT* rd_oldval;

extern ulong    szmask_a;
extern ulong    szmask_b;

extern BLK      sec_beg;
extern BLK      sec_cur;
extern BLK      sec_end;

//   ---     ---     ---     ---     ---

extern HASH*    GNAMES_HASH;
extern HASH*    LNAMES_HASH;

extern MEM*     memlng;
extern uint     lngptr;

//   ---     ---     ---     ---     ---

extern uint     ins_code;
extern uint     ins_argc;

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __MA_BOXES_H__