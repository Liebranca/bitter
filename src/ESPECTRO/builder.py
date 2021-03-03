#   ---     ---     ---     ---     ---   #
#   builder.py                            #
#                                         #
#   project build avtomat                 #
#                                         #
#   ---     ---     ---     ---     ---   #

from ESPECTRO import (

    DOS,

    TOUCH,
    MTIMEVS,

    OKFILE,
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

def AVTO_SETOUT(s):
    global OBJFOLD; OBJFOLD = s;

def AVTO_SETIN (s):
    global SRCFOLD; SRCFOLD = s;

def AVTO_SETINC(s):
    global INCLUDES; INCLUDES = " ".join("-I%s"%fold for fold in s);

def AVTO_SETLIB(s):
    global LIBPATH; LIBPATH  = " ".join("-L%s"%fold for fold in s);

#   ---     ---     ---     ---     ---

def AVTO_CLEAN(ext):
    DOS("@del %s\\*.%s"%(OBJFOLD,ext));

def AVTO_SRCBUILD(src, obj, gcc):

    if OKFILE(obj): DOS("@del %s"%obj);     # delete pre-existing .o
    print(src.split("\\")[-1]);

    s = "@%s -MMD %s %s -c %s -o %s 2> %s"%(gcc, LIBPATH, INCLUDES, src, obj, LOGOS());
    DOS(s); err = SYSREAD();

    if err:

        print("\nERRORS/WARNINGS:\n\n%s"%err);

        isWarning = OKFILE(obj);            # if file was created, it was just a warning
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

def AVTO_MKEXE(olist, gcc, name):

    exe = f"{OBJFOLD}\\{name}.exe"
    if OKFILE(exe): DOS(f"@del {exe}");

    olist = " ".join(fname for fname in olist);
    DOS(f"@{gcc} {olist} -o {exe} 2> {LOGOS()}");

    err = SYSREAD();
    if err:
        print("\nERRORS/WARNINGS:\n\n%s"%err);

        isWarning = OKFILE(exe);
        if not isWarning or FATAL_WARNINGS():
            DOS(f"@del {exe}"); print("BAD EXE; terminated.");

def AVTO_MKLIB(olist, ar, name):

    lib = f"{OBJFOLD}\\{name}.lib";
    if OKFILE(lib): DOS(f"@del {lib}");

    olist = " ".join(fname for fname in olist);
    DOS(f"@{ar} crf {lib} {olist} 2> {LOGOS()}");

    err = SYSREAD();
    if err:
        print("\nERRORS/WARNINGS:\n\n%s"%err);

        isWarning = OKFILE(lib);
        if not isWarning or FATAL_WARNINGS():
            DOS(f"@del {lib}"); print("BAD LIB; terminated.");

def AVTO_MKDLL(olist, gcc, name):

    dll = f"{OBJFOLD}\\{name}.dll"
    if OKFILE(dll): DOS(f"@del {dll}");

    olist = " ".join(fname for fname in olist);
    DOS(f"@{gcc} -shared {olist} -o {dll} 2> {LOGOS()}");

    err = SYSREAD();
    if err:
        print("\nERRORS/WARNINGS:\n\n%s"%err);

        isWarning = OKFILE(dll);
        if not isWarning or FATAL_WARNINGS():
            DOS(f"@del {dll}"); print("BAD DLL; terminated.");

#   ---     ---     ---     ---     ---
