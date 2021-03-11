#   ---     ---     ---     ---     --- #
#   lyos.py                             #
#                                       #
#   shorthands and aliases for os       #
#   and sys calls; some path ops        #
#                                       #
#   ---     ---     ---     ---     --- #

from   msvcrt   import getch;
from   pathlib  import Path;

import os, sys, atexit, time;

sys.ps1 = '>';
sys.ps2 = '-';

#   ---     ---     ---     ---     ---

WCHROMA =                                   {

     0: ";2;0;0;0m",                        # black
     1: ";2;128;0;0m",                      # red
     2: ";2;0;128;0m",                      # green
     3: ";2;128;128;0m",                    # yellow
     4: ";2;0;0;128m",                      # blue
     5: ";2;128;0;128m",                    # magenta
     6: ";2;0;128;128m",                    # cyan
     7: ";2;128;128;128m",                  # white

    10: ";2;64;0;0m",                       # bright black
    11: ";2;255;0;0m",                      # bright red
    12: ";2;0;255;0m",                      # bright green
    13: ";2;255;255;0m",                    # bright yellow
    14: ";2;0;0;255m",                      # bright blue
    15: ";2;255;0;255m",                    # bright magenta
    16: ";2;0;255;255m",                    # bright cyan
    17: ";2;255;255;255m",                  # bright white

                                            };

def ANSIC(f=1, b=1):

    if f > 17:
        f = "\x1b[39m";
    else:
        f = f"\x1b[38{WCHROMA[f]}";

    if b > 17:
        b = "\x1b[49m";

    else:
        b = f"\x1b[48{WCHROMA[b]}";

    return f + b;

#   ---     ---     ---     ---     ---

hxDEBUG  = -1;
hxEPRINT =  1;

def DEBUG_PRINT(d=None):

    global hxDEBUG;

    if d:
        hxDEBUG = d;
        return None;

    return hxDEBUG;

def DISABLE_EPRINT(d=None):

    global hxEPRINT;

    if d:
        hxEPRINT = d;
        return None;

    return hxEPRINT;

hxNVRASK = 0;

def DISABLE_CONFIRM(d=None):

    global hxNVRASK;

    if d:
        hxNVRASK = d;
        return None;

    return hxNVRASK;

ERRLEVELS =                                 {

   -1: "SATANIC",
    0: "SYS",
    1: "WRNG",
    2: "ERR",
    3: "FTL_ERR"

                                            };

PALETTE   =                                 {

   "IN": "\x1b[0m",
   "BX": "\x1b[0m",

   -1  : "\x1b[0m",
    0  : "\x1b[0m",
    1  : "\x1b[0m",
    2  : "\x1b[0m",
    3  : "\x1b[0m"

                                            };

import inspect;

def QLNAME(frame):

    code = frame.f_code;
    name = code.co_name;
    qual = None;

    for cls in ( obj for obj in frame.f_globals.values()  #
                     if  inspect.isclass(obj)             ):

        if hasattr(cls, name):

            member = getattr(cls, name);
            if not inspect.isfunction(member): continue;

            if member.__code__ == code:
                qual = member.__qualname__;
                break;

    return qual;

def GETERR(err):
    return PALETTE[err] + ERRLEVELS[err];

def CPRINT(s, f=0):
    print(s, sep='', end='', flush=f);

def ERRPRINT(*args, err=0, rec=1, sep=' ', end=''):

    global hxEPRINT;
    if not hxEPRINT: return None;

#   ---     ---     ---     ---     ---

    global hxDEBUG; rec = max(1, min(rec, 7));
    mess = f"{sep.join(s for s in args)}{end}";

    prev   = sys._getframe(rec);
    kls    = QLNAME(prev);

    caller = prev.f_code.co_name if not kls else kls;
    file   = prev.f_code.co_filename.replace(f"{ROOT()}\\", "").split("\\");

    if rec > 1:
        last     = sys._getframe(1);
        reporter = last.f_code.co_name;

    else:
        reporter = caller;

    mess   = mess.replace("$CALLER", caller  );
    mess   = mess.replace("$LAST",   reporter);

