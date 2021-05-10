#include "KVRNEL/LEX/kvr_intprt.h"
#include "KVRNEL/TYPES/zjc_hash.h"

#ifdef main
#undef main
#endif

void main() {

    __openlog();

    HASH* h=MKHASH(1, "TABLE");
    int* p=NULL; int values[16];

    char* keys[16]={"x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7",
                    "y0", "y1", "y2", "y3", "y4", "y5", "y6", "y7"};

    for(int i=0; i<16; i++) {

        values[i]=i;

        STR_HASHSET(h, keys[i], values+i);
        STR_HASHGET(h, keys[i], p, 1);

        STR_HASHSET(h, keys[i], values+i);
        STR_HASHGET(h, keys[i], p, 0);

        if(p) { CALOUT("%i\n\b", *p); }

    }

    /* testing interpreter

    COM c; SETINCOM(&c); SETINBUFF("FOR<int> t, (0,5,2) { x++; } "); TKIN();

    for(int i=0; i<c.token_count;i++) { fprintf(log, "%s ", c.tokens[i]); }

    */

    DLMEM(byref(h->m));
    __closelog();

}