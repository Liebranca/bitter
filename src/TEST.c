#include "KVRNEL/MEM/KVR_BIN.h"
#include "KVRNEL/MEM/KVR_COM.h"
#include "KVRNEL/LEX/KVR_INTPRT.h"

#include <stdio.h>

#define COMLEN 70

void main() {

    COM c = {0};

    char buff [COMLEN];
    GTIN(buff, COMLEN);

    MKCOM(&c, buff);

}