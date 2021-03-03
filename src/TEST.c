#include "KVRNEL/MEM/KVR_BIN.h"

#include <stdio.h>

void main() {

    BIN b     = {0};
    b.path    = "D:\\lieb_git\\KVR\\src\\TEST.uu";
    b.mode    = "wb+";

    int isnew = 0;

    BINOPEN ((&b), isnew); printf("%i\n", isnew);
    BINCLOSE((&b));

}