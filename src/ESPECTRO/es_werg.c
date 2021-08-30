/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    ES_WERG                               *
*                                           *
*     -its werg                             *
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

#include "es_werg.h"

#include "KVRNEL/kvr_paths.h"
#include "CHASM/chMNG.h"

#include "SIN/__sin.h"
#include "SIN/sin_texture.h"
#include "SIN/REND/sin_canvas.h"

#include "KVNSL/es_pecon.h"

//   ---     ---     ---     ---     ---

int main(int argc, char** argv) {

    char* paths[4]={
        "D:\\lieb_git\\KVR\\trashcan\\log\\",
        "D:\\lieb_git\\KVR\\trashcan\\log\\",
        "D:\\lieb_git\\KVR\\trashcan\\log\\",
        "D:\\lieb_git\\KVR\\trashcan\\log\\"
    };

    NTKVRPTH(paths     );
    NTCHMNG ("SINx8", 0);
    NTSIN   (          );
    NTPCON  (          );

    while(GTCHMNGRUN()) {

        int evilstate=FRBEGCHMNG();
        if(evilstate) { break; };

        HICON();

        FRENDCHMNG();
        SLEEPCHMNG();

    };

    DLPCON  ();
    DLSIN   ();
    DLCHMNG ();
    DLKVRPTH();

    return 0;

};

//   ---     ---     ---     ---     ---