#   ---     ---     ---     ---     ---

    if hxDEBUG <= err:

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
        mess   = f"{GETERR(err)}: {caller} on {file} @{line}\n" + mess;

    else:
        mess = f"{GETERR(err)}: {mess}"

#   ---     ---     ---     ---     ---

    print(mess + "\x1b[0m");

hxWARNS = True;

def FATAL_WARNINGS(d=None):

    global hxWARNS;

    if d:
        hxWARNS = d;
        return None;

    return hxWARNS;

hxWALL = False;

def FKWALL(d=None):

    global hxWALL;

    if d:
        hxWALL = d;
        return None;

    return hxWALL;

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
    else                    : PLATFORM = "Win32";

    ERRPRINT("TARGET%s"%PLATFORM);

hxBTIME = 0.0;
def BTIME(f=None):

    global hxBTIME;
    if f:

        if f == 'rnow': f = time.time();

        hxBTIME = f;
        return None;

    return hxBTIME;

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

    if exts:

        flist  = os.listdir(path);
        select = [];

        for e in exts:
            l = [f for f in flist if f".{e}" in f];
            if len(l): select.extend(l);

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

def MKDIR(path, recursive=1):

    if recursive:

        d = path.split("\\"); r = d[0]; 
        for sp in d[1:]:
            r = r + f"\\{sp}";
            if not OKPATH(r): os.mkdir(r);

        return 0;

    else:

        try:
            os.mkdir(r); return 0;

        except:
            ERRPRINT(f"$LAST can't create dir <{path}> because the path is invalid",
                      "$CALLER may have meant to turn on the recursive flag?",
                      err=2, rec=2, sep="\n"                                       );

            return 1;

#   ---     ---     ---     ---     ---

def SEP_DIRFRFILE(path):
    return "\\".join( x for x in (path.split("\\")[:-1]));

def SEP_FILEFRDIR(path):
    return path.split("\\")[-1];

#   ---     ---     ---     ---     ---

def FLFILE(path, ask=1):

    global hxNVRASK;
    if hxNVRASK: ask=0;

    if not OKFILE(path):
        ERRPRINT(f"File <{SHPATH(path)}> doesn't exist, cannot flush", err=2, rec=2);

    else:

        if ask:
            i = CHOICE(f"Erase contents of <{SHPATH(path)}> ?")
            if i == 2:
                ERRPRINT(f"Denied $CALLER-requested flush", rec=2);
                return None;

        open(path, 'w').close();

#   ---     ---     ---     ---     ---

def RDFILE(path, b=0, rh=0, rl=0, dec=0, rl_sep="\n", trunc=0, ask=1):

    global hxNVRASK;
    if hxNVRASK: ask=0;

    if not OKFILE(path):
        ERRPRINT(f"File <{SHPATH(path)}> doesn't exist, cannot read", err=2, rec=2);
        return (None, None);

    if trunc and rh: rh = 0;
    if dec and not b: dec = 0;

    size = FILESIZE(path);
    if ask:
        i = CHOICE(f"File <{SHPATH(path)}> is over ({(READSIZE()/1024):.2f}KB)\n",
                   f"Read anyway?"                                               );

        if i == 2:
            ERRPRINT("$CALLER attempted reading large file, but $LAST denied", rec=2);
            return None;

#   ---     ---     ---     ---     ---

    if not rl:

        if b:
            m = 'rb+' if trunc else 'rb';
            d = b"";

        else:
            m = 'r+' if trunc else 'r';
            d = "";

#   ---     ---     ---     ---     ---

        file = None; ERRPRINT(f"Opened file <{SHPATH(path)}>", rec=2);
        if rh:

            file = open(path, m);
            while size:

                rb = min(size, READSIZE()); # bytes to read clamped to bytes left

                s  = file.read(rb);         # read, cat, substract
                d  = d + s; size -= rb;

        else:

            with open(path, m) as file:

                while size:

                    rb = min(size, READSIZE());

                    s  = file.read(rb);
                    d  = d + s; size -= rb;

                if trunc: file.truncate(0);

            ERRPRINT(f"Closed file <{SHPATH(path)}>", rec=2);

