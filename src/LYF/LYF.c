#include "zjc_joj.h"
#include "../KVRNEL/kvr_paths.h"

#include <stdio.h>

//   ---     ---     ---     ---     ---

EXPORT void NTLYF(char** pth_l)             { NTKVRPTH(pth_l);                              };
EXPORT void DLLYF(            )             { DLKVRPTH(     );                              };

//   ---     ---     ---     ---     ---

EXPORT void RNCOL(float* pixels, uint size) {

    BIN* joj=MKJOJ("D:\\jojtest.joj", 3, size);
    ENCJOJ(joj, pixels); BINCLOSE(joj); DLMEM(joj);

};

//   ---     ---     ---     ---     ---