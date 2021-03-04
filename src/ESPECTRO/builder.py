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
    MTIMEVS,

    OKFILE,
    LISTDIR,
    EXTSOLVE,
    CATPATH,

    SYSREAD,
    LOGOS,

    FATAL_WARNINGS
);

#   ---     ---     ---     ---     ---

OBJFOLD  = "";
SRCFOLD  = "";
INCLUDES = "";
LIBPATH  = "";
LIBBIN   = "";

def AVTO_SETOUT(s):
    global OBJFOLD; OBJFOLD = s;

def AVTO_SETIN (s):
    global SRCFOLD; SRCFOLD = s;

def AVTO_SETINC(s):
    global INCLUDES; INCLUDES = " ".join("-I%s"%fold for fold in s);

def AVTO_LIBDIR(s):
    global LIBPATH; LIBPATH  = f"-L{s}";

def AVTO_LIBBIN(s):
    global LIBBIN;  LIBBIN   = " ".join("-l%s"%lib  for lib  in s);

#   ---     ---     ---     ---     ---

def AVTO_CLEAN(ext):
    DOS(f"@del {OBJFOLD}\\*.{ext}");

def AVTO_SRCBUILD(src, obj, gcc):

    if OKFILE(obj): DOS(f"@del {obj}");     # delete pre-existing .o
    print(src.split("\\")[-1]);

    s = f"@{gcc} -Wall -MMD {INCLUDES} -c {src} -o {obj} 2> {LOGOS()}";
    DOS(s); mess = SYSREAD();

    if mess:

        isWarning = OKFILE(obj);            # if file was created, it was just a warning

        mess = f"\n{mess}";
        ERRPRINT(mess, err=1 if isWarning else 3, rec=2);

        if not isWarning       : return 3;  # stop compile; error flag
        elif   FATAL_WARNINGS(): return 2;  # stop compile; warning flag

    else:                                   # files that produce warnings will not be touched
        TOUCH(src); TOUCH(obj);             # this is so the warn nags you till you fix it

    if err: return 1;                       # 'success' with warnings
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

def AVTO_CHKDEPS(m):

    libs  = m.libs if TARGET() == 'x32' else m.libs64;
    files = LISTDIR(LIBPATH, [".lib", ".dll"]);

    for lib in libs:

        full = "";
        for f in files:

            if EXTSOLVE(lib, f):
                full = f;
                files.pop(f);

                break;

        if not full:
            ERRPRINT(f"{m.name} missing dependency {lib} in <{LIBPATH}>", err=3, rec=2);
            return 1;

        f = f"{LIBDIR}\\{full}"

#   ---     ---     ---     ---     ---

def AVTO_MKEXE(olist, gcc, name):

    exe = f"{OBJFOLD}\\{name}.exe"
    if OKFILE(exe): DOS(f"@del {exe}");

    olist = " ".join(fname for fname in olist);
    DOS(f"@{gcc} -Wall {olist} -o {exe} {LIBPATH} {LIBBIN} 2> {LOGOS()}");

    mess = SYSREAD();
    if mess:

        isWarning = OKFILE(exe);

        mess = f"\n{mess}";
        ERRPRINT(mess, err=1 if isWarning else 3, rec=2);

        if not isWarning or FATAL_WARNINGS():
            DOS(f"@del {exe}"); ERRPRINT("BAD EXE; terminated.", err=3, rec=2);
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

        mess = f"\n{mess}";
        ERRPRINT(mess, err=1 if isWarning else 3, rec=2);

        if not isWarning or FATAL_WARNINGS():
            DOS(f"@del {lib}"); ERRPRINT("BAD LIB; terminated.", err=3, rec=2);
            return (lib, 1);

    return (lib, 0);

def AVTO_MKDLL(olist, gcc, name):

    dll = f"{OBJFOLD}\\{name}.dll"
    if OKFILE(dll): DOS(f"@del {dll}");

    olist = " ".join(fname for fname in olist);
    DOS(f"@{gcc} -Wall -shared {olist} -o {dll} 2> {LOGOS()}");

    mess = SYSREAD();
    if mess:

        isWarning = OKFILE(dll);

        mess = f"\n{mess}";
        ERRPRINT(mess, err=1 if isWarning else 3, rec=2);

        if not isWarning or FATAL_WARNINGS():
            DOS(f"@del {dll}"); ERRPRINT("BAD DLL; terminated.", err=3, rec=2);
            return (dll, 1);

    return (dll, 0);

#   ---     ---     ---     ---     ---
