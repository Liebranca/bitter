#   ---     ---     ---     ---     --- #
#   ESPECTRO                            #
#                                       #
#   console rat module makes dev env    #
#                                       #
#   ---     ---     ---     ---     --- #

from . import PYZJC;

from .utils.lyos import (

    getch,
    CHDIR,
    CWD,

    ROOT,
    BTIME,
    GETKVNSL,
    GETKVRSOR,

    COORD,
    REGION,
    LAYOUT,

    INPUTFIELD,
    TOGGLEFIELD,
    PSEUDOPOP,
    SHUFFLER,
    CONFIRMER,

    DOS,
    CLS,

    CPRINT,
    ERRPRINT,

    WALK,
    LISTDIR,
    EXTSOLVE,

    MKDIR,
    MKFILE,
    RDFILE,
    FLFILE,

    DELF,
    DELD,

    OKPATH,
    CATPATH,
    SHPATH,

    OKFILE,
    FILESIZE,
    READSIZE,

    TOUCH,
    FKTOUCH,
    MODTIME,
    MTIMEVS,
    MTIMELT,
    MTIMEGT,

    SYSREAD,
    SYSDUMP,
    LOGOS,

    STARTUP,
    CLEANUP,

    CC,
    TARGET,

    CHOICE,
    MULTICHOICE,

    FKWALL,
    FATAL_WARNINGS

);

#   ---     ---     ---     ---     ---

from .utils.lystr     import clnstr, clnpth, hxchr;
from .utils.lylist    import LFLAT, LKEYS, lyDICT;

from .utils.multilang import ALL_LANGS, COMP_LANG, MKSRC, MKHED;

from .builder import (

    AVTO_SETOUT,
    AVTO_SETIN,
    AVTO_INCLUDES,
    AVTO_LIBS,

    AVTO_CHKFILE,
    AVTO_CHKDEPS,
    AVTO_CLEAN,

    AVTO_MKEXE,
    AVTO_MKLIB,
    AVTO_MKDLL,

);

from .HXPX            import hxPX, SCANLIBS, PXUI;
from .lang.KVR        import INTPRT;

#   ---     ---     ---     ---     ---
