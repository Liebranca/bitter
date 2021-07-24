#include "../KVRNEL/zjc_evil.h"
#include "../KVRNEL/TYPES/zjc_BinTypes.h"

#include <stdio.h>

EXPORT void RNCOL(float* pixels, uint size) {

    for(uint x=0; x<size;x+=4) {
        JOJPIX joj=rgba_to_joj(pixels+x); joj_to_rgba(pixels+x, &joj);
    };

};