#   ---     ---     ---     ---     ---

        if dec:

            try:
                d = d.decode('ascii');

            except UnicodeDecodeError:
                d = d.decode                ( 'utf-8'                                       );
                ERRPRINT                    ( "Your staunch defiance of ASCII is",          #
                                              "looked down uppon by dark forces unknown...",#
                                              err=-1, rec=2                                 );

        if rh: return (d, file);
        return (d, None);

#   ---     ---     ---     ---     ---

    file  = None; ERRPRINT(f"Opened file <{SHPATH(path)}>", rec=2);
    file  = open(path, 'r+');
    lines = rl_sep.join(line for line in file);

    if trunc:
        file.truncate(0);

    if not rh:
        file.close();
        ERRPRINT(f"Closed file <{SHPATH(path)}>", rec=2);

    return (lines, file);

#   ---     ---     ---     ---     ---

def MKFILE(path, s="", b=0, rh=0, ask=0):

    global hxNVRASK;
    if hxNVRASK: ask=0;

    d = SEP_DIRFRFILE(path);

    if not OKPATH(d):

        i = CHOICE(f"Unexistent directory <{SHPATH(d)}>. Create?");

        if i == 1:
            MKPATH(d);

        else:
            ERRPRINT("BAD PATH: $LAST cannot create file for $CALLER", err=2, rec=2);
            return None;

    if OKFILE(path) and ask:
        i = CHOICE(f"File <{SHPATH(path)}> already exists. Overwrite?");

        if i == 2:
            ERRPRINT("$CALLER attempted overwrite, but $LAST denied", rec=2);
            return None;

#   ---     ---     ---     ---     ---

    success_mess = f"$CALLER created file <{SHPATH(path)}>";
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
                                              err=-1, rec=2                                 );

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
    return '\\'.join(s for s in args);

#   ---     ---     ---     ---     ---

def DELF(path, ext=0):

    if OKFILE(path):

        DOS("@del %s"%path);
        ERRPRINT(f"$CALLER deleted file <{SHPATH(path)}>", rec=3 if ext else 2);

    else: ERRPRINT(f"Error deleting <{SHPATH(path)}>", err=2, rec=3 if ext else 2);

def DELD(path):

    if OKPATH(path):

        flist = LISTDIR(path);
        if len(flist):
            i = CHOICE(f"Directory <{SHPATH(path)}> is not empty.\n"\
                       +"All files in it will be lost. Is that OK?" );

            if i == 1:
                for f in flist:
                    DELF(f"{path}\\{f}", 1);

            else:
                ERRPRINT(f"$LAST aborted deletion of <{SHPATH(path)}>", rec=2);

        DOS("@rmdir %s"%path);
        ERRPRINT(f"$CALLER deleted directory <{SHPATH(path)}>", rec=2);

    else: ERRPRINT(f"Error deleting <{SHPATH(path)}>", err=2, rec=2);

#   ---     ---     ---     ---     ---

def FKTOUCH(f):
    t = BTIME();
    os.utime(f, (t, t));

def TOUCH(f):
    Path(f).touch();

def MODTIME(path):
    return os.path.getmtime(path);

def MTIMEVS(f1, f2):
    return MODTIME(f1) != MODTIME(f2);

def MTIMELT(flist):

    t      = MODTIME(flist[0]);
    lesser = flist[0];

    for f in flist[1:]:
        new_t = MODTIME(f);
        if new_t < t:
            t      = new_t;
            lesser = f;

    return lesser;

def MTIMEGT(flist):

    t       = MODTIME(flist[0]);
    greater = flist[0];

    for f in flist[1:]:
        new_t = MODTIME(f)
        if new_t > t:
            t       = new_t;
            greater = f;

    return greater;

#   ---     ---     ---     ---     ---

KVRLOG   = None;

def CLEANUP():
    global KVRLOG; DELF(KVRLOG);

