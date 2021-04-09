#   ---     ---     ---     ---     ---   #
#   builder.py                            #
#                                         #
#   project build avtomat                 #
#                                         #
#   ---     ---     ---     ---     ---   #

from ESPECTRO import (

    DOS,
    ERRPRINT,

    TOUCH,
    FKTOUCH,
    MTIMEVS,
    MTIMELT,
    MTIMEGT,

    OKFILE,
    LISTDIR,
    EXTSOLVE,
    CATPATH,
    TARGET,

    SYSREAD,
    LOGOS,

    FKWALL,
    FATAL_WARNINGS
);

#   ---     ---     ---     ---     ---

OBJFOLD  = "";
SRCFOLD  = "";
INCLUDES = "";
LIBS     = "";

def AVTO_SETOUT(s):
    global OBJFOLD; OBJFOLD = s;

def AVTO_SETIN (s):
    global SRCFOLD; SRCFOLD = s;

def AVTO_INCLUDES(s, ap=0):
    global INCLUDES;
    if ap:
        INCLUDES = INCLUDES+" "+s;
    else:
        INCLUDES = s;

def AVTO_LIBS(s, ap=0):
    global LIBS;
    if ap:
        LIBS = LIBS+" "+s;
    else:
        LIBS = s;

#   ---     ---     ---     ---     ---

def AVTO_CLEAN(ext):
    DOS(f"@del {OBJFOLD}\\*.{ext}");

def AVTO_SRCBUILD(src, obj, gcc):

    if OKFILE(obj): DOS(f"@del {obj}");     # delete pre-existing .o
    ERRPRINT(src.split("\\")[-1], err=-1); _w = " -Wall " if FKWALL() else " "

    s = f"@{gcc}{_w}-MMD {INCLUDES} -c {src} -o {obj} 2> {LOGOS()}";
    DOS(s); mess = SYSREAD();

    if mess:
        isWarning = OKFILE(obj);            # if file was created, it was just a warning
        ERRPRINT(mess, err=1 if isWarning else 2, rec=3);

        if not isWarning       : return 3;  # stop compile; error flag
        elif   FATAL_WARNINGS(): return 2;  # stop compile; warning flag

    else:                                   # files that produce warnings will not be touched
        FKTOUCH(src); FKTOUCH(obj);         # this is so the warn nags you till you fix it

    if mess: return 1;                      # 'success' with warnings
    return 0;                               # actual success

#   ---     ---     ---     ---     ---

def AVTO_CHKFILE(f, gcc, brute=0):

    fname = f[0]; fdata = f[1];

    src = SRCFOLD + "\\" + fname + "." + fdata.ext;
    obj = OBJFOLD + "\\" + fname + ".o";

    if not OKFILE(obj) or brute:
        return (obj, AVTO_SRCBUILD(src, obj, gcc));

    elif MTIMEVS(src, obj):
        return (obj, AVTO_SRCBUILD(src, obj, gcc));

    return (obj, -1);

#   ---     ---     ---     ---     ---

def AVTO_CHKDEPS(m, name, libs=None):

    if not libs: return 0;

    ref   = f"{OBJFOLD}\\{name}"
    if not OKFILE(ref): return 1;

#   ---     ---     ---     ---     ---

    for lib in libs:
        x = MTIMELT([lib, ref]);
        if x == ref:
            return 1;

    return 0;

#   ---     ---     ---     ---     ---

def AVTO_MKEXE(olist, gcc, name):

    exe = f"{OBJFOLD}\\{name}.exe"
    if OKFILE(exe): DOS(f"@del {exe}");

    olist = " ".join(fname for fname in olist); _w = " -Wall " if FKWALL() else " "
    DOS(f"@{gcc}{_w}{olist} -o {exe} {LIBS} 2> {LOGOS()}");

    mess = SYSREAD();
    if mess:
        isWarning = OKFILE(exe);
        ERRPRINT(mess, err=1 if isWarning else 2, rec=3);

        if not isWarning or FATAL_WARNINGS():
            if OKFILE(exe): DOS(f"@del {exe}");
            ERRPRINT("BAD EXE; terminated.", err=3, rec=3);
            return (exe, 1);

    return (exe, 0);

def AVTO_MKLIB(olist, ar, name):

    lib = f"{OBJFOLD}\\{name}.lib";
    if OKFILE(lib): DOS(f"@del {lib}");

    olist = " ".join(fname for fname in olist);
    DOS(f"@{ar} crf {lib} {olist} 2> {LOGOS()}");

    mess = SYSREAD();
    if mess:
        isWarning = OKFILE(lib);
        ERRPRINT(mess, err=1 if isWarning else 2, rec=3);

        if not isWarning or FATAL_WARNINGS():
            if OKFILE(lib): DOS(f"@del {lib}");
            ERRPRINT("BAD LIB; terminated.", err=3, rec=3);
            return (lib, 1);

    return (lib, 0);

def AVTO_MKDLL(olist, gcc, name):

    dll = f"{OBJFOLD}\\{name}.dll"
    if OKFILE(dll): DOS(f"@del {dll}");

    olist = " ".join(fname for fname in olist); _w = " -Wall " if FKWALL() else " "
    DOS(f"@{gcc}{_w}-shared {olist} -o {dll} 2> {LOGOS()}");

    mess = SYSREAD();
    if mess:
        isWarning = OKFILE(dll);
        ERRPRINT(mess, err=1 if isWarning else 2, rec=3);

        if not isWarning or FATAL_WARNINGS():
            if OKFILE(dll): DOS(f"@del {dll}");
            ERRPRINT("BAD DLL; terminated.", err=3, rec=3);
            return (dll, 1);

    return (dll, 0);

#   ---     ---     ---     ---     ---
