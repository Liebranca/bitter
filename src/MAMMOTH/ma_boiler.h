#ifndef __MA_BOILER_H__
#define __MA_BOILER_H__

#include "ma_boxes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

int    NOREDCL (uchar* name    );           // check varname in use
int    NOOVERSZ(uint sz, uint f);           // throw error if sz%f

//   ---     ---     ---     ---     ---

void   REGTP   (void           );           // main decl hook

void   REGVOI  (void           );           // 'entry' decl hooks
void   REGNHL  (void           );           // these just set some flags
void   REGTRK  (void           );           // could maybe be replaces by a single fun
void   REGCHR  (void           );           // however, i felt like being lazy
void   REGWID  (void           );
void   REGLNG  (void           );
void   REGQAT  (void           );
void   REGFLT  (void           );

//   ---     ---     ---     ---     ---

void   UPKTYPE (uchar* typeval );           // setup typedata for read decl

SYMBOL SYMNEW  (uchar* fam     ,
                uchar* key     ,
                NIHIL  fun     );           // used for populating gnames

void   VALNEW  (uchar* name    ,
                uchar* val     ,
                uint   size    );           // used for populating lnames

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

//   ---     ---     ---     ---     ---

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
    default:                                \
        (*r)+=(*v); break;                  \
                                            \
    }; (*v)=0; break;                       }

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

//   ---     ---     ---     ---     ---

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

//   ---     ---     ---     ---     ---

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

#define PRCAST(T) PRCAST##_##T

//   ---     ---     ---     ---     ---

#define ADDRFET(T, addr) (T*) (((ADDR*) addr)->box+0)

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

#ifdef __cplusplus
}
#endif

#endif // __MA_BOILER_H__