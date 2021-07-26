#include "zjc_joj.h"
#include "../KVRNEL/kvr_paths.h"

#include <stdio.h>

#define BLKMGK_V    0.1
#define BLKMGK_DONE 0x444f4e45

//   ---     ---     ---     ---     ---

static uint DAF_SIZE_I;

//   ---     ---     ---     ---     ---

EXPORT void NTBLKMGK(char** pth_l)          {

    NTKVRPTH(pth_l);

    DAF_SIZE_I=0;

    CALOUT("BLKMGK v%f\n\b", BLKMGK_V);                                                     };

EXPORT void DLBLKMGK() { DLKVRPTH(); };

//   ---     ---     ---     ---     ---

EXPORT void UTJOJ(uint i, uint dim)         {

                                            // kiknit on first run
    if(!i)                                  { NTJOJENG();               };

    ENCJOJ                                  (dim, &DAF_SIZE_I           );

  if(i==BLKMGK_DONE) {                      // zip and kikdel on last run
        ZPJOJ                               (DAF_SIZE_I                 );
        DLJOJENG                            (                           );
    };                                                                                      };

//   ---     ---     ---     ---     ---