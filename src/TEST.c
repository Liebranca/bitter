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


#include "KVRNEL/kvr_paths.h"
#include "CHASM/chMNG.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

int main(int argc, char *argv[])            {

    char* paths[4]={
        "D:\\lieb_git\\KVR\\trashcan\\log\\",
        "D:\\lieb_git\\KVR\\trashcan\\log\\",
        "D:\\lieb_git\\KVR\\trashcan\\log\\",
        "D:\\lieb_git\\KVR\\trashcan\\log\\"
    };

    NTKVRPTH(paths);
    NTCHMNG("SINx8", 1);

    while(GTCHMNGRUN()) {
        FRBEGCHMNG();
        FRENDCHMNG();
        SLEEPCHMNG();
    };

    DLCHMNG();
    DLKVRPTH();

    return 0;

};

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
};
#endif