#ifndef __ZJC_BLCRK_H__
#define __ZJC_BLCRK_H__

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

#include "../KVRNEL/MEM/kvr_bin.h"
#include "../KVRNEL/TYPES/zjc_BinTypes.h"

#define CRK_ENCODE 0
#define CRK_DECODE 1

//   ---     ---     ---     ---     ---

typedef struct CRKH {                       // header for bcrk files

    char   name[KVR_IDK_WIDTH];             // id for this mesh

    ushort vert_count;                      // mesh dimensions
    ushort idex_count;

    uint   fracl;                           // compression level

} CRKH;

//   ---     ---     ---     ---     ---

BIN*     MKCRK   (char* path, uint  mode  );
CRKVRT*  GTCRK   (BIN* crk                );

int      NTCRKENG(int mode                );
int      DLCRKENG(int mode                );

int      ENCCRK  (CRKH* crkh, uint* size_i);

int      ZPCRK   (uint size_i, uint count,\
                  CRKH* crkh              );

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif //__ZJC_BLCRK_H__