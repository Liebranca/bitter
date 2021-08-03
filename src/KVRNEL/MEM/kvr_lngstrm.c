/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    KVR_LNGSTRM                           *
*                                           *
*     -a longstream                         *
*     -lng for short                        *
*     -wip for a 'live' fread               *
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

#include "kvr_bin.h"

//   ---     ---     ---     ---     ---

BIN* MKLNGSTRM(char* path, char* name)      {

    BIN* lng;
    int  evilstate;
    int  i=0;

    BINOPEN                                 (lng, path, KVR_FMODE_RB, KVR_FTYPE_LNG,
                                             ZJC_DAFSIZE, evilstate                );

    static char* type="STRM";

    do { lng->m.id.type[i] = *type; i++;  } while  (*type++                        ); i=0;
    do { lng->m.id.key [i] = *name; i++;  } while  (*name++                        );

    return lng;

};

//   ---     ---     ---     ---     ---