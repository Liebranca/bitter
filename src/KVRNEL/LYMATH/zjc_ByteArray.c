/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    ZJC_BYTEARRAY                         *
*                                           *
*     -basic bytesized units                *
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

#include "zjc_ByteArray.h"
#include "zjc_evil.h"
#include "TYPES/zjc_BinTypes.h"

#include <stdio.h>

//   ---     ---     ---     ---     ---

#define LUS8(x)  lym_ustr8_##x

static ustr8*    lym_ustr8_a;
static ustr8*    lym_ustr8_b;

//   ---     ---     ---     ---     ---

void ldaus8  (ustr8* a)                     { LUS8(a)=a;                                    };
void ldbus8  (ustr8* b)                     { LUS8(b)=b;                                    };

void staus8  (uint   F)                     { LUS8(a)->F=F;                                 };
void stbus8  (uint   F)                     { LUS8(b)->F=F;                                 };

void shaus8  (void    )                     { CALOUT('E', "[x%u y%u z%u w%u]",              \
                                                     LUS8(a)->x, LUS8(a)->y ,               \
                                                     LUS8(a)->z, LUS8(a)->w);               };

void shbus8  (void    )                     { CALOUT('E', "[x%u y%u z%u w%u]",              \
                                                     LUS8(b)->x, LUS8(b)->y ,               \
                                                     LUS8(b)->z, LUS8(b)->w);               };

void fshaus8 (void    )                     { CALOUT('E', "0x%02X", LUS8(a)->F);            };
void fshbus8 (void    )                     { CALOUT('E', "0x%02X", LUS8(b)->F);            };

void bshaus8 (void    )                     { CALOUT('E', "0b"          );
                                              for(uchar i=0; i<32; i++) {                   \
                                                  CALOUT('E', "%d"      ,                   \
                                                     nthbit(LUS8(a)->F, i%8));              \
                                            };                                              };

void bshbus8 (void    )                     { CALOUT('E', "0b"          );
                                              for(uchar i=0; i<32; i++) {                   \
                                                  CALOUT('E', "%d"      ,                   \
                                                     nthbit(LUS8(b)->F, i%8));              \
                                            };                                              };

//   ---     ---     ---     ---     ---

void bdaus8 (uchar x, uchar y,                                                              \
             uchar z, uchar w)              { LUS8(a)->F=x+(y<<8)+(z<<16)+(w<<24);          };
void bdbus8 (uchar x, uchar y,                                                              \
             uchar z, uchar w)              { LUS8(b)->F=x+(y<<8)+(z<<16)+(w<<24);          };

void faddus8(void)                          { LUS8(a)->F=LUS8(a)->F+LUS8(b)->F;             };
void fsubus8(void)                          { LUS8(a)->F=LUS8(a)->F-LUS8(b)->F;             };
void fmulus8(void)                          { LUS8(a)->F=LUS8(a)->F*LUS8(b)->F;             };
void fdivus8(void)                          { LUS8(a)->F=LUS8(a)->F/LUS8(b)->F;             };
void fandus8(void)                          { LUS8(a)->F=LUS8(a)->F&LUS8(b)->F;             };
void f_orus8(void)                          { LUS8(a)->F=LUS8(a)->F|LUS8(b)->F;             };
void f_stus8(void)                          { LUS8(a)->F|=LUS8(b)->F;                       };
void fustus8(void)                          { LUS8(a)->F&=~LUS8(b)->F;                      };
void fnndus8(void)                          { LUS8(a)->F=!(LUS8(a)->F&LUS8(b)->F);          };
void fxorus8(void)                          { LUS8(a)->F=LUS8(a)->F^LUS8(b)->F;             };
void fxnrus8(void)                          { LUS8(a)->F=!(LUS8(a)->F^LUS8(b)->F);          };
void fnorus8(void)                          { LUS8(a)->F=!(LUS8(a)->F|LUS8(b)->F);          };

//   ---     ---     ---     ---     ---

void addus8 (void   )                       { LUS8(a)->x=LUS8(a)->x+LUS8(b)->x;             \
                                              LUS8(a)->y=LUS8(a)->y+LUS8(b)->y;             \
                                              LUS8(a)->z=LUS8(a)->z+LUS8(b)->z;             \
                                              LUS8(a)->w=LUS8(a)->w+LUS8(b)->w;             };

void subus8 (void   )                       { LUS8(a)->x=LUS8(a)->x-LUS8(b)->x;             \
                                              LUS8(a)->y=LUS8(a)->y-LUS8(b)->y;             \
                                              LUS8(a)->z=LUS8(a)->z-LUS8(b)->z;             \
                                              LUS8(a)->w=LUS8(a)->w-LUS8(b)->w;             };

void mulus8 (uchar s)                       { LUS8(a)->x=LUS8(a)->x*s;                      \
                                              LUS8(a)->y=LUS8(a)->y*s;                      \
                                              LUS8(a)->z=LUS8(a)->z*s;                      \
                                              LUS8(a)->w=LUS8(a)->w*s;                      };

void mulus8f(float s)                       { LUS8(a)->x=((float)LUS8(a)->x)*s;\
                                              LUS8(a)->y=((float)LUS8(a)->y)*s;\
                                              LUS8(a)->z=((float)LUS8(a)->z)*s;\
                                              LUS8(a)->w=((float)LUS8(a)->w)*s;};

void divus8 (uchar s)                       { LUS8(a)->x=LUS8(a)->x/s;                      \
                                              LUS8(a)->y=LUS8(a)->y/s;                      \
                                              LUS8(a)->z=LUS8(a)->z/s;                      \
                                              LUS8(a)->w=LUS8(a)->w/s;                      };

//   ---     ---     ---     ---     ---