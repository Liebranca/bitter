#ifndef __MA_BOILER_H__
#define __MA_BOILER_H__

#include "ma_boxes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

uint   GTUNITCNT(uint size, uint mag);      // calculate arrsize%unitsz
void   CHKMEMLAY(void               );      // contiguousness test

void   MOVBLK   (BLK* b, int dirn   );      // block byte && memunit shifts
void   BYTESTEP (void               );      // advance rd_result by rd_size
void   TPADDR   (ADDR* addr         );      // decode addr typedata
void   RSTSEC   (void               );      // prep for first seceval run
void   RSTPTRS  (void               );      // reset result, lhand, value (not lngptr)

void   MAFETCH (MEMUNIT* r          ,
                MEMUNIT* v          );      // value fetcher

//   ---     ---     ---     ---     ---

int    NOREDCL  (uchar* name        );      // check varname in use
int    NOOVERSZ (uint sz, uint f    );      // throw error if sz%f

//   ---     ---     ---     ---     ---

void   REGTP    (void               );      // main decl hook

void   REGVOI   (void               );      // 'entry' decl hooks
void   REGNHL   (void               );      // these just set some flags
void   REGTRK   (void               );      // could maybe be replaces by a single fun
void   REGCHR   (void               );      // however, i felt like being lazy
void   REGWID   (void               );
void   REGINT   (void               );
void   REGLNG   (void               );
void   REGFLT   (void               );

//   ---     ---     ---     ---     ---

void   UPKTYPE (uchar* typeval );           // setup typedata for read decl

SYMBOL SYMNEW  (uchar* fam     ,
                uchar* key     ,
                NIHIL  fun     );           // used for populating gnames

void   VALNEW  (uchar*   name  ,
                MEMUNIT* val   ,
                uint     size  );           // used for populating lnames

void VALSIZ    (uchar* type    ,
                uchar*  to     );           // get size of base type

//   ---     ---     ---     ---     ---
// state flags...

#define MAMMIT_SF_PESO 0x000000FF

#define MAMMIT_SF_PESC 0x00000001
#define MAMMIT_SF_PLIT 0x00000002
#define MAMMIT_SF_PSEC 0x00000004
#define MAMMIT_SF_PFET 0x00000008
#define MAMMIT_SF_PLCO 0x00000010
#define MAMMIT_SF_PMCO 0x00000020

#define MAMMIT_SF_CNTX 0x0000FF00

#define MAMMIT_SF_CREG 0x00000100
#define MAMMIT_SF_CDEC 0x00000200
#define MAMMIT_SF_CDEF 0x00000400

#define MAMMIT_SF_CCLA 0x00000800
#define MAMMIT_SF_CPRC 0x00001000

//   ---     ---     ---     ---     ---
// debug/errcatch stuff

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
// operators

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

//   ---     ---     ---     ---     ---

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

//   ---     ---     ---     ---     ---

#define OP_W_EQUR                           \
  ( OP_GT    | OP_LT                        \
  | OP_BANG  | OP_MODUS                     \
  | OP_AMPER | OP_XORUS                     \
  | OP_MINUS | OP_MUL                       \
  | OP_DIV   | OP_PIPE                      \
  | OP_EQUL  )

//   ---     ---     ---     ---     ---

#define OPSWITCH_MINUSX                     \
    if(rd_flags&OP_MINUS) {                 \
        (*r)=-(*r);                         \
        rd_flags&=~OP_MINUS;                \
    }; break

#define OP_FORCEBIN(op)                     \
    (*r)=((uint)(*r))op((uint)(*v))

#define OP_FORCEUNA(op)                     \
    (*r)=op(uint)(*v)

//   ---     ---     ---     ---     ---

#define CALCUS_OPSWITCH {                   \
                                            \
    switch(rd_flags&0xFFFFFFFC) {           \
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
                                            \
/*   ---     ---     ---     ---     --- */ \
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
                                            \
/*   ---     ---     ---     ---     --- */ \
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
                                            \
/*   ---     ---     ---     ---     --- */ \
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
/*   ---     ---     ---     ---     --- */ \
                                            \
                                            \
    case OP_XORUS:                          \
        OP_FORCEBIN(^); OPSWITCH_MINUSX;    \
                                            \
    case OP_TILDE:                          \
        OP_FORCEUNA(~); OPSWITCH_MINUSX;    \
                                            \
                                            \
/*   ---     ---     ---     ---     --- */ \
                                            \
    case OP_AT: {                           \
        MAFETCH((MEMUNIT*) r, (MEMUNIT*) v);\
        OPSWITCH_MINUSX;                    \
    };                                      \
                                            \
/*   ---     ---     ---     ---     --- */ \
                                            \
    default:                                \
        (*r)+=(*v); break;                  \
                                            \
    }; (*v)=0; break;                       }

//   ---     ---     ---     ---     ---

#define FETMASK(elems, idex) (idex)&((elems)-1)
#define SIZMASK(size) ((0x01LL<<(size*8))-1)

//   ---     ---     ---     ---     ---

#define ADDRFET(T, addr) (T*) (((ADDR*) addr)->box+0)

#define CURLVAL (mammi->lvalues+mammi->lvaltop)

#define INCLVAL(x) {                                                                        \
    if( ((x)/UNITSZ) < 1) { mammi->lvaltop++;             }                                 \
    else                  { mammi->lvaltop += (x)/UNITSZ; };                                }

//   ---     ---     ---     ---     ---

#define MAMMIT_REG_ADD {                                                                    \
    pe_reg->jmpt[pe_reg->elems] = mammi->lvaltop;                                           \
    if( ((size)/UNITSZ) < 1) { pe_reg->size++;                }                             \
    else                     { pe_reg->size += (size)/UNITSZ; };                            \
    pe_reg->elems++;                                                                        }

#define MAMMIT_LVLB_NXT {                                                                   \
    mammi->lvlb_stack[mammi->lvlb]=rd_flags;                                                \
    mammi->lvlb++; rd_flags=0;                                                              \
    rd_lhand+=rd_step; rd_value+=rd_step; lngptr+=rd_step;                                  }

#define MAMMIT_LVLB_PRV {                                                                   \
    mammi->lvlb--; rd_flags=mammi->lvlb_stack[mammi->lvlb];                                 \
    rd_lhand-=rd_step; rd_value-=rd_step; lngptr-=rd_step;                                  }

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __MA_BOILER_H__