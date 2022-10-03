#ifndef __MA_BOXES_H__
#define __MA_BOXES_H__

#include "KVRNEL/TYPES/zjc_hash.h"

#include "ma_allo.h"

#include <stdio.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct MAMM_TYPEVAL_DATA {          // to simplify reading wacky types

    uint   flags;                           // static, const, unsigned, addr...
    uint   strsz;                           // size, or rather capacity
    uint   strus;                           // non-terminator charcount
    uint   strtp;                           // type-id of chars

} TYPEDATA; extern TYPEDATA typedata;

//   ---     ---     ---     ---     ---

typedef struct MAMM_SYNTX_SYMBOL {          // used for string to funcall mambo

    ID      id;                             // polyheader, makes this block hashable

    NIHIL   onrd;                           // links block with a given funcall
    MEMUNIT value;                          // for when you want a magic return ptr

} SYMBOL;

//   ---     ---     ---     ---     ---

typedef struct MAMM_SYNTX_MACRO {           // alias for name/token

    ID      id;                             // identifier for alias
    MEMUNIT value;

} MACRO;

//   ---     ---     ---     ---     ---

typedef struct MAMM_SYNTX_LABEL  {          // alias for value

    ID       id;                            // polyheader

    MEMUNIT  loc;                           // location of self within table
    MEMUNIT  p_loc;                         // location of parent

    TYPEDATA meta;                          // it's data about other data!

} LABEL;

//   ---     ---     ---     ---     ---

#define CALCUS_CONST 0xC0457A47
#define CALCUS_FETCH 0xFE7C4B01
#define CALCUS_MACRO 0xE49A4DE9
#define CALCUS_SEPAR 0x5E9A9A7E
#define CALCUS_NIHIL 0x5001C1DE
#define CALCUS_CHSTR 0xC4A957A9

typedef struct MAMM_CALCUS_TOKEN {          // breaks down expressions into maleable data

    union {
        struct {
           uint     ttype;                  // token type
           uint     vsize;                  // value size; only used for vsa

        }; MEMUNIT  prefix;                 // describes the block
    };

    uchar   lops[8];                        // left-hand operators
    uchar   rops[8];                        // right-hand operators

    MEMUNIT value;                          // some number; can be an address

} CTOK; extern CTOK* rd_ctok;

//   ---     ---     ---     ---     ---

typedef struct MAMM_CNTX {                  // a sentinel for block end/start

    MEMUNIT state;                          // internal state flags

    uint    elems;                          // number of labels
    uint    size;                           // total size, measured in memunits

} CNTX; extern CNTX* cur_cntx;

//   ---     ---     ---     ---     ---

#define MA_STRM_FILE 0x01

typedef struct MAMM_STRM {                  // ram, disk... it's all storage

    uint    cur;                            // current offset
    uint    sz;                             // upper bound
    uint    used;                           // utilized capacity
    uint    flg;                            // usage flags

    void*   ptr;                            // addres of block itself


} STRM;

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

    uchar     lvla_stack[FRAMESZ      ];    // markers for recalling previous context
    MEMUNIT   lvlb_stack[FRAMESZ      ];    // ^idem, for prev evalstate of expression
    MEMUNIT   call_stack[FRAMESZ      ];    // ^same, records jumped-from (go-back-to)

    uint      callstk_i;                    // callstack top

    uintptr_t vaddr;                        // next fetch at
    uint      vtype;                        // next fetch typedata

    uint      lvaltop;                      // next offset @lvalues that's free
    MEMUNIT   lvalues   [NAMESZ       ];    // yer vars arrrr
    SYMBOL    gvalues   [NAMESZ       ];    // array of built-ins

    MACRO     mvalues   [NAMESZ       ];    // textual substitution
    STK       mvalstk;                      // free indices into mvalues
    uint      mvalstk_i [NAMESZ       ];    // ^memory for stack

    uint      jmpt_i;                       // top of jump table
    LABEL     jmpt_h    [NAMESZ       ];    // descriptors for contents of table
    uintptr_t jmpt      [NAMESZ       ];    // offsets into lvalues

    uchar     entry     [ZJC_IDK_WIDTH];    // program entry point
    MEMUNIT   next;                         // next line to be executed

    uint      pass;                         // pass number

    size_t    strm_i;                       // current stream
    STRM      strm      [FOPEN_MAX    ];    // opened streams


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
extern HASH*    MNAMES_HASH;

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