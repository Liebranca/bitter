#ifndef __MA_ALLO_H__
#define __MA_ALLO_H__

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---
// constants

#ifndef MAMMIT_TK_WIDTH
    #define MAMMIT_TK_WIDTH 64

#endif

#ifndef MAMMIT_TK_COUNT
    #define MAMMIT_TK_COUNT 64

#endif

#ifndef FRAMESZ
    #define FRAMESZ 256

#endif

#ifndef NAMESZ
    #define NAMESZ 2048

#endif

#define NUM_PASSES 2

#ifndef MEMUNIT
    #define MEMUNIT uint64_t

#endif

#define UNITSZ sizeof(MEMUNIT)

#define FREE_BLOCK      ((uintptr_t) 0xF9EEB10CF9EEB10CLL)
#define FRBLK           (NAMESZ-8)

#define DEAD_BLOCK      ((uintptr_t) 0xDEADBEEFDEADBEEFLL)

//   ---     ---     ---     ---     ---

void allo_test(void);

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __MA_ALLO_H__