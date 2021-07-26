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

    OKPATH,
    OKFILE,
    RDFILE,
    SHPATH,

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
HEDS     = [];

# if this is set (and OKPATH) dlls are sent to it
# hardcoding it for now for testing
BUILD_DLL_TO="D:\\lieb_git\\UPBGE_ASSETS\\lytools"

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

    #---------------------------
    # added this here so i can look at the disassembly
    # make a flag to disable or maybe put it somewhere else
    # i'd prefer having it always on, but yknow: things
    asm=obj.replace(".o", ".s");
    s = f"@{gcc}{_w} {INCLUDES} -S -fverbose-asm -O2 {src} -o {asm} 2> {LOGOS()}";
    DOS(s); mess=mess+SYSREAD();
    #---------------------------

    if mess:
        isWarning = OKFILE(obj);            # if file was created, it was just a warning
        ERRPRINT(mess, err=1 if isWarning else 2, rec=3);

        if not isWarning       : return 3;  # stop compile; error flag
        elif   FATAL_WARNINGS(): return 2;  # stop compile; warning flag

    if mess: return 1;                      # 'success' with warnings
    return 0;                               # actual success

#   ---     ---     ---     ---     ---

def TCHHEDS():
    global HEDS;
    for hed in HEDS:
        FKTOUCH(hed);

    del HEDS; HEDS=[];

def CHKHEDS(src, f):

    f=RDFILE(f, rl=1, rl_sep=" ", ask=0, mute=1)[0];
    deps=f.replace("\n", "").replace("/", "\\").replace("\\\\", "\\").split(" ");

    if len(deps) > 1:
        deps=deps[1:];
        deps=[path for path in deps if OKFILE(path)];
        global HEDS; HEDS.extend([dep for dep in deps if dep not in HEDS]);

        if src not in HEDS:
            HEDS.append(src);

        for dep in deps:

            if MTIMEVS(dep, src):
                return 1;

    return 0;

def AVTO_CHKFILE(f, gcc, brute=0):

    fname = f[0]; fdata = f[1];

    src = SRCFOLD + "\\" + fname + "." + fdata.ext;
    obj = OBJFOLD + "\\" + fname + ".o";
    hed = OBJFOLD + "\\" + fname + ".d";

    for f in [src, obj, hed]:
        if f not in HEDS: HEDS.append(f);

    if not OKFILE(obj) or brute:
        return (obj, AVTO_SRCBUILD(src, obj, gcc));

    elif MTIMEVS(src, obj):
        return (obj, AVTO_SRCBUILD(src, obj, gcc));

    elif OKFILE(hed):
        if CHKHEDS(src, hed):
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

    if BUILD_DLL_TO and OKPATH(BUILD_DLL_TO):
        OBJFOLD=BUILD_DLL_TO;

    name=name.lower(); dll = f"{OBJFOLD}\\{name}.dll"
    if OKFILE(dll): DOS(f"@del {dll}");

    olist = " ".join(fname for fname in olist);
    _w = " -Wall " if FKWALL() else " ";

    DOS(f"@{gcc}{_w} -shared {olist} -o {dll} {LIBS} 2> {LOGOS()}");

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
