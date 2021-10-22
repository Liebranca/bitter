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

void PROCADD   (uint size      );           // append line to proc

uint POPOPS    (void           );           // pop operators from value

//   ---     ---     ---     ---     ---

void CALCUS_COLLAPSE(void);                 // magic

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

#define OP_PLUS  0x0000000000000001LL
#define OP_MINUS 0x0000000000000002LL
#define OP_MUL   0x0000000000000004LL
#define OP_DIV   0x0000000000000008LL
#define OP_MODUS 0x0000000000000010LL

#define OP_GT    0x0000000000000020LL
#define OP_LT    0x0000000000000040LL
#define OP_EQUAL 0x0000000000000080LL
#define OP_ECOOL 0x0000000000000100LL

#define OP_BANG  0x0000000000000200LL
#define OP_QUEST 0x0000000000000400LL

#define OP_TILDE 0x0000000000000800LL
#define OP_MONEY 0x0000000000001000LL
#define OP_KUSH  0x0000000000002000LL
#define OP_RADIX 0x0000000000004000LL

//   ---     ---     ---     ---     ---

#define OP_COLON 0x0000000000008000LL

#define OP_AMPER 0x0000000000010000LL
#define OP_PIPE  0x0000000000020000LL
#define OP_XORUS 0x0000000000040000LL
#define OP_LSHFT 0x0000000000080000LL
#define OP_RSHFT 0x0000000000100000LL

#define OP_DAMPR 0x0000000000200000LL
#define OP_DPIPE 0x0000000000400000LL

#define OP_AT    0x0000000000800000LL
#define OP_ARROW 0x0000000001000000LL
#define OP_BRROW 0x0000000002000000LL
#define OP_SQUOT 0x0000000004000000LL
#define OP_DQUOT 0x0000000008000000LL

//   ---     ---     ---     ---     ---

#define OP_PPLUS 0x0000000010000000LL
#define OP_MMINU 0x0000000020000000LL

#define OP_EPLUS 0x0000000040000000LL
#define OP_EMINU 0x0000000080000000LL
#define OP_EMUL  0x0000000100000000LL
#define OP_EDIV  0x0000000200000000LL
#define OP_EMODU 0x0000000400000000LL

#define OP_EBANG 0x0000000800000000LL
#define OP_EAMPR 0x0000001000000000LL
#define OP_EPIPE 0x0000002000000000LL
#define OP_EXOR  0x0000004000000000LL

#define OP_EGT   0x0000008000000000LL
#define OP_ELT   0x0000010000000000LL

#define OP_EMONY 0x0000020000000000LL

//   ---     ---     ---     ---     ---

#define OPSWITCH_MINUSX                     \
    if(rd_flags&OP_MINUS) {                 \
        (*r)=-(*r);                         \
        rd_flags&=~OP_MINUS;                \
    }; break

#define OP_FORCEBIN(op)                     \
    (*r)=((MEMUNIT)(*r))op((MEMUNIT)(*v))

#define OP_FORCEUNA(op)                     \
    (*r)=op(MEMUNIT)(*v)

//   ---     ---     ---     ---     ---

#define CALCUS_OPSWITCH {                   \
                                            \
    switch(rd_flags&0xFFFFFFFFFFFFBFFFLL) { \
                                            \
    case OP_EMUL:                           \
    case OP_MUL:                            \
        (*r)*=(*v); OPSWITCH_MINUSX;        \
                                            \
    case OP_EDIV:                           \
    case OP_DIV:                            \
        (*r)/=(*v); OPSWITCH_MINUSX;        \
                                            \
    case OP_EMODU:                          \
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
    case OP_EGT:                            \
        (*r)=(*r)>=(*v); OPSWITCH_MINUSX;   \
                                            \
    case OP_LSHFT:                          \
        OP_FORCEBIN(<<); OPSWITCH_MINUSX;   \
                                            \
    case OP_LT:                             \
        (*r)=(*r)<(*v);  OPSWITCH_MINUSX;   \
                                            \
    case OP_ELT:                            \
        (*r)=(*r)<=(*v); OPSWITCH_MINUSX;   \
                                            \
                                            \
/*   ---     ---     ---     ---     --- */ \
                                            \
    case OP_BANG:                           \
        (*r)=!(*v); OPSWITCH_MINUSX;        \
                                            \
    case OP_EBANG:                          \
        (*r)=(*r)!=(*v); OPSWITCH_MINUSX;   \
                                            \
    case OP_QUEST:                          \
        (*r)=(*v)!=0; OPSWITCH_MINUSX;      \
                                            \
    case OP_EQUAL:                          \
        (*r)=(*v); rd_flags&=~OP_EQUAL;     \
        OPSWITCH_MINUSX;                    \
                                            \
    case OP_ECOOL:                          \
        (*r)=(*r)==(*v); OPSWITCH_MINUSX;   \
                                            \
                                            \
/*   ---     ---     ---     ---     --- */ \
                                            \
    case OP_DAMPR:                          \
        OP_FORCEBIN(&&); OPSWITCH_MINUSX;   \
                                            \
    case OP_EAMPR:                          \
    case OP_AMPER:                          \
        OP_FORCEBIN(&); OPSWITCH_MINUSX;    \
                                            \
                                            \
    case OP_DPIPE:                          \
        OP_FORCEBIN(||); OPSWITCH_MINUSX;   \
                                            \
    case OP_EPIPE:                          \
    case OP_PIPE:                           \
        OP_FORCEBIN(|); OPSWITCH_MINUSX;    \
                                            \
                                            \
/*   ---     ---     ---     ---     --- */ \
                                            \
    case OP_EXOR:                           \
    case OP_XORUS:                          \
        OP_FORCEBIN(^); OPSWITCH_MINUSX;    \
                                            \
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
    case OP_PPLUS:                          \
        (*r)++; break;                      \
                                            \
    case OP_MMINU:                          \
        (*r)--; break;                      \
                                            \
/*   ---     ---     ---     ---     --- */ \
                                            \
    case OP_EPLUS:                          \
    case OP_PLUS:                           \
    default:                                \
        (*r)+=(*v); break;                  \
                                            \
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

#define MAMMIT_CNTX_ADD(whom) {                                                             \
    whom->jmpt[whom->elems] = mammi->lvaltop - whom->start;                                 \
    if( ((size)/UNITSZ) < 1) { whom->size++;                }                               \
    else                     { whom->size += (size)/UNITSZ; };                              \
    whom->elems++;                                                                          }

#define MAMMIT_CNTX_FETCH(whom, off) ((whom->start) + (whom->jmpt[off]))

//   ---     ---     ---     ---     ---

#define MAMMIT_LVLB_NXT {                                                                   \
    mammi->lvlb_stack[mammi->lvlb]=rd_flags;                                                \
    mammi->lvlb++; rd_flags=0;                                                              \
    rd_lhand+=rd_step; rd_value+=rd_step; lngptr+=rd_step;                                  }

#define MAMMIT_LVLB_PRV {                                                                   \
    mammi->lvlb--; rd_flags=mammi->lvlb_stack[mammi->lvlb];                                 \
    rd_lhand-=rd_step; rd_value-=rd_step; lngptr-=rd_step;                                  }

//   ---     ---     ---     ---     ---

#define FEL32(v) ((v)&(SIZMASK(4)))
#define FEH32(v) ((v)>>32)

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __MA_BOILER_H__