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
        x[i]=i; HASHSET(byref(h), byref(keys[i]), x+i);

    };

    HASHGET(byref(h), byref(keys[5]), p, 1); int z=9;
    LKUP k2=NITKEY("x9"); STR_HASHSET(byref(h), k2.key, &z);
    keys[5]=k2; int y=0; int runs=10000000;

    timer_start();
    for(int i=0; i<runs; i++) {
        STR_HASHGET(byref(h), keys[y].key, p, 0); if(y>7) { y=0; }

    }; CALOUT("STR_HASHGET:    %fsec\n\b", timer_end());

    timer_start();
    for(int i=0; i<runs; i++) {
        HASHGET(byref(h), byref(keys[y]), p, 0); if(y>7) { y=0; }

    }; CALOUT("HASHGET:        %fsec\n\b", timer_end());

    /* testing interpreter

    COM c; SETINCOM(&c); SETINBUFF("FOR<int> t, (0,5,2) { x++; } "); TKIN();

    for(int i=0; i<c.token_count;i++) { fprintf(log, "%s ", c.tokens[i]); }

    */

    MEMFREE(byref(h.m), byref(h));
    __closelog();

}