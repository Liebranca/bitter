#ifndef __ZJC_JOJ_H__
#define __ZJC_JOJ_H__

#include "../KVRNEL/MEM/kvr_bin.h"
#include "../KVRNEL/TYPES/zjc_BinTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct JOJFILE {                    // a square, four-layered texture file

    BIN  src;                               // bin with mem header
    uint size;                              // images are always squares
    uint buffloc;                           // stride to start of joj buff

} BJOJ;

//   ---     ---     ---     ---     ---

BJOJ* MKJOJ(char* path,                     // inits a joj file
            char* mode,
            uint  size);

BJOJ* LDJOJ(char* path);                    // open an existing joj

int DEFLJOJ(char*  path,                    // compress multiple joj into a single daf
            float* pixels,
            uint   size  );

JOJPIX* GTJOJ(BJOJ* joj);                   // get the pixel buffer of a joj bin

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ZJC_JOJ_H__