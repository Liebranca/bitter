/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    SIN_GLOBALS                           *
*                                           *
*     -defines some constants               *
*     -sin datamang                         *
*                                           */
// *   ---     ---     ---     ---     ---  *
/*    LIBRE SOFTWARE                        *
*                                           *
*     Licenced under GNU GPL3               *
*     be a bro and inherit                  *
*                                           */
// *   ---     ---     ---     ---     ---  *
/* CONTRIBUTORS                             *
*     lyeb,                                 *
*                                           */
/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/

#include "sin_globals.h"

//   ---     ---     ---     ---     ---

static HASH* SIN_HASH = NULL;

void NTSINHASH()                            { SIN_HASH=MKHASH(SIN_HASHSIZ, "SIN_HASH");     };
void DLSINHASH()                            { DLMEM(SIN_HASH);                              };

//   ---     ---     ---     ---     ---

int STSINHASH (ID* data)                    { HASHSET(SIN_HASH, data);                      };

int GTSINHASH (ID* data, void** dst,
               int pop             )        { HASHGET(SIN_HASH, data, dst, pop);            };
                                    
int FNDSINHASH(char* key,void** dst,
               int pop             )        { STR_HASHGET(SIN_HASH, key, dst, pop);         };

//   ---     ---     ---     ---     ---