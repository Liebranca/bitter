#ifndef __SIN_TEXTURE_H__
#define __SIN_TEXTURE_H__

#include "KVRNEL/zjc_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

void NTSINTEX(                    );        // kiknit
void DLSINTEX(                    );        // kikdel

uint MKTEX   (uint dim, char* name);        // occupy texslot
void DLTEX   (uint idex           );        // free texslot

void RDPNG   (char* fpath         );

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SIN_TEXTURE_H__