def STARTUP(SETTINGS):

    PALETTE["IN"] = ANSIC(12, 4);

    PALETTE[-1  ] = ANSIC(11,  4);
    PALETTE[ 0  ] = ANSIC(16,  4);
    PALETTE[ 1  ] = ANSIC(18,  3);
    PALETTE[ 2  ] = ANSIC( 7,  1);
    PALETTE[ 3  ] = ANSIC(17, 11);

    r             = SETTINGS[ 0]; ROOT           (r              );

    ccd           = SETTINGS[ 2];
    ccd64         = SETTINGS[ 3];
    p             = SETTINGS[ 4]; CC             (ccd, ccd64, p  );

    read_size     = SETTINGS[ 5]; READSIZE       (read_size      ); 
    warnfatal     = SETTINGS[ 6]; FATAL_WARNINGS (warnfatal      );
    debugprnt     = SETTINGS[ 7]; DEBUG_PRINT    (debugprnt      );
    disblprnt     = SETTINGS[ 8]; DISABLE_EPRINT (not disblprnt  );
    nvrask        = SETTINGS[ 9]; DISABLE_CONFIRM(nvrask         );
    fkwall        = SETTINGS[10]; FKWALL         (fkwall         );

    global KVRLOG;

    logpath = ROOT() + "\\KVR\\trashcan\\log";
    if not OKPATH(logpath): MKDIR(logpath);

    KVRLOG = logpath + "\\KVNSLOG"; MKFILE(KVRLOG, ask=0);

    DOS('TITLE %__SLAVE%%_PLATFORM% (ESPECTRO)');

    atexit.register(CLEANUP);

def LOGOS():
    global KVRLOG; return KVRLOG;

#   ---     ---     ---     ---     ---

def SYSFLUSH():
    global KVRLOG; FLFILE(KVLOG, 0);

def SYSREAD(clear=1):
    global KVRLOG;

    global hxEPRINT; hxEPRINT = 0;

    try:
        log = RDFILE(KVRLOG, rl=1, trunc=clear, ask=0)[0];
    finally:
        hxEPRINT = 1;

    return log;

def SYSDUMP():
    pass;

#   ---     ---     ---     ---     ---

class KEYBOARD_CONTROLLER:

    LOG  = 27;

    KEYS =                                  {

        "BACKSPACE" :  8,
        "RETURN"    : 13,
        "EXIT"      : 26,
        "ESCAPE"    : 27,

        "ARROWLEFT" : 77,
        "ARROWRIGHT": 75,
        "ARROWUP"   : 72,
        "ARROWDOWN" : 80,

        "SPACE"     : 32,

        "DELETE"    : 83,
        "START"     : 71,
        "END"       : 79,
        "SEMICOLON" : 59,

        "ORDM"      : 167,

        "SPEC"      : 224

                                            };

#   ---     ---     ---     ---     ---

    @staticmethod
    def RUN(cases={}, spec_cases={}, typewriter=None):

        KEYS = KEYBOARD_CONTROLLER.KEYS;
        VAL  = list(KEYS.values());
        ID   = list(KEYS.keys  ());

        retx = None;

        while not retx:

            char = getch().upper(); x = ord(char);
            KEYBOARD_CONTROLLER.LOG = x;

            if x == KEYS["SPEC"]:
                y = ord(getch());

                if y not in VAL:
                    continue;

                i = VAL.index(y); key = ID[i];

                if key in spec_cases:
                    retx = spec_cases[key]();

            elif typewriter and (91 > x >= 65):
                typewriter();

            else:

                if x not in VAL:
                    continue;

                i = VAL.index(x); key = ID[i];

                if key in cases:
                    retx = cases[key]();

            CPRINT("", 1);

        return retx;

#   ---     ---     ---     ---     ---

class ASCIBOX:

    CHARS =                                 {

        "VV": " ││║║█",
        "VR": " ├╞╟╠▌",
        "VL": " ┤╡╢╣▐",
        "VS": " ·░▒▓█",
        "TL": " ┌╒╓╔▄",
        "BL": " └╘╙╚▀",
        "TR": " ┐╕╖╗▄",
        "BR": " ┘╛╜╝▀",
        "HH": " ─═─═■",
        "HD": " ┬╤╥╦▄",
        "CC": " ┼╪╫╬█",
        "HU": " ┴╧╨╩▀"

                                            };

    @staticmethod
    def MAKE(self, items, title="BOX", ptrchar_l='♦', ptrchar_r='♦', offset=0, pad=0,
             sel=5, thick=2, col="IN", rev=1, align=0, t_align=2, p_align=2):

        self = ASCIBOX();

        if not pad:
            pad = len(title) + 4;

        newpad = 0;
        for s in items:
            if len(s) > pad:
                pad = len(s);
                newpad = 1;

        if newpad:
            pad += 4;
            if pad%2: pad += 1;

