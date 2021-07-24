#include "zjc_joj.h"
#include <string.h>

//   ---     ---     ---     ---     ---

BJOJ* MKJOJ(char* path,
            char* mode,
            uint  size)                     {

    BIN* bin; uint ex_alloc=sizeof(uint)+((size*size)*sizeof(JOJPIX));
    int evilstate; BINOPEN(bin, path, mode, 1, ex_alloc, evilstate);

    BJOJ* joj=(BJOJ*) bin;

    joj->size=size;
    joj->buffloc=strlen(path)+1;

    return joj;                                                                             };

BJOJ* LDJOJ(char* path)                     {

    BIN* bin; int evilstate; BINOPEN(bin, path, "rb", 1, 0, evilstate);
    uint size; int rb; BINREAD_ATR(bin, rb, uint, 1, &size, sizeof(SIG));

    DLMEM(bin); return MKJOJ(path, "rb", size);                                             };

JOJPIX* GTJOJ(BJOJ* joj)                    {

    return MEMBUFF(

               byref(joj->src.m), JOJPIX,
               joj->buffloc+sizeof(uint)

           );                                                                               };

//   ---     ---     ---     ---     ---

int DEFLJOJ(char*  path,
            float* pixels,
            uint   size  )                  {

    BIN* bin; int evilstate=0;

};

//   ---     ---     ---     ---     ---