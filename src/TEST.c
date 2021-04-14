#include "KVRNEL/LEX/KVR_INTPRT.h"

#ifdef main
#undef main
#endif

#include <stdio.h>

#define COMLEN 70

typedef struct TEST_S { int x; } TEST;

void main() {

    // FOR<TEST*> t, T(0,10)
    // 1: for
    // 2: (TYPE name=ARR+start; name<ARR+end;
    // for(TEST* t=T+0; t<T+10; t++)

    COM c; SETINCOM(&c); SETINBUFF("FOR<int> t, (0,5,2) { x++; } "); TKIN();
    FILE* log;

    log = fopen ("D:\\lieb_git\\KVR\\trashcan\\log\\KVNSLOG", "w+");
    for(int i=0; i<c.token_count;i++) { fprintf(log, "%s ", c.tokens[i]); }

    fclose(log);

}