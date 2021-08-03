#ifndef __ZJC_ID_H__
#define __ZJC_ID_H__

#include "../zjc_CommonTypes.h"

#define KVR_IDK_WIDTH 20
#define KVR_IDT_WIDTH  4

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

typedef struct ZJC_BLOCK_HEADER {           // base struct to identify otherwise wild voids

    union {

        struct {
            char type[KVR_IDT_WIDTH];       // four chars describing the block
            char key [KVR_IDK_WIDTH];       // for fetching/inserting on table

        };  char full[KVR_IDT_WIDTH
                     +KVR_IDK_WIDTH];       // type + key

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