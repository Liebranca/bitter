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
    #define NAMESZ 1024

#endif

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
    uint      lvlb_stack[FRAMESZ];          // ^idem, for prev evalstate of expression
    NIHIL     cntxcallbk[FRAMESZ];          // callbacks for context state swaps

    uintptr_t vaddr;                        // next fetch at
    uint      vtype;                        // next fetch data

    uint      lvaltop;                      // next offset @lvalues that's free
    uchar     lvalues   [NAMESZ ];          // yer vars arrrr
                                
    SYMBOL    slots     [NAMESZ ];          // array of built-ins
    STK       slstack;                      // stack of (free)indices into built-ins array

} MAMMIT; extern MAMMIT* mammi;

//   ---     ---     ---     ---     ---

typedef struct MAMM_ADDR {                  // helper struct for var fetch/insert

    ID    id;                               // typedata (0x00-03) name (0x04-17)
    uchar box[];                            // stored data

} ADDR;

//   ---     ---     ---     ---     ---

extern uint   gblevil;

extern uchar  tokens[MAMMIT_TK_COUNT][MAMMIT_TK_WIDTH];

extern uchar* rd_tk;
extern uint   rd_tki;
extern uint   rd_tkp;
extern uint   rd_tkx;

extern uchar* rd_buff;

extern uchar  rd_cur;
extern uchar  rd_prv;
extern uchar  rd_nxt;
extern uint   rd_cast;
extern uint   rd_line;

extern ushort rd_wid;
extern uint   rd_pos;

//   ---     ---     ---     ---     ---

extern uint   rd_flags;
extern uint   rd_size;
extern uint   rd_elems;

extern uchar* rd_rawv;
extern uchar* rd_lhand;
extern uchar* rd_value;
extern uchar* rd_oldval;

extern ulong  szmask_a;
extern ulong  szmask_b;

extern uint   sec_beg;
extern uint   sec_end;

//   ---     ---     ---     ---     ---

extern HASH*  GNAMES_HASH;
extern HASH*  LNAMES_HASH;

extern MEM*   memlng;
extern uint   lngptr;

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __MA_BOXES_H__