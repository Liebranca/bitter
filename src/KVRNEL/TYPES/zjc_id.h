#ifndef __ZJC_ID_H__
#define __ZJC_ID_H__

#include "../zjc_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct ZJC_BLOCK_HEADER {           // base struct to identify otherwise wild voids

    union {

        struct {
            char type[ 4];                  // four chars describing the block
            char key [20];                  // for fetching/inserting on table

        };  char full[24];                  // type + key

    };

    sint x;                                 // table bucket index
    sint y;                                 // subindex into bucket array

} ID;

//   ---     ---     ---     ---     ---

ID IDNEW(char* type, char* key);            // call on block init


//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif //__ZJC_ID_H__