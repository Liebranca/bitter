#   ---     ---     ---     ---     --- #
#   ESPECTRO                            #
#                                       #
#   console rat module makes dev env    #
#                                       #
#   ---     ---     ---     ---     --- #

from . import PYZJC;

from .utils.lyos import (

    getch,

    ROOT,
    BTIME,
    GETKVNSL,

    DOS,
    CLS,
    PEIN,

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
    AVTO_SETINC,
    AVTO_LIBDIR,
    AVTO_LIBBIN,

    AVTO_CHKFILE,
    AVTO_CHKDEPS,
    AVTO_CLEAN,

    AVTO_MKEXE,
    AVTO_MKLIB,
    AVTO_MKDLL,

);

from .HXPX            import hxPX;
from .lang.KVR        import INTPRT;

#   ---     ---     ---     ---     ---
