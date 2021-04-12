#include "KVRNEL/LEX/KVR_INTPRT.h"

#ifdef main
#undef main
#endif

#include <stdio.h>

#define COMLEN 70

void main() {

    COM c; SETINCOM(&c); SETINBUFF("IF x<y THEN x<5 { x++; } "); TKIN();
    FILE* fp;

    fp = fopen ("D:\\lieb_git\\KVR\\trashcan\\log\\KVNSLOG", "w+");
    for(int i=0; i<c.token_count;i++) { fprintf(fp, "%s ", c.tokens[i]); }

    fclose(fp);

}