#   ---     ---     ---     ---     ---

        self.title     = title;
        self.pad       = pad;

        self.items     = items;
        self.sel       = items[sel];
        self.ptr       = sel;
        self.rev       = rev;

        self.thick     = min(thick, 5);
        self.col       = PALETTE[col];

        print(self.col, "\n", "\n");

        self.ioffset   = offset;
        self.align     = align;

        self.t_align   = t_align;
        self.p_align   = p_align;
        self.ptrchar_l = ptrchar_l;
        self.ptrchar_r = ptrchar_r;

#   ---     ---     ---     ---     ---

        CPRINT("\x1b[?25l");
        self.drawTop(); self.drawMid("");

        for i in items:
            self.drawMid(i);

        self.drawMid(""); self.drawBottom();

        CPRINT(f"\x1b[{(len(self.items) + 2) - self.ptr}F");
        return self.RUN();

#   ---     ---     ---     ---     ---

    def RUN(self):

        spec_cases =                        {

            "ARROWUP"  : self.UP,
            "ARROWDOWN": self.DOWN,
                                            };

        cases =                             {

            "RETURN"   : self.SELECT,
            "ESCAPE"   : self.CANCEL,
            "EXIT"     : self.CANCEL,
            "ORDM"     : self.CANCEL,

                                            };

        return KEYBOARD_CONTROLLER.RUN(cases, spec_cases);

