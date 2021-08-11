#ifndef __SIN_GLOBALS_H__
#define __SIN_GLOBALS_H__

#include "../KVRNEL/zjc_CommonTypes.h"
#include "../KVRNEL/TYPES/zjc_id.h"
#include "../../KVRNEL/TYPES/zjc_hash.h"

#include "../glad/glad.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

#define SIN_TEXNIDS  64
#define SIN_M3DNIDS 128
#define SIN_ANSNIDS  32
#define SIN_HASHSIZ   6

#define SIN_TEXLAYS   4

//   ---     ---     ---     ---     ---

void NTSINHASH(                    );       // kiknit
void DLSINHASH(                    );       // kikdel

int STSINHASH (ID* data            );       // insert data into table
int GTSINHASH (ID* data, void** dst,
               int pop             );       // looks for id matching data and put it in dst
                                            // if pop, removes data from hash
int FNDSINHASH(char* key,void** dst,
               int pop             );       // looks for id matching key and put it in dst
                                            // if pop, removes data from hash

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SIN_GLOBALS_H__