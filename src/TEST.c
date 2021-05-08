#include "KVRNEL/LEX/kvr_intprt.h"
#include "KVRNEL/TYPES/zjc_hash.h"

#ifdef main
#undef main
#endif

void main() {

    __openlog();

    // testing hash tables

    HASH h; MKHASH(&h, 8, "TABLE0");

    int x[8]={0}; int* p;
    LKUP keys[8]={
        NITKEY("x0"), NITKEY("x1"), NITKEY("x2"), NITKEY("x3"),
        NITKEY("x4"), NITKEY("x5"), NITKEY("x6"), NITKEY("x7")
    };

    for(int i=0; i<8; i++) {
        x[i]=i; STR_HASHSET(byref(h), keys[i].key, x+i);

    };

    for(int i=0; i<8; i++) {
        HASHGET(byref(h), byref(keys[i]), p, 1);
        CALOUT("%i: %i %i\n\b", keys[i].idex, keys[i].mod, *p);

    };


    /* testing interpreter

    COM c; SETINCOM(&c); SETINBUFF("FOR<int> t, (0,5,2) { x++; } "); TKIN();

    for(int i=0; i<c.token_count;i++) { fprintf(log, "%s ", c.tokens[i]); }

    */

    MEMFREE(byref(h.m), byref(h));
    __closelog();

}