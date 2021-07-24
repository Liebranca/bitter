#include "zjc_joj.h"
#include "kvr_zwrap.h"

#include <string.h>

//   ---     ---     ---     ---     ---

BIN* MKJOJ(char* path,
           char* mode,
           uint  size)                     {

    BIN* joj;

    uint ex_alloc=(

            ( sizeof(uint)               )  // for storing size
        +   ( (size*size)*sizeof(JOJPIX) )  // the color buffer itself

        );

    int evilstate; BINOPEN(joj, path, mode, 1, ex_alloc, evilstate);

    path        = PTHBIN(joj);

    uint  loc   = (strlen(path)+1) * sizeof(char);
    uint* loc_p = (uint*) (path+loc);
    *loc_p      = size;

    return joj;                                                                             };

BIN* LDJOJ(char* path)                      {

    BIN* bin; int evilstate; BINOPEN(bin, path, "rb", 1, 0, evilstate);
    uint size; int rb; BINREAD_ATR(bin, rb, uint, 1, &size, sizeof(SIG));

    DLMEM(bin); return MKJOJ(path, "rb", size);                                             };

uint* GTJOJ(BIN* joj)                       {

    char* path  = PTHBIN(joj);
    uint  loc   = (strlen(path)+1) * sizeof(char);

    uint* loc_p = (uint*) (path+loc);

    return loc_p;                                                                           };

//   ---     ---     ---     ---     ---

int ENCJOJ(BIN* joj, float* pixels)         {

    uint size; JOJPIX* buff;
    uint* loc_p=GTJOJ(joj); size=*loc_p;
    loc_p++; buff=(JOJPIX*) loc_p;

    uint size_squared=size*size; int wb;
    uint bytesize=sizeof(uint)+((size_squared*1)*sizeof(JOJPIX));

    for(uint z=0; z<4; z++) {               // loop through the four layers

        uint start=z*size_squared*4;

        for(uint x=start, y=0;
            x<start+(size_squared*4);
            x+=4, y++               )       //convert rgba to jojpix and store in buff

        { JOJPIX cpix=rgba_to_joj(pixels+x); buff[y]=cpix; };

        if(!z) {                            // store joj size on first run, then buff
            BINWRITE_ATR(joj, wb, uint, 1, &size, sizeof(SIG));

        }; BINWRITE(joj, wb, JOJPIX, size_squared, buff);

        break;

    };

    BIN* dst; int evilstate; BINOPEN(dst, "D:\\jojcomp.joj", "wb+", 1, 0, evilstate);
    uint deflsize=0; DEFLBIN(joj, dst, bytesize, &deflsize);

    BINCLOSE(dst); DLMEM(dst);

    return 0;                                                                               };

int DEFLJOJ(char** paths,
            uint   num_paths,
            uint   size     )               {

    BIN* bin; int evilstate=0;

    return 0;                                                                               };

//   ---     ---     ---     ---     ---