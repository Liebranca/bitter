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

ERRLEVELS =                                 {

    0: "SYS",
    1: "WRNG",
    2: "ERR",
    3: "FTL_ERR",
    4: "SATANIC"

                                            };

def ERRPRINT(*args, err=0, rec=1, sep=' ', end='\n'):

    global hxDEBUG; rec = max(1, min(rec, 7));
    mess = f"{sep.join(s for s in args)}{end}";
    
    if hxDEBUG:

        prev   = sys._getframe(rec);

        caller = prev.f_code.co_name;
        file   = prev.f_code.co_filename.replace(f"{ROOT()}\\", "").split("\\");

        x      = len(file);

        if x == 3:
            file = f"<{file[0]}> {file[2]}";

        elif x == 4:
            file = f"<{file[2]}> {file[3]}";

        elif x == 5:
            file = f"<{file[2]}\\{file[3]}> {file[4]}";

        else:
            file = file[-1];

        line   = prev.f_lineno;

        if rec > 1:
            last     = sys._getframe(1);
            reporter = last.f_code.co_name;

        else:
            reporter = caller;

        mess   = mess.replace("$CALLER", caller  );
        mess   = mess.replace("$LAST",   reporter);

        mess   = f"{ERRLEVELS[err]}: {caller} on {file} @{line}\n" + mess;

    else:

        mess =                              (

                         mess.replace       ("$CALLER", "Caller")
                    if   mess.startswith    ("$CALLER"          )

                    else mess.replace       ("$CALLER", "caller") 

                                            );

        mess =                              (

                         mess.replace       ("$LAST", "Reporter")
                    if   mess.startswith    ("$LAST"            )

                    else mess.replace       ("$LAST", "reporter") 

                                            );

    print(mess);

#   ---     ---     ---     ---     ---

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
    else                    : PLATFORM = "x32";

    ERRPRINT("TARGET%s"%PLATFORM);

#   ---     ---     ---     ---     ---

def DOS(comm): os.system(comm );
def CLS(    ): DOS      ('cls');

#   ---     ---     ---     ---     ---

def SHPATH(path):
    return path.replace(f"{ROOT()}", "ROOT");

def OKPATH(path):
    return os.path.isdir (path) if os.path.exists(path) else False;

def OKFILE(path):
    return os.path.isfile(path) if os.path.exists(path) else False;

def FILESIZE(path):

    if OKFILE(path) : return os.path.getsize(path);
    else: ERRPRINT(f"<{SHPATH(path)}> -- unexistent or not a file", err=2, rec=2)

    return 0;

def DIRSIZE(path):

    if OKPATH(path) : return os.path.getsize(path);
    else: ERRPRINT(f"<{SHPATH(path)}> -- unexistent or not a directory", err=2, rec=2)

    return 0;

def WALK(path):
    return list(os.walk(path));

def LISTDIR(path, exts=[]):
    if ext: return [x for x in os.listdir(path) if len(x) >= 4 and  x[-4:] in exts];
    return os.listdir(path);

def EXTSOLVE(name, full):

    if len(name) >= 4 and len(full) >= 4:
        if name == full[:-4]: return 1;

    return 0;

#   ---     ---     ---     ---     ---

CHOICE_RECMAX = 20;

def CHOICE(mess, choices = ["Y", "N"], repeat = 0):

    choices = [ch.upper() for ch in choices];
    chs     = "\\".join(choices);

    chb     = [bytes(ch, 'ascii') for ch in choices];

    if not repeat:
        print(mess + " [" + chs + "]");

    c   = getch().upper();
    if c in chb:
        return 1 + chb.index(c);

    elif repeat < CHOICE_RECMAX:
        return CHOICE(mess, choices, repeat + 1);

    else:
        ERRPRINT("$LAST exceeded number of invalid inputs", err=2, rec=2);
        return 0;

