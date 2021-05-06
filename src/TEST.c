#include "KVRNEL/LEX/kvr_intprt.h"
#include "KVRNEL/TYPES/zjc_hash.h"

#include <time.h>

#ifdef main
#undef main
#endif

typedef struct TEST_S { int x; } TEST;

const  float   CPS         = 1.0f / CLOCKS_PER_SEC;
static clock_t framebegin  = 0;
static clock_t frameend    = 0;

float clock_calcDelta() { return (frameend - framebegin) * CPS; }

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
        HASHGET(byref(h), byref(keys[i]), p, int, 0)
        CALOUT("%i\n\b", *p);

    };

    framebegin=clock(); int y=0;
    for(int i=0; i<4096*8*25; i++) {
        HASHSET(byref(h), byref(keys[y]), x+y);
        HASHGET(byref(h), byref(keys[y]), p, int, 0); y++; if(y>7) {y=0;} }

    frameend=clock();

    CALOUT("%fsecs taken for HASHGET\n\b", clock_calcDelta());

    /* testing interpreter

    COM c; SETINCOM(&c); SETINBUFF("FOR<int> t, (0,5,2) { x++; } "); TKIN();

    for(int i=0; i<c.token_count;i++) { fprintf(log, "%s ", c.tokens[i]); }

    */

    MEMFREE(byref(h.m), byref(h));
    __closelog();

}