#   ---     ---     ---     ---     ---

    def KILL(self):
        CPRINT("\x1b[?25h");
        CPRINT(f"\x1b[{(len(self.items) + 2) - self.ptr}E");

    @property
    def offset(self):
        return " " * self.ioffset;

    @property
    def start_line(self):
        return self.col;

    @property
    def close_line(self):
        return "\x1b[0m\n";

    def drawTop(self):

        CHARS = ASCIBOX.CHARS; CPRINT(self.offset + self.start_line
                                     +  CHARS["TL"][self.thick]     );

        hchar = CHARS["HH"][self.thick] if self.thick < 5 else CHARS["TR"][5];

        space = self.pad;
        if   self.t_align == 1:
            wsp_e = "]" ; space -= 1;
            space -= len(self.title); wsp_s = (hchar * (space - 1)) + ("[");

        elif self.t_align == 2:
            w = int((self.pad)/2) - int(len(self.title)/2);
            wsp_s = (hchar * (w-2)) + ("["); space -= (w-1);
            space -= len(self.title); wsp_e = ("]") + (hchar * (space - 1));

        else:
            wsp_s = "["; space -= 1;
            space -= len(self.title); wsp_e = ("]") + (hchar * (space - 1));

        CPRINT(wsp_s + self.title + wsp_e); CPRINT(CHARS["TR"][self.thick]
                                                  + self.close_line, 1   );

    def drawMid(self, i):
        CHARS = ASCIBOX.CHARS; CPRINT(self.offset + self.start_line
                                     + CHARS["VV"][self.thick]      );
        if i == self.sel:
            i = self.itemAligned_sel(i);
        else:
            i = self.itemAligned(i);

        CPRINT(i); CPRINT(CHARS["VV"][self.thick]
                         + self.close_line, 1   );

    def drawBottom(self):
        CHARS = ASCIBOX.CHARS; CPRINT(self.offset + self.start_line
                                     + CHARS["BL"][self.thick]      );

        hchar = CHARS["HH"][self.thick] if self.thick < 5 else CHARS["BR"][5];
        CPRINT(hchar * self.pad); CPRINT(CHARS["BR"][self.thick]
                                        + self.close_line, 1   );

    def itemAligned_sel(self, i):

        length = len(i);

        pdch_s = self.ptrchar_l if self.p_align in [2, 0] else " ";
        pdch_e = self.ptrchar_r if self.p_align in [2, 1] else " ";

        space = self.pad;
        if   self.align == 1:
            wsp_e = "\x1b[27m" + pdch_e + " "; space -= 2;
            space -= length; wsp_s = " " + pdch_s + "\x1b[7m" + (" " * (space - 2));

        elif self.align == 2:
            w = int((self.pad)/2) - int(length/2);
            wsp_s = " " + pdch_s + "\x1b[7m" + (" " * (w - 2)); space -= w;
            space -= length; wsp_e = (" " * (space - 2)) + "\x1b[27m" + pdch_e + " ";

        else:
            wsp_s = " " + pdch_s + "\x1b[7m"; space -= 2;
            space -= length; wsp_e = (" " * (space - 2)) + "\x1b[27m" + pdch_e + " ";

        return wsp_s + i + wsp_e;

    def itemAligned(self, i):

        space = self.pad; length = len(i);
        if   self.align == 1:
            wsp_e = "  "; space -= 2;
            space -= length; wsp_s = " " * space;

        elif self.align == 2:
            w = int((self.pad)/2) - int(length/2);
            wsp_s = " " * (w); space -= w;
            space -= length; wsp_e = " " * space;

        else:
            wsp_s = "  "; space -= 2;
            space -= length; wsp_e = " " * space;

        return "\x1b[27m" + wsp_s + i + wsp_e;

    def UP(self):
        if not self.ptr > 0: return;

        CPRINT(f"\x1b[G\x1b[{self.ioffset}C");
        CPRINT(self.start_line + self.itemAligned(self.sel) + "\x1b[0m");
        
        self.ptr -= 1; self.sel = self.items[self.ptr];
        CPRINT(f"\x1b[F\x1b[{self.ioffset}C");
        CPRINT(self.start_line + self.itemAligned_sel(self.sel) + "\x1b[0m", 1);

    def DOWN(self):
        if not self.ptr < (len(self.items) - 1): return;

        CPRINT(f"\x1b[G\x1b[{self.ioffset}C");
        CPRINT(self.start_line + self.itemAligned(self.sel) + "\x1b[0m");
        
        self.ptr += 1; self.sel = self.items[self.ptr];
        CPRINT(f"\x1b[E\x1b[{self.ioffset}C");
        CPRINT(self.start_line + self.itemAligned_sel(self.sel) + "\x1b[0m", 1);

    def SELECT(self):
        self.KILL(); return self.ptr + 1;

    def CANCEL(self):
        self.KILL(); return -1;

#   ---     ---     ---     ---     ---

