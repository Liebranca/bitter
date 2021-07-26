#include "zjc_joj.h"
#include "kvr_zwrap.h"

#include "../KVRNEL/MEM/kvr_str.h"

#include <string.h>

//   ---     ---     ---     ---     ---

BIN* MKJOJ(char* path,
           uint  mode,
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

    BIN* bin; int evilstate; BINOPEN(bin, path, 0, 1, 0, evilstate);
    uint size; int rb; BINREAD_ATR(bin, rb, uint, 1, &size, sizeof(SIG));

    DLMEM(bin); return MKJOJ(path, 0, size);                                                };

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
    uint bytesize=((size_squared*1)*sizeof(JOJPIX));
    BINWRITE(joj, wb, uint, 1, &size);

    uint start=0;

//   ---     ---     ---     ---     ---

    for(uint z=0; z<4; z++) {               // loop through the four layers

        start=z*size_squared*4;

        for(uint x=start, y=0;
            x<start+(size_squared*4);
            x+=4, y++               )       //convert rgba to jojpix and store in buff

        { JOJPIX cpix=rgba_to_joj(pixels+x); buff[y]=cpix; };
        BINWRITE(joj, wb, JOJPIX, size_squared, buff);

        break;

    };

//   ---     ---     ---     ---     ---

    BIN* dst; int evilstate; BINOPEN(dst, "D:\\jojcomp.joj", 3, 1, 0, evilstate);
    uint deflsize=0; DEFLBIN(joj, dst, bytesize, &deflsize, sizeof(uint), sizeof(uint)*2);

    rewind(dst->file);
    fseek(dst->file, 0,           SEEK_CUR);
    fseek(dst->file, sizeof(SIG), SEEK_CUR);
    fseek(dst->file, 0,           SEEK_CUR);

    BINWRITE(dst, wb, uint, 1, &size    );
    BINWRITE(dst, wb, uint, 1, &deflsize);

    DECJOJ(dst, pixels);
    BINCLOSE(dst); DLMEM(dst);

    return 0;                                                                               };

//   ---     ---     ---     ---     ---

void DECJOJ(BIN* src, float* pixels)        {

    uint size=0;
    uint deflsize=0;

    int wb; BIN* joj;

    rewind(src->file);
    fseek(src->file, 0,           SEEK_CUR);
    fseek(src->file, sizeof(SIG), SEEK_CUR);
    fseek(src->file, 0,           SEEK_CUR);

    fseek(src->file, 0,           SEEK_CUR);
    BINREAD(src, wb, uint, 1, &size    );
    BINREAD(src, wb, uint, 1, &deflsize);
    fseek(src->file, 0,           SEEK_CUR);

    char* srcpath=PTHBIN(src);
    MEM* path=MKSTR(srcpath, 4); RPSTR(path, "_dec.joj", path->bsize-9);
    joj=MKJOJ((char*) path->buff, 3, size); DLMEM(path);

    uint size_squared=size*size;
    uint bytesize=((size_squared*1)*sizeof(JOJPIX));
    INFLBIN(src, joj, bytesize, deflsize, sizeof(uint), sizeof(uint)*2);

    rewind(joj->file);
    fseek(joj->file,           0, SEEK_CUR);
    fseek(joj->file, sizeof(SIG)+sizeof(uint), SEEK_CUR);
    fseek(joj->file,           0, SEEK_CUR);

//   ---     ---     ---     ---     ---

    JOJPIX* buff;
    uint* loc_p=GTJOJ(joj); size=*loc_p;
    loc_p++; buff=(JOJPIX*) loc_p;

    for(uint z=0; z<4; z++) {               // loop through the four layers

        uint start=z*size_squared*4;        // read from joj
        BINREAD(joj, wb, JOJPIX, size_squared, buff);

        for(uint x=start, y=0;
            x<start+(size_squared*4);
            x+=4, y++               )       //convert jojpix to rgba and modify pixels

        { float* pixel = pixels+x; joj_to_rgba(pixel, buff+y); };

        break;

    };

//   ---     ---     ---     ---     ---

    BINCLOSE(joj); DLMEM(joj);                                                              };

//   ---     ---     ---     ---     ---

int DEFLJOJ(char** paths,
            uint   num_paths,
            uint   size     )               {

    BIN* bin; int evilstate=0;

    return 0;                                                                               };

//   ---     ---     ---     ---     ---