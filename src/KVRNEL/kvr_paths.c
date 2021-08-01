#include "kvr_paths.h"
#include "MEM/kvr_bin.h"
#include "MEM/kvr_str.h"

#include <string.h>

//   ---     ---     ---     ---     ---

static BIN* CALLOG     = NULL;              // handle to the log dump

//   ---     ---     ---     ---     ---

#define PECTROPATH 0                        // deactivates my errlog redirection

void __openlog()                            {

    #if PECTROPATH
    CALLOG->file=freopen                    (PTHBIN(CALLOG), "w+", stderr);
    #endif

#if KVR_DEBUG & KVR_CALOK
    CALOUT                                  ('K', "\n\bKVR RUN\n\b"      );                 };
#endif

void __closelog()                           {

    FILE* f=CALLOG->file;

    DLMEM                                   (CALLOG                );

#if KVR_DEBUG & KVR_CALOK
    CALOUT                                  ('K', "KVR END\n\b\n\b");
#endif

    #if PECTROPATH
    fclose                                  (f                     );
    #endif

                                                                                            };

//   ---     ---     ---     ---     ---

#define DEFN_KVR_PATH(NAME)                                                                  \
                                                                                             \
    static MEM*  NAME;                                                                       \
                                                                                             \
    void     NT##NAME (char* buff  )        { NAME = MKSTR  (buff, 0, 0   );                }\
    char*    ST##NAME (char* buff  )        { STSTR         (&NAME, buff  );                }\
    char*    GT##NAME (            )        { return MEMBUFF(NAME, char, 0);                }

//   ---     ---     ---     ---     ---

DEFN_KVR_PATH         (ROOT        );
DEFN_KVR_PATH         (CWD         );
DEFN_KVR_PATH         (PEROOT      );
DEFN_KVR_PATH         (PECWD       );

// [...]

void NTKVRPTH(char** pth_l)                 {

    NTROOT  (pth_l[0]);
    NTCWD   (pth_l[1]);
    NTPEROOT(pth_l[2]);
    NTPECWD (pth_l[3]);

    char* fname = "KVNSLOG";

    CALLOG      = MKBIN(GTROOT(), 3, 2, strlen(fname)+16);
    char* path  = PTHBIN(CALLOG); int y=0;

    do { ;                       } while(*path++ ); path-=1;
    do { path[y]=*fname; y++;    } while(*fname++);

    __openlog();

};

void DLKVRPTH        (            )         {

    DLMEM         (ROOT         );
    DLMEM         (CWD          );
    DLMEM         (PEROOT       );
    DLMEM         (PECWD        );

    __closelog    (             );                                                          };

//   ---     ---     ---     ---     ---

