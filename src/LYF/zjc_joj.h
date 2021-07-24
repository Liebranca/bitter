#ifndef __ZJC_JOJ_H__
#define __ZJC_JOJ_H__

#include "../KVRNEL/MEM/kvr_bin.h"
#include "../KVRNEL/TYPES/zjc_BinTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

BIN* MKJOJ(char* path,                      // inits a joj file
           char* mode,
           uint  size);

BIN* LDJOJ(char* path);                     // open an existing joj

int ENCJOJ(BIN* joj, float* pixels);        // write pixels to joj

int DEFLJOJ(char** paths,                   // compress multiple joj into a single daf
            uint   num_paths,
            uint   size );

uint* GTJOJ(BIN* joj);                      // get start of joj buffer

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __ZJC_JOJ_H__