class INPUTFIELD:

    @staticmethod
    def MAKE(length=64, col="IN", prompt='$:'):

        self        = INPUTFIELD();

        self.col    = PALETTE[col];
        self.prompt = prompt;
        self.length = length;

        self.WIPE(); self.FCOLUMN();
        CPRINT("\x1b[?25l", 1);
        return self.RUN();

    @property
    def ch(self):
        if not len(self.buff): return " ";
        return self.buff[self.ptr] if self.ptr < len(self.buff) else " ";

    def KILL(self):
        self.LCOLUMN(); CPRINT(f"\x1b[27m{self.col}{self.ch}\x1b[D");
        CPRINT("\x1b[?25h\x1b[0m\n");

    def RUN(self):

        spec_cases =                        {

            "ARROWLEFT" : self.RIGHT,
            "ARROWRIGHT": self.LEFT,
            "DELETE"    : self.DELCUR,
            "START"     : self.FCOLUMN,
            "END"       : self.LCOLUMN
                                            };

        cases =                             {

            "RETURN"    : self.SELECT,
            "ESCAPE"    : self.CANCEL,
            "EXIT"      : self.CANCEL,
            "ORDM"      : self.CANCEL,

            "SPACE"     : self.PASTECHAR,
            "SEMICOLON" : self.PASTECHAR,
            "BACKSPACE" : self.DELPREV,

                                            };

        return KEYBOARD_CONTROLLER.RUN(cases, spec_cases, self.PASTECHAR);

    def WIPE(self):
        self.ptr = 0; self.buff = ""; self.LINEDRAW();

    def LINEDRAW(self):
        CPRINT(f"\x1b[G{self.col}{self.prompt}{self.buff}"\
               f"{' ' * (self.length - len(self.buff))}"  );

    def RIGHT(self):
        if self.ptr == len(self.buff): return;
        CPRINT(f"\x1b[27m{self.col}{self.ch}\x1b[7m"); self.ptr += 1;
        CPRINT(f"\x1b[7m{self.col}{self.ch}\x1b[27m\x1b[D");

    def LEFT(self):
        if self.ptr == 0: return;
        CPRINT(f"\x1b[27m{self.col}{self.ch}\x1b[2D"); self.ptr -= 1;
        CPRINT(f"\x1b[7m{self.col}{self.ch}\x1b[27m\x1b[D");

    def FCOLUMN(self):
        CPRINT(f"\x1b[27m{self.col}{self.ch}\x1b[7m"); self.ptr = 0;
        CPRINT(f"\x1b[{len(self.prompt) + 1}G");
        CPRINT(f"\x1b[7m{self.col}{self.ch}\x1b[27m\x1b[D");

    def LCOLUMN(self):

        CPRINT(f"\x1b[27m{self.col}{self.ch}\x1b[7m"); self.ptr = len(self.buff);
        CPRINT(f"\x1b[{len(self.prompt)+ 1 + len(self.buff)}G");
        CPRINT(f"\x1b[7m{self.col}{self.ch}\x1b[27m\x1b[D");

    def PASTECHAR(self):

        x = KEYBOARD_CONTROLLER.LOG;
        if len(self.buff) < self.length:
            ch = chr(x)
            self.buff = self.buff[:self.ptr] + ch + self.buff[self.ptr:]; self.ptr += 1;
            CPRINT(f"\x1b[27m{self.col}{ch}\x1b[7m");
            CPRINT(f"\x1b[7m{self.col}{self.ch}\x1b[27m\x1b[D");

    def DELPREV(self):

        if self.ptr > 0:

            self.ptr -= 1; self.buff = self.buff[:self.ptr] + self.buff[self.ptr+1:] + " ";
            tail      = self.buff[self.ptr:];

            CPRINT(f"\x1b[27m{self.col}{self.ch}\x1b[2D\x1b[7m");
            CPRINT(f" \b\x1b[27m{self.col}{tail}"); CPRINT(f"\x1b[{len(tail)}D");
            CPRINT(f"\x1b[7m{self.col}{self.ch}\x1b[27m\x1b[D");
            self.buff = self.buff[:-1];

        else:
            self.DELCUR();

    def DELCUR(self):

        if len(self.buff) and self.ptr != len(self.buff):

            old_length   = len(self.buff);
            self.buff    = self.buff[:self.ptr] + self.buff[self.ptr+1:] + " ";
            tail         = self.buff[self.ptr:];

            if self.ptr  == old_length:
                self.ptr -= 1;

            CPRINT(f"\x1b[27m{self.col}{tail}\x1b[{len(tail)}D\x1b[7m");
            self.buff = self.buff[:-1];

            CPRINT(f"\x1b[7m{self.col}{self.ch}\x1b[27m\x1b[D");

    def CANCEL(self):
        self.buff = "EXIT"; self.ptr = len(self.buff);
        self.LINEDRAW(); self.KILL(); return "EXIT";

    def SELECT(self):
        self.KILL(); return self.buff;

#   ---     ---     ---     ---     ---

def TESTVCHOICE():

    ops = ["PISS", "SHIT", "FUCK", "CUNT", "COCKSUCKER", "MOTHERFUCKER", "TITS"];
    pad = 0;

    box = ASCIBOX(ops, title="SEVEN DIRTY WORS");
    i   = box.RUN(); i = i-1 if i > 0 else i;

    if i > 0: print(ops[i]);
    else    : print("CANCELLED");

    del box;

def PEIN():
    return INPUTFIELD.MAKE();