#include "KVRNEL/LEX/kvr_intprt.h"
#include "KVRNEL/TYPES/zjc_hash.h"

#ifdef main
#undef main
#endif

typedef struct TEST_S { int x; } TEST;

void main() {

    __openlog();

    // testing hash tables

    HASH h; MKHASH(&h, 6, "TABLE0");

    int x[8]={0}; char* keys[8]={ "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7" };
    for(int i=0; i<8; i++) { x[i]=i; HASHSET(byref(h), keys[i], x+i); };

    int* p;
    for(int i=7; i>-1; i--)                 { HASHGET(byref(h), keys[i], p, int, 1);
        if(!p)                              { CALOUT("%s\n\b", "FAIL");                     }
        else                                { CALOUT("%s is %i\n\b", keys[i], *p);          };
                                                                                            };

    for(int i=0; i<8; i++)                  { HASHGET(byref(h), keys[i], p, int, 0);
        if(!p)                              { CALOUT("%s\n\b", "FAIL");                     }
        else                                { CALOUT("%s is %i\n\b", keys[i], *p);          };
                                                                                            };

    /* testing interpreter

    COM c; SETINCOM(&c); SETINBUFF("FOR<int> t, (0,5,2) { x++; } "); TKIN();

    for(int i=0; i<c.token_count;i++) { fprintf(log, "%s ", c.tokens[i]); }

    */

    MEMFREE(byref(h.m), byref(h));
    __closelog();

}