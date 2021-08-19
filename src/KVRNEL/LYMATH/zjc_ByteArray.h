#ifndef __ZJC_BYTERRAY_H__
#define __ZJC_BYTERRAY_H__

#include "zjc_evil.h"
#include "zjc_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct LYMTH_USTR8 {

    union {
        struct {
            uint x:8;
            uint y:8;
            uint z:8;
            uint w:8;

        };  uint  F;
            uchar V[4];

    };

} ustr8; CASSERT(sizeof(ustr8)==4, "USTR8 SHOULD BE 4 BYTES");

//   ---     ---     ---     ---     ---

void ldaus8  (ustr8* a        );            // load address into var
void ldbus8  (ustr8* b        );

void bdaus8  (uchar x, uchar y,
              uchar z, uchar w);            // set val by elem
void bdbus8  (uchar x, uchar y,
              uchar z, uchar w);

void staus8  (uint  F         );            // set val by mask
void stbus8  (uint  F         );

//   ---     ---     ---     ---     ---

void addus8  (void            );            // ncrry a+b
void subus8  (void            );            // ncrry a-b
void mulus8  (uchar s         );            // ncrry a*s
void mulus8f (float s         );            // ncrry ((float)a)*s
void divus8  (uchar s         );            // ncrry a/s

void faddus8 (void            );            // carry a+b
void fsubus8 (void            );            // carry a-b
void fmulus8 (void            );            // carry a*b
void fdivus8 (void            );            // carry a/b

//   ---     ---     ---     ---     ---

void shaus8  (void            );            // dec repr
void shbus8  (void            ); 

void fshaus8 (void            );            // hex repr
void fshbus8 (void            );

void bshaus8 (void            );            // bin repr
void bshbus8 (void            );

//   ---     ---     ---     ---     ---

void fxorus8 (void            );            // a^b

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif //__ZJC_BYTERRAY_H__