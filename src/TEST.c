/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    TEST                                  *
*                                           *
*     -blank entry point                    *
*     -keep it tidy                         *
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

#ifdef main
#undef main
#endif

#include "../KVRNEL/kvr_paths.h"

#include "CHASM/chMNG.h"

void main(void)                             {

    char* paths[4]={
        "D:\\lieb_git\\KVR\\trashcan\\log\\",
        "D:\\lieb_git\\KVR\\trashcan\\log\\",
        "D:\\lieb_git\\KVR\\trashcan\\log\\",
        "D:\\lieb_git\\KVR\\trashcan\\log\\"
    };

    NTKVRPTH(paths);
    NTCHMNG("SINx8", 0);

    while(GTCHMNGRUN()) {
        FRBEGCHMNG();
        FRENDCHMNG();
        SLEEPCHMNG();
    };

    DLCHMNG();
    DLKVRPTH();

};