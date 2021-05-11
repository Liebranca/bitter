#include "KVRNEL/LEX/kvr_intprt.h"
#include "KVRNEL/TYPES/zjc_hash.h"

#ifdef main
#undef main
#endif

typedef struct TEST_S {

    ID  id;
    int data;

} T; T TMAKE(char* key, int value) { T t={ IDNEW("TEST", key), value}; return t; }

void main() {

    __openlog();

    HASH* h=MKHASH(16, "TABLE");
    T* p=NULL; T values[16];

    char* keys[16]={"x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7",
                    "y0", "y1", "y2", "y3", "y4", "y5", "y6", "y7"};

    for(int i=0; i<16; i++) {

        values[i]=TMAKE(keys[i], i);
        HASHSET(h, byref(values[i]));

        HASHGET(h, byref(values[i].id), p, 0);
        STR_HASHGET(h, keys[i], p, 0);

        if(p) { CALOUT("%s: [%i, %i]\n\b", p->id.key, p->id.x, p->id.y); }

    };

    h=GWHASH(h);

    for(int i=0; i<16; i++) {

        CALOUT("%s: [%i, %i]\n\b", values[i].id.key, values[i].id.x, values[i].id.y);

    };

    /* testing interpreter

    COM c; SETINCOM(&c); SETINBUFF("FOR<int> t, (0,5,2) { x++; } "); TKIN();

    for(int i=0; i<c.token_count;i++) { fprintf(log, "%s ", c.tokens[i]); }

    */

    DLMEM(byref(h->m));
    __closelog();

}