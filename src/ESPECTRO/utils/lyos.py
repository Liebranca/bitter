#   ---     ---     ---     ---     --- #
#   lyos.py                             #
#                                       #
#   shorthands and aliases for os       #
#   and sys calls; some path ops        #
#                                       #
#   ---     ---     ---     ---     --- #

from   msvcrt   import getch;
from   pathlib  import Path;

import os, sys, atexit;

sys.ps1 = '>';
sys.ps2 = '-';

#   ---     ---     ---     ---     ---

hxDEBUG = True;

def DEBUG_PRINT(d=None):

    global hxDEBUG;

    if d:
        hxDEBUG = d;
        return None;

    return hxDEBUG;

hxWARNS = True;

def FATAL_WARNINGS(d=None):

    global hxWARNS;

    if d:
        hxWARNS = d;
        return None;

    return hxWARNS;

#   ---     ---     ---     ---     ---

hxROOT = "";

def ROOT(r=None):

    global hxROOT;

    if r:
        hxROOT = r;
        return None;

    return hxROOT;

#   ---     ---     ---     ---     ---

MCHUNK = 0;
def READSIZE(s=None):

    global MCHUNK;

    if s:
        MCHUNK = s;
        return None;

    return MCHUNK;

#   ---     ---     ---     ---     ---

CCPATH   = "";
CC64     = "";
PLATFORM = "";

def CC(c=None, c64=None, p=None):

    global CCPATH, CC64, PLATFORM;

    if c and c64 and p:
        CCPATH = c; CC64 = c64; PLATFORM = p;
        return None;

    elif PLATFORM == "x64":
        return CCPATH + CC64 + "\\bin\\"

    return CCPATH + "\\bin\\"

def TARGET(p=None):

    global PLATFORM;

    if not p:
        return PLATFORM;

    if p in ["64", "x64", 0]: PLATFORM = "x64";
    else                    : PLATFORM = "x86";

    if DEBUG_PRINT(): print("TARGET%s"%PLATFORM);

#   ---     ---     ---     ---     ---

def DOS(comm): os.system(comm );
def CLS(    ): DOS      ('cls');

#   ---     ---     ---     ---     ---

def OKPATH(path):
    return os.path.isdir (path) if os.path.exists(path) else False;

def OKFILE(path):
    return os.path.isfile(path) if os.path.exists(path) else False;

def FILESIZE(path):

    if OKFILE(path) : return os.path.getsize(path);
    elif DEBUG_PRINT(): print("Can't get size of <%s> -- unexistent or not a file"%path)

    return 0;

def DIRSIZE(path):

    if OKPATH(path) : return os.path.getsize(path);
    elif DEBUG_PRINT(): print("Can't get size of <%s> -- unexistent or not a directory"%path)

    return 0;

def WALK(path):
    return list(os.walk(path));

def MKDIR(path):
    os.mkdir(path);

def CATPATH(*args):
    return "\\".join(s for s in args);

#   ---     ---     ---     ---     ---

def DELF(path):

    if OKFILE(path):
        DOS("@del %s"%path);
        if DEBUG_PRINT(): print("Deleted file <%s>"%path);

    elif DEBUG_PRINT():
        print("Error deleting <%s>"%path);

def DELD(path):

    if OKPATH(path):
        DOS("@rmdir %s"%path);
        if DEBUG_PRINT(): print("Deleted directory <%s>"%path);

    elif DEBUG_PRINT():
        print("Error deleting <%s>"%path);

#   ---     ---     ---     ---     ---

def TOUCH(f):
    Path(f).touch();

def MODTIME(path):
    return os.path.getmtime(path);

def MTIMEVS(f1, f2):
    return MODTIME(f1) != MODTIME(f2);

#   ---     ---     ---     ---     ---

KVRLOG   = None;

def CLEANUP():
    global KVRLOG; DELF(KVRLOG);
    sys.exit();

def STARTUP(SETTINGS):

    r         = SETTINGS[0]; ROOT           (r              );

    ccd       = SETTINGS[2];
    ccd64     = SETTINGS[3];
    p         = SETTINGS[4]; CC             (ccd, ccd64, p  );

    read_size = SETTINGS[5]; READSIZE       (read_size      ); 
    warnfatal = SETTINGS[6]; FATAL_WARNINGS (warnfatal      );
    debugprnt = SETTINGS[7]; DEBUG_PRINT    (debugprnt      );

    global KVRLOG;

    logpath = ROOT() + "\\KVR\\trashcan\\log";
    if not OKPATH(logpath): MKDIR(logpath);

    KVRLOG = logpath + "\\KVNSLOG";
    open(KVRLOG, 'w').close();

    DOS('TITLE %__SLAVE%%_PLATFORM% (ESPECTRO)');
    atexit.register(CLEANUP);

def LOGOS():
    global KVRLOG; return KVRLOG;

#   ---     ---     ---     ---     ---

def SYSFLUSH():
    global KVRLOG;
    open(KVRLOG, 'w').close();

def SYSREAD(clear=1):
    global KVRLOG;

    f     = open(KVRLOG, 'r+');
    lines = '\n'.join(line for line in f);

    if clear: f.truncate(0);
    f.close(); return lines;

def SYSDUMP():
    pass;

#   ---     ---     ---     ---     ---

kBACKSPACE = 8;

def IN(ret = 13):

    char = '\x00'; usin = ''; prmt = "\r>"
    print("%s"%prmt, end = '');

    while( ord(char) != ret ):

        char = getch();

        if char:

            x    = ord(char);
            if x == kBACKSPACE:
                usin = usin[0:-1]; CLS();

            else:
                char = (char.decode('utf-8')).upper();
                usin = usin + char;

            print("%s%s"%(prmt, usin), end = '');

    return usin;

#   ---     ---     ---     ---     ---