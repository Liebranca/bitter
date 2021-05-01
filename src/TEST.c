#include "KVRNEL/LEX/kvr_intprt.h"
#include "KVRNEL/TYPES/zjc_hash.h"

#ifdef main
#undef main
#endif

typedef struct TEST_S { int x; } TEST;

void main() {

    __openlog();

    // testing hash tables

    HASH h; MKHASH(&h, 64, "TABLE0");
    int x=666;
    for(int i=0; i<9; i++) { ADHASH(&h, "x", &x); }; // TODO: catch full

    int* p=(int*) GTHASH(&h, "x", 0);

    if(!p) { CALOUT("FAIL");        }
    else   { CALOUT("p is %i", *p); };

    /* testing interpreter

    COM c; SETINCOM(&c); SETINBUFF("FOR<int> t, (0,5,2) { x++; } "); TKIN();

    for(int i=0; i<c.token_count;i++) { fprintf(log, "%s ", c.tokens[i]); }

    */

    MEMFREE(byref(h.m), byref(h));
    __closelog();

}