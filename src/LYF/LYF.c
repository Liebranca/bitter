#include "zjc_joj.h"

#include <stdio.h>

EXPORT void RNCOL(float* pixels, uint size) {

    __openlog();

    BIN* joj=MKJOJ("D:\\jojtest.joj", "wb+", size);
    ENCJOJ(joj, pixels); BINREMOVE(joj); DLMEM(joj);

    __closelog();

};
