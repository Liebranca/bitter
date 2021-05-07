#include "KVRNEL/LEX/kvr_intprt.h"
#include "KVRNEL/TYPES/zjc_hash.h"

#include <time.h>

#ifdef main
#undef main
#endif

void main() {

    __openlog();

    // testing hash tables

    HASH h; MKHASH(&h, 8, "TABLE0");

    int x[8]={0}; int* p;
    LKUP keys[9]={
        NITKEY("x0"), NITKEY("x1"), NITKEY("x2"), NITKEY("x3"),
        NITKEY("x4"), NITKEY("x5"), NITKEY("x6"), NITKEY("x7")
    };

    for(int i=0; i<8; i++) {
        x[i]=i; HASHSET(byref(h), byref(keys[i]), x+i);
        HASHGET(byref(h), byref(keys[i]), p, int, 0)
        if(p) { CALOUT("%i\n\b", *p); }

    };

    /* testing interpreter

    COM c; SETINCOM(&c); SETINBUFF("FOR<int> t, (0,5,2) { x++; } "); TKIN();

    for(int i=0; i<c.token_count;i++) { fprintf(log, "%s ", c.tokens[i]); }

    */

    MEMFREE(byref(h.m), byref(h));
    __closelog();

}