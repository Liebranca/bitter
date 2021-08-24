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
void LDTEX   (uint idex           );        // load data into tex

uint GTTEXLOC(uint idex           );        // get gl location

void RDPNG   (char* fpath         ,
              uchar* buff         );

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SIN_TEXTURE_H__