alpha = [ bytes(chr(x), 'ascii') for x in range(65, 91) ];
def MULTICHOICE(mess, items, repeat = 0):

    if not repeat:

        print(mess); i = 0;
        for item in items:

            print("%s) %s"%(alpha[i].decode('ascii'), item));
            i += 1;

    c = getch().upper();
    if c in alpha:
        return 1 + alpha.index(c);

    elif repeat < CHOICE_RECMAX:
        return MULTICHOICE(mess, items, repeat + 1);

    else:
        ERRPRINT("$LAST exceeded number of invalid inputs", err=2, rec=2);
        return 0;

#   ---     ---     ---     ---     ---

def MKDIR(path):

    d = path.split("\\"); recurs = d[0]; 
    for sp in d[1:]:
        recurs = recurs + f"\\{sp}";
        if not OKPATH(recurs): os.mkdir(recurs);

#   ---     ---     ---     ---     ---

def FLFILE(path, ask=1):

    if not OKFILE(path):
        ERRPRINT(f"File <{SHPATH(path)}> doesn't exist, cannot flush");

    else:

        if ask:
            i = CHOICE(f"Erase contents of <{SHPATH(path)}> ?")
            if i == 2:
                ERRPRINT(f"Denied $CALLER-requested flush", rec=2);
                return None;

        open(path, 'w').close();

def MKFILE(path, s="", b=0, rh=0):

    d = "\\".join( x for x in (path.split("\\")[:-1]);

    if not OKPATH(d):

        i = CHOICE(f"Unexistent directory <{SHPATH(d)}>. Create?");

        if i == 1:
            MKPATH(d);

        else:
            ERRPRINT("BAD PATH: $LAST cannot create file for $CALLER", err=2, rec=2);
            return None;

    if OKFILE(path):
        i = CHOICE(f"File <{SHPATH(d)}> already exists. Overwrite?");

        if i == 2:
            ERRPRINT("$CALLER attempted overwrite, but $LAST denied", rec=2);
            return None;

#   ---     ---     ---     ---     ---

    success_mess = f"Created file <{SHPATH(path)}> for $CALLER";
    m = 'wb' if b else 'w';

    if s:

        if b:
            if type(s) != bytes:

                try:
                    s = bytes(s, encoding='ascii')

                except UnicodeEncodeError:

                    s = bytes               ( s, encoding='utf-8'                           );
                    ERRPRINT                ( "Your staunch defiance of ASCII is",          #
                                              "looked down uppon by dark forces unknown...",#
                                              err=4, rec=2                                  );

#   ---     ---     ---     ---     ---

        if rh:
            file.open(path, m);
            file.write(s); ERRPRINT(success_mess, rec=2);

            return file;                    # danger, no auto-close hook
                                            # TODO: handle this with a class maybe

        with open(path, m) as file:
            file.write(s); ERRPRINT(success_mess, rec=2);

        return 1;

#   ---     ---     ---     ---     ---

    if rh:
        open(path, m); ERRPRINT(success_mess, rec=2);

        return file;                        # same problem as above

    open(path, m).close(); ERRPRINT(success_mess, rec=2);
    return 1;

def CATPATH(*args):
    return "\\".join(s for s in args);

#   ---     ---     ---     ---     ---

def DELF(path):

    if OKFILE(path):
        DOS("@del %s"%path);
        ERRPRINT(f"Deleted file <{SHPATH(path)}>", rec=2);

    else: ERRPRINT(f"Error deleting <{SHPATH(path)}>", err=2, rec=2);

def DELD(path):

    if OKPATH(path):
        DOS("@rmdir %s"%path);
        ERRPRINT(f"Deleted directory <{SHPATH(path)}>", rec=2);

    else: ERRPRINT(f"Error deleting <{SHPATH(path)}>", err=2, rec=2);

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

    KVRLOG = logpath + "\\KVNSLOG"; MKFILE(KVRLOG);

    DOS('TITLE %__SLAVE%%_PLATFORM% (ESPECTRO)');
    atexit.register(CLEANUP);

def LOGOS():
    global KVRLOG; return KVRLOG;

#   ---     ---     ---     ---     ---

def SYSFLUSH():
    global KVRLOG; FLFILE(KVLOG, 0);

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