#   ---     ---     ---     ---     --- #
#   ESPECTRO                            #
#                                       #
#   console rat module makes dev env    #
#                                       #
#   ---     ---     ---     ---     --- #

from .utils.lyos import (

    ROOT,

    DOS,
    CLS,
    IN,

    WALK,

    MKDIR,
    DELF,
    DELD,

    OKPATH,
    CATPATH,

    OKFILE,
    FILESIZE,
    READSIZE,

    TOUCH,
    MTIMEVS,

    SYSREAD,
    SYSDUMP,
    LOGOS,

    STARTUP,

    CC,
    TARGET,

    FATAL_WARNINGS

);

#   ---     ---     ---     ---     ---

from .utils.lystr     import clnstr, clnpth, hxchr;
from .utils.lylist    import LFLAT, LKEYS, lyDICT;

from .utils.multilang import ALL_LANGS, COMP_LANG, MKSRC, MKHED;

from .utils.console   import CHOICE, MULTICHOICE;

from .builder import (

    AVTO_SETOUT,
    AVTO_SETIN,
    AVTO_SETINC,
    AVTO_SETLIB,

    AVTO_CHKFILE,
    AVTO_CLEAN,

    AVTO_MKEXE,
    AVTO_MKLIB,
    AVTO_MKDLL,

);

from .HXPX            import hxPX as PX;

#   ---     ---     ---     ---     ---
