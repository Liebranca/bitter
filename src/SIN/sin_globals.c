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

uint SIN_EVILSTATE    = 0;

uint SIN_WSIZX        = 640;
uint SIN_WSIZY        = 480;

static HASH* SIN_HASH = NULL;

void NTSINHASH(void)                        { SIN_HASH=MKHASH(SIN_HASHSIZ, "SIN_HASH");     };
void DLSINHASH(void)                        { DLMEM(SIN_HASH);                              };

//   ---     ---     ---     ---     ---

int STSINHASH (ID* data)                    { HASHSET(SIN_HASH, data);                      };

int GTSINHASH (ID* data, void** dst,
               int pop             )        { HASHGET(SIN_HASH, data, dst, pop);            };
                                    
int FNDSINHASH(char* key,void** dst,
               int pop             )        { STR_HASHGET(SIN_HASH, key, dst, pop);         };

//   ---     ---     ---     ---     ---