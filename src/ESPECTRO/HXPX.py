#   ---     ---     ---     ---     ---   #
#   HXPX.py                               #
#                                         #
#   for creating and loading *.px files   #
#   make and manage large prjxs with      #
#   multiple modules, build automation    #
#                                         #
#   ---     ---     ---     ---     ---   #

from dataclasses import dataclass
from ESPECTRO    import (

    ROOT,
    CC,
    TARGET,

    CPRINT,
    INPUTFIELD,
    TOGGLEFIELD,
    PSEUDOPOP,
    SHUFFLER,

    COORD,
    REGION,
    LAYOUT,
    GETKVNSL,
    GETKVRSOR,

    DOS,
    ERRPRINT,

    clnstr,
    clnpth,

    ALL_LANGS,
    COMP_LANG,
    MKSRC,
    MKHED,

    CATPATH,
    SHPATH,
    OKPATH,
    OKFILE,

    FILESIZE,
    READSIZE,

    WALK,
    LISTDIR,

    MKDIR,
    MKFILE,
    RDFILE,
    DELF,
    DELD,

    CHOICE,
    MULTICHOICE,

    LKEYS,
    lyDICT,

    BTIME,

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

build_opt = ["EXE", "LIB", "DLL"];
PX        = None;

#   ---     ---     ---     ---     ---

class hxLIB:

    def __init__(self, name, used=0, ext=1):

        self.name = name;
        self.used = used;
        self.ext  = ext;

        if ext==1: self.include = "-I"+CATPATH(ROOT(), "_include", name);
        else     : self.include = "-I"+CATPATH(ROOT(), ext, "src", name);

        self.nited = 1;

    def __setattr__(self, attr, value):
        if hasattr(self, "nited"):
            if attr == "used":
                if value:
                    if self.name not in PX.libs:
                        PX.libs.append(self.name);
                else:
                    if self.name in PX.libs:
                        PX.libs.remove(self.name);

        object.__setattr__(self, attr, value);

    @property
    def path(self):
        if self.ext==1: return CATPATH(ROOT(), "_lib", TARGET(), self.name);
        else          : return CATPATH(ROOT(), ext, "trashcan", TARGET(), name);

    @property
    def buildstr(self):
        p=self.path; l=LISTDIR(p, exts=["lib"]);
        return "-L"+self.path+" "+(" ".join(["-l"+(lib.split(".")[0]) for lib in l]));

#   ---     ---     ---     ---     ---

class hxSRC:

    def __init__(self, ext="c", hed=0):

        self.ext  = ext;
        self.hed  = hed;

    def __repr__(self): return f"{self.ext.upper()} FILE";

    @staticmethod
    def new():

        self = hxSRC();

        print("Creating new source file\n");

        x         = input                   ("-Name:     ");
        name      = clnstr(x);

        i         = MULTICHOICE             ("Extension", ALL_LANGS);
        if not i:
            del self; return None;

        self.ext  = ALL_LANGS[i-1];

        global PX;

        mod = PX.curmod;

        i   = MULTICHOICE("Choose a submodule", mod.reprdirs());
        if not i:
            del self; return None;

        key = LKEYS(mod.subdirs, i-1 if i else 0);

        PX.cursub = key;                    # set cwd to folder we add to
        mod.subdirs[key][name] = self;

#   ---     ---     ---     ---     ---

        self.hed = 0;

        if self.ext in COMP_LANG:
            r     = CHOICE("Would you like to create a header file?");
            if r == 1:

                self.hed = 1;
                f        = PX.at + "\\" + name + ".h";

                MKFILE(f, "".join(MKHED(name, self.ext)));

#   ---     ---     ---     ---     ---

        f = PX.at + "\\" + name + "." + self.ext;
        if not OKFILE(f):

            lines = MKSRC(name, self.ext);
            if self.hed: lines[6] = '#include "%s.h"\n\n'%(name);

            MKFILE(f, "".join(lines));

        return self;

#   ---     ---     ---     ---     ---

    @staticmethod
    def from_list(flist):

        result     = {};
        names_exts = [f.split(".") for f in flist];

        for desc in names_exts:
            name, ext = desc;

            if ext not in ALL_LANGS:
                continue;

            if ext != "h": result[name] = hxSRC(ext, name + ".h" in flist);

        return result;

#   ---     ---     ---     ---     ---

class hxMOD:

    def __init__(self, name = 'MOD', subdirs=None, mode = 1):

        self.name  = name;
        self.mode  = mode;

        if subdirs:
            for sd in subdirs:
                files       = subdirs[sd];
                subdirs[sd] = { fname:hxSRC(**fdata) for fname, fdata in files.items() };

            self.subdirs    = lyDICT({ sd:files for sd, files in subdirs.items() });
        else: self.subdirs  = lyDICT({                                           });

        self.makecur();

    def __repr__(self): return f"{self.name}" if self.name != '__main__' else "MAIN";

#   ---     ---     ---     ---     ---

    @property
    def path(self):
        global PX;
        return CATPATH(ROOT(), PX.name, "src", self.name).replace("__main__", "");

#   ---     ---     ---     ---     ---

    @staticmethod
    def new():

        global PX;
        self         = hxMOD();

        print                               ("Creating new module for prjx %s\n"%PX.name);

        x            = input                ("-Name: "                                 );
        self.name    = clnstr               (x                                         );

        i            = MULTICHOICE("Build %s as"%self.name, build_opt);
        if not i:
            del self; return None;

        self.mode    = i - 1 if i else 0;

        mdpath       = self.path;



        PX.curmod = self;
        return self;

#   ---     ---     ---     ---     ---

    def scandir(self):

        scn    = WALK(self.path);
        scn    = [ tup for tup in scn if "__pycache__" not in tup[0] ];

        for cd, sd, files in scn[1:]:       # populate dict;
                                            # self.subdirs[name] = [files]

            cd               = cd.split("\\")[-1];

            _files           = hxSRC.from_list(files);
            self.subdirs[cd] = _files;

        if scn[0][2]:
            self.subdirs["__main__"] = hxSRC.from_list(scn[0][2]);

        self.makecur();

    #same as scandir, for a single folder
    def scanfold(self, fdpath):

        files                = hxSRC.from_list(WALK(fdpath)[0][2]);

        fdpath               = fdpath.split("\\")[-1];
        self.subdirs[fdpath] = files;
        self.makecur();

#   ---     ---     ---     ---     ---

    def addfold(self, newsub):

        global PX; PX.curmod = self; PX.cursub = newsub;
        fdpath = PX.at;

        self.subdirs[newsub] = {};
        if not OKPATH(fdpath):
            MKDIR(fdpath);

        else:
            r = CHOICE("Found submodule directory. Scan?");
            if r == 1: self.scanfold(fdpath);

        self.makecur();

    def makecur(self):
        global PX; PX.curmod = self;
        try   : PX.cursub = self.subdirs[-1];
        except: PX.cursub = "";

    def popfold(self, sub):

        global PX; PX.curmod = self;
        
        if sub in self.subdirs:
            del self.subdirs[sub];
            PX.cursub=sub; fdpath=PX.at;
            DELD(fdpath);

            self.makecur();

    def reprdirs(self):
        return [ sd for sd in self.subdirs ];

    def dirSelect(self):

        global PX;

        i = MULTICHOICE("Select submodule", [sd for sd in self.subdirs.keys()]);
        if not i: return None;

        i = i - 1 if i else 0;
        d = LKEYS(self.subdirs, i);

        PX.cursub = d; return d;

    def fileSelect(self):

        global PX; flist = list(self.subdirs[PX.cursub].keys());

        i = MULTICHOICE("Select file", [f for f in flist]);
        if not i: return None;

        f = flist[(i - 1 if i else 0)]

        return f;

#   ---     ---     ---     ---     ---

    def addfile(self):
        self.makecur();
        return hxSRC.new();

    def listfiles(self, sub):
        return ( [fname, fdata] for fname, fdata in self.subdirs[sub].items() );

#   ---     ---     ---     ---     ---

    def popfile(self, fname):

        global PX; PX.curmod = self;

        fdpath = ""; fh = None; sd = "";    # fullpath; filehandle; subdir
        sm     = fname.split("\\");

        if len(sm) == 2:                    # if fname == SUBMODULE\\FILENAME
            sd = sm[0]; fname = sm[1];      # then sd = split(\\) @0 | fname = split(\\) @1

            if sd in self.subdirs:          # check sd exists and look for fname

                if fname in self.subdirs[sd]:
                    fh = self.subdirs[sd].pop(fname);

#   ---     ---     ---     ---     ---

        else:                               # default to looking through all subdirs

            for d in self.subdirs:
                if fname in self.subdirs[d]:
                    sd = d;
                    fh = self.subdirs[sd].pop(fname);
                    break;

#   ---     ---     ---     ---     ---

        if fh:
            r = CHOICE("File removed from project. Delete?");
            if r == 1:

                PX.cursub = sd; fdpath = PX.at;
                DELF(fdpath + "\\" + fname + "." + fh.ext);
                if fh.hed:
                    DELF(fdpath + "\\" + fname + ".h");

            del fh;

        else:
            ERRPRINT(f"Cannot find file <{SHPATH(fname)}>", err=2, rec=1);

#   ---     ---     ---     ---     ---

class hxPX:

    def __init__(self, name="", libs=None, modules=None, mode=0):

        self.name         = name;
        self.mode         = mode;

        self.curmod       = "";
        self.cursub       = "";

        global PX; PX     = self;

        if libs: self.libs=libs;
        else   : self.libs=[];

        if modules : self.modules = [ hxMOD(**m) for m in modules ];
        else       : self.modules = [                             ];

#   ---     ---     ---     ---     ---

    def save(self):

        px_dict =                           {

            "name"    : self.name,
            "mode"    : self.mode,
            "libs"    : self.libs,

            "modules" : [],

                                            };

#   ---     ---     ---     ---     ---

        for m in self.modules:

            d =                             {

                "name"   : m.name,
                "mode"   : m.mode,
                "subdirs": {},

                                            };

            for sd in m.subdirs:

                sm = d["subdirs"][sd] = {};
                for fname, fdata in m.subdirs[sd].items():

                    sm[fname] =             {

                            "ext": fdata.ext,
                            "hed": fdata.hed

                                            };

            px_dict["modules"].append(d);

        MKFILE(self.path + ("\\%s.px"%self.name), str(px_dict).replace(" ", ""), b=1);

#   ---     ---     ---     ---     ---

    @staticmethod
    def load(name):

        path = CATPATH(ROOT(), name, "%s.px"%name);

        if not OKFILE(path):
            ERRPRINT(f"Project <{SHPATH(path)}> not found", err=2, rec=1);
            return None;

        d = eval(RDFILE(path, b=1, dec=1, ask=0)[0]);
        return hxPX(**d);

#   ---     ---     ---     ---     ---

    @property
    def at(self):
        return CATPATH(self.curmod.path, self.cursub).replace("__main__", "");

    @property
    def path(self):
        return CATPATH(ROOT(), self.name);

    @property
    def includes(self):
        return ( [ CATPATH(self.path, "include") ] #
               + [ m.name for m in self.modules  ] #
               + self.add_includes                 );

    @property
    def libdir(self):
        return CATPATH(ROOT(), "_lib", TARGET());

    @property
    def intdir(self):
        return CATPATH(self.path, "trashcan", TARGET());

    @property
    def outdir(self):
        return CATPATH(self.path, "bin", TARGET());

#   ---     ---     ---     ---     ---

    def setincludes(self, includes, m=0):

        if not m:
            self.add_includes = includes;

        else    :
            self.add_includes.extend(includes);

    def addlibs(self, libs=[]):

        nthmod = 1;
        while nthmod == 1:
            m = self.modSelect();

            nthlib = 1;
            while nthlib == 1:
                l32 = input("lib: ");

                i = CHOICE("Is the 64-bits version named different?");
                if i == 1:
                    l64 = input("lib64: ");

                else: l64 = l32;

                if l32 not in m.lib  : m.lib.append(l32);
                if l64 not in m.lib64: m.lib64.append(l64);

                nthlib = CHOICE(f"\nAdd another lib to {m.name}?");

            nthmod = CHOICE("Add libs to another module?");

#   ---     ---     ---     ---     ---

    def mkpaths(self):

        p = self.path;

        if not OKPATH(p):
            MKDIR(p);

            if not OKPATH(p):
                ERRPRINT("Path creation failed: you're in deep shit", err=3, rec=1);
                return None;

        folds = [ f"{p}\\src",
                  f"{p}\\bin\\x64",           f"{p}\\bin\\Win32\\",
                  f"{p}\\trashcan\\x64",      f"{p}\\trashcan\\Win32" ];

        for f in folds:
            if not OKPATH(f): MKDIR(f);

#   ---     ---     ---     ---     ---

    @staticmethod
    def new():
        self      = hxPX                    (                        );
        print                               ("Creating new PX file\n");

        x         = input                   ("-Name: "               );
        self.name = clnpth                  (x                       );

#   ---     ---     ---     ---     ---

        self.mkpaths();

        i            = MULTICHOICE("Build %s as"%self.name, build_opt);
        if not i:
            del self; return None;

        self.mode    = i - 1 if i else 0;

        self.modules = self.scanmods();

        if self.modules: self.curmod = self.modules[0];

        global PX; PX = self; return self;  # make current and return

#   ---     ---     ---     ---     ---

    def scanmods(self):

        global PX; PX = self;

        srcpath = CATPATH(self.path, "src");
        modules = WALK(srcpath)[0][1];

        i = 0; result = [];
        for m in modules:

            new_module = hxMOD(m);
            new_module.scandir();
            result.append(new_module);

            self.build_order.append(i);

        files      = WALK(srcpath)[0][2];
        new_module = hxMOD("__main__", mode=self.mode);
        _files     = hxSRC.from_list(files);

        new_module.subdirs["__main__"] = _files;
        result.append(new_module);

        return result;

    def modSelect(self):

        i = MULTICHOICE("Select module", [m.name for m in self.modules]);
        if not i: return None;

        m = self.modules[i - 1 if i else 0];

        self.curmod = m; self.cursub = LKEYS(m.subdirs, 0);

        return m;

    def addModule(self):
        return self.modules.append(hxMOD.new());

    def popModule(self, m):
        self.modules.pop(self.modules.index(m));

        if len(self.modules):
            self.curmod = self.modules[-1]; self.curmod.makecur();
        else:
            self.curmod=None; self.cursub=None;

#   ---     ---     ---     ---     ---

    def addFile(self):
        m = self.modSelect();
        m.addfile();

    def popFile(self):
        m = self.modSelect();
        d = m.dirSelect();

        m.popfile(f"{d}\\{m.fileSelect()}");

#   ---     ---     ---     ---     ---

    def build(self, brute=0):

        global PX; PX = self;
        KVNSL=GETKVNSL(); KVNSL.DEBUG_TOGGLE();
        KVNSL.DEBUG_OUT(["Compilation started"], {"err":0});

        BTIME('rnow');                      # sets time value for all timestamps to right now
                                            # ensures there's no variance in mod/access time
                                            # for all files built by this call

        outbase = self.intdir;
        release = self.outdir;
        libdir  = self.libdir;

        cc      = CC();

        gcc     = cc + "gcc.exe";
        ar      = cc + "ar.exe"

        abort   = 0; i=0; intfiles=[];

        for lib in LIBS:
            AVTO_INCLUDES(lib.include,  i!=0);
            AVTO_LIBS    (lib.buildstr, i!=0); i=1;

        for m in self.modules:

            self.curmod = m;
            outdir = outbase + "\\" + m.name
            if not OKPATH(outdir): MKDIR(outdir);

            AVTO_SETOUT(outdir); mfiles = []; fcount = 0;

#   ---     ---     ---     ---     ---

            for sd in m.subdirs:

                self.cursub = sd;
                AVTO_SETIN(self.at);
                _files = [ f for f in m.listfiles(sd)     # this is ugly
                           if f[1].ext in COMP_LANG    ]; # if fdata.ext reqs compile

                for f in _files:            # compile into .o if necessary
                    ofile, cf = AVTO_CHKFILE(f, gcc, brute);
                    if cf > 0:
                        KVNSL.DEBUG_SPIT();
                    if cf in [2, 3]:
                        if   cf == 2:
                            ERRPRINT("Compile warnings (FATAL_WARNINGS is ON)");
                        elif cf == 3:
                            ERRPRINT("Compile errors");

                        abort = True; break;

                    mfiles.append(ofile); fcount += cf;

                if abort: break;

#   ---     ---     ---     ---     ---

            if abort: break;

            if m.name == '__main__' or m.mode == 0:
                n = self.name; outdir=release;
                if m.mode==0: DEPS = AVTO_CHKDEPS(m, f"{n}.exe", intfiles);

            else:
                n = m.name; DEPS = 0;
                if m.name == '__main__': outdir=release;
                else: outdir=outbase + "\\" + m.name;

            AVTO_SETOUT(outdir);
            if len(mfiles) or DEPS:

                if fcount != -len(mfiles) or brute or DEPS:

                    if    m.mode == 0: inc, failure = AVTO_MKEXE(mfiles, gcc, n);
                    elif  m.mode == 1: inc, failure = AVTO_MKLIB(mfiles, ar,  n);
                    else             : inc, failure = AVTO_MKDLL(mfiles, gcc, n);

                    if failure:
                        abort = 1;
                        break;

                else:
                    ERRPRINT(f"{m.name} is up to date");

            if m.mode == 1 and OKFILE(CATPATH(outdir, m.name+".lib")):
                AVTO_INCLUDES(f"-I{m.path}",            1);
                AVTO_LIBS    (f"-L{outdir} -l{m.name}", 1);

                intfiles.append(CATPATH(outdir, m.name+".lib"));

#   ---     ---     ---     ---     ---

        KVNSL.DEBUG_SPIT();
        #KVNSL.DEBUG_TOGGLE();

        if self.mode == 0 and not abort:    # ask run after compile

            # send this block to an intermediate call
            i = CHOICE("\nBuild ready. Run?")
            if i == 1: self.run();

    def run(self):
        if self.mode == 0:
            DOS(f"SET PATH={CATPATH(ROOT(), '_run', TARGET())};%PATH%");
            DOS(f"{self.outdir}\\{self.name}.exe");

        else:
            ERRPRINT(f"Cannot run {self.name}: it is a library", rec=1);

#   ---     ---     ---     ---     ---

    def clean(self):

        global PX; PX = self;

        outbase = self.intdir;
        libbase = self.libdir;
        relbase = self.outdir

        for m in self.modules:

            self.curmod = m;

            outdir = f"{outbase}\\{m.name}"
            if not OKPATH(outdir): continue;

            AVTO_SETOUT(outdir);
            AVTO_CLEAN("o"); AVTO_CLEAN("d");

            if m.mode in (1, 2):
                AVTO_SETOUT(outbase);
                AVTO_CLEAN(".lib"); AVTO_CLEAN(".dll");

            else:
                if m.name == '__main__': outdir = relbase;
                else: outdir = f"{relbase}\\{bin}"

                AVTO_SETOUT(outdir);
                AVTO_CLEAN(".exe");

#   ---     ---     ---     ---     ---

    def __repr__(self):
        return f"PX {self.name}";

    def fileTree(self):

        s = "+%s\n|\n"%("-" * 48);

        exts = { k:0 for k in ALL_LANGS };
        tot  = 0;

        for m in self.modules:

            mname = m.name.replace("__main__", "MAIN");
            s = s + "|__< %s >%s\n|\n"%(mname, "_" * ( 42 - len(mname) ));

            for sd in m.subdirs:

                sdn = ("\\" + sd).replace("__main__", ".")

                s = s + "|---- %s\n"%sdn;

                for fname, fdata in m.subdirs[sd].items():

                    s = s + "|-------- $%s"%fname;

                    s = s + ( " " * ( 32 - len(fname)) );

                    if fdata.ext in COMP_LANG and fdata.hed:
                        s = s + " (%s, H)\n"%fdata.ext.upper();

                    else:
                        s = s + " (%s)\n"%fdata.ext.upper();

                    exts[fdata.ext] += 1; tot += 1;

                if not m.subdirs[sd]:
                    s = s + "|-------- (empty)\n";

                s = s + "|\n";
            s = s + "+%s\n|\n"%("-" * 48);

        exts = { k:n for k, n in exts.items() if n };
        pers = "";

        j = 0;
        for e, i in exts.items():
            x = (i/tot) * 100;
            pers = pers + "%.2f%% %s"%(x, e.upper());

            if j < len(exts) - 1:
                pers = pers + " | "

            j += 1;

        s = ("\n>__%s__ ( %s )\n"%(self.name, pers)) + s;

        print(s);

#   ---     ---     ---     ---     ---

LIBS=None; new_sub=None; new_mod=None; new_src=None;

def SCANLIBS():

    # default project hardcoded while we work on it
    global PX; PX = hxPX.load("KVR");
    global LIBS; LIBS=[]; err=0;

    libfiles = LISTDIR( CATPATH(ROOT(), "_lib", "Win32") );
    for lib in libfiles:
        LIBS.append(hxLIB(lib, lib in PX.libs));

    return err;

def CONFIM_CLEAR(DO, CALL0, ARGS0, CALL1, ARGS1):

    if DO:
        CALL0(*ARGS0);

    CALL1(*ARGS1);

def PXDRAWMENU(CALL):
    KVNSL = GETKVNSL(); ID = CALL(); KVNSL.FLREGION(ID);

def PXLISTMODS(func, pghit, tgt):

    global PX; KVNSL = GETKVNSL();
    if KVNSL.CUR_REGION.NAME != tgt: KVNSL.CHREGION(tgt);
    else: KVNSL.CLREGION();

    region = KVNSL.CUR_REGION; region.onPagHit = pghit;
    i=0; j=0; region.PAGES=int(len(PX.modules))/region.PAGESIZE;
    for m in PX.modules[region.getPageOffset():]:
        name = m.name if m.name != "__main__" else "MAIN";

        region.addItem(2+(j*region.ITEMSIZE+2), 1+i,
                       name, style=2, func=func, args={"m":m});
        i+=1;

        if i >= int(region.PAGESIZE/2):
            j+=1; i=0;
            if j > 1:
                break;

    if region.ITEMS: KVNSL.FLREGION();

def PXLISTSUBS(func, pghit, tgt):

    global PX; KVNSL = GETKVNSL();
    if KVNSL.CUR_REGION.NAME != tgt: KVNSL.CHREGION(tgt);
    else: KVNSL.CLREGION();

    region = KVNSL.CUR_REGION; region.onPagHit = pghit;
    i=0; j=0; region.PAGES=int(len(PX.curmod.subdirs))/region.PAGESIZE;

    for sd in list(PX.curmod.subdirs.keys())[region.getPageOffset():]:
        name = sd if sd != "__main__" else "MAIN";

        region.addItem(2+(j*region.ITEMSIZE+2), 1+i,
                       name, style=2, func=func, args={"sd":sd});
        i+=1;

        if i >= int(region.PAGESIZE/2):
            j+=1; i=0;
            if j > 1:
                break;

    if region.ITEMS: KVNSL.FLREGION();

def PXLISTLIBS(pghit, tgt):

    global PX, LIBS; KVNSL = GETKVNSL();
    if KVNSL.CUR_REGION.NAME != tgt: KVNSL.CHREGION(tgt);
    else: KVNSL.CLREGION();

    pad=len(LIBS[0].name);
    for lib in LIBS[1:]:
        length=len(LIBS[0].name);
        if pad < length:
            pad = length;

    pad+=5;

    region = KVNSL.CUR_REGION; region.onPagHit = pghit;
    i=0; j=0; region.PAGES=int(len(LIBS))/region.PAGESIZE; k=region.getPageOffset();
    for lib in LIBS[region.getPageOffset():]:
        name = lib.name;
        name = name+(" "*(pad-len(name)))+": "+("YES" if lib.used else "NO");
        region.addItem(2+(j*region.ITEMSIZE+2), 1+i,
                       name, style=2, func=SETFIELD_TOGGLE, args={'ob':lib, 'field':"used",
                       'idex':k, 'l':['NO', 'YES']});
        i+=1; k+=1;

        if i >= int(region.PAGESIZE/2):
            j+=1; i=0;
            if j > 1:
                break;

    if region.ITEMS: KVNSL.FLREGION();

#   ---     ---     ---     ---     ---

def PXUI():

    KVNSL = GETKVNSL();

    INFO       = REGION ( "INFO",                           #
                          rect=COORD( 1,  1, 76, 18, 18, 22 ),
                          borders=(0,1,1,1),
                          corners=(1,1,9,3),
                          labels=["", "BACK"],
                          align=(0,0,1));

    LPANEL     = REGION ( "LPANEL",                         #
                          rect=COORD( 1,  1, 16,  1,  1, 18 ),
                          borders=(1,1,3,5),
                          corners=(12,14,13,11),
                          labels =[PX.name, "INFO"],
                          align=(0,1,0));

    RPANEL     = REGION ( "RPANEL",                         #
                          rect=COORD(16, 16, 76,  1,  1, 18 ),
                          borders=(1,0,3,1),
                          align=(0,1,0),
                          labels=["", ""],
                          corners=(2,6,2,7));

    PX_LAYOUT = LAYOUT ([INFO, LPANEL, RPANEL]);

    DOS("@ECHO OFF && CLS"); KVNSL.CLS();
    KVNSL.NEW_SCREEN("USER", PX_LAYOUT);
    KVNSL.CHREGION("LPANEL"); PXMAINMENU(); KVNSL.FILLSCREEN();
    DOS('TITLE %__SLAVE%%_PLATFORM% (ESPECTRO)'); itm, info = KVNSL.RNAVIGATE(0);
    KVNSL.OUT(itm, flush=0); KVNSL.OUT(info, region=INFO, flush=1);
    CPRINT("\x1b[0m");

def PXMAINMENU():
    global PX; KVNSL = GETKVNSL();
    region = KVNSL.CUR_REGION;

    KVNSL.CLREGION("LPANEL", PX.name);

    region.addItem(1, 2, "SAVE", style=0, info="", func=SAVEPX);

    region.addItem(1, 4, "MODULES", style=0, info="", func=PXMODMENU);
    region.addItem(1, 5, "FILES", style=0, info="", func=None);
    region.addItem(1, 6, "LIBS", style=0, info="", func=PXLIBSEL);

    region.addItem(1, 10, "BUILD", style=0,
                   info="Compiles the current project", func=PX.build, args={"brute":0});

    return "LPANEL";

def PXMODMENU():
    global PX; KVNSL = GETKVNSL();
    region = KVNSL.CUR_REGION; INFO = KVNSL.CUR_SCREEN.INFO;

    KVNSL.CLREGION("LPANEL", f"{PX.name}/MODULES");

    region.addItem(1, 2, "NEW", style=0, info="Create a new module", func=ADDMODPX);
    region.addItem(1, 4, "SELECT", style=0,
                   info=f"Select a module to edit (current: {PX.curmod})",
                   func=SELMODPX);

    region.addItem(1, 5, "REMOVE", style=0,
                   info=f"Remove a module from project",
                   func=POPMODPX);

    region.addItem(1, 6, "ORDER", style=0,
                   info=f"Set the build order for modules",
                   func=ORDMODPX);

    region.addItem(1, 8, "ADDSUB", style=0,
                   info=f"Create a subdirectory within current module ({PX.curmod})",
                   func=ADDSUBPX);

    region.addItem(1, 9, "POPSUB", style=0,
                   info=f"Remove a subdirectory from current module ({PX.curmod})",
                   func=POPSUBPX);

    region.addItem(1, 14, "BACK", style=0, info="Return to main menu",
                   func=PXDRAWMENU, args={'CALL':PXMAINMENU});

    s = "";
    for elem in region.ITEMS:
        s = s + region.drawMid(elem['y']);

    itm, info = KVNSL.RNAVIGATE(0);
    KVNSL.OUT(s+itm, flush=0); KVNSL.OUT(info, region=INFO, flush=1);
    region.onEscHit = (PXDRAWMENU, [PXMAINMENU]);

def GLOBALINFO(s):
    KVNSL = GETKVNSL(); KVNSL.OUT(KVNSL.CUR_SCREEN.INFO.updateBuff(s),
                                  region=KVNSL.CUR_SCREEN.INFO)

    for elem in KVNSL.CUR_REGION.ITEMS:
        elem['info']=s;

#   ---     ---     ---     ---     ---

def PXLIBSEL():
    KVNSL = GETKVNSL(); KVNSL.CUR_SCREEN.REGIONS["RPANEL"].LABELS[0]="LIBRARIES";
    PXLISTLIBS(PXLIBSEL, "RPANEL");

#   ---     ---     ---     ---     ---

def SAVEPX():
    global PX; PX.save();

def SELMODPX_INT(m):
    KVNSL = GETKVNSL();
    m.makecur(); KVNSL.CUR_REGION.PAGE=0;

    region = KVNSL.CUR_SCREEN.REGIONS['LPANEL']; region.onPagHit=None;

    elem   = region.ITEMS[1];
    elem['info'] = f"Select a module to edit (current: {PX.curmod})";

    elem   = region.ITEMS[4];
    elem['info'] = f"Create a subdirectory within current module ({PX.curmod})";

    elem   = region.ITEMS[5];
    elem['info'] = f"Remove a subdirectory from current module ({PX.curmod})";

    KVNSL.CUR_SCREEN.REGIONS["RPANEL"].LABELS[0]="";
    KVNSL.SWAPWIPE('RPANEL', 'LPANEL');

def SELMODPX():
    KVNSL = GETKVNSL(); KVNSL.CUR_SCREEN.REGIONS["RPANEL"].LABELS[0]="MODULE SELECT";
    PXLISTMODS(SELMODPX_INT, SELMODPX, "RPANEL");

#   ---     ---     ---     ---     ---

def ORDMODPX_INT(m):
    global PX; KVNSL = GETKVNSL();

    KVNSL.OUT(KVNSL.CUR_SCREEN.INFO.updateBuff(
    f"Use UP and DOWN arrow keys to relocate {str(m)}"), region=KVNSL.CUR_SCREEN.INFO)

    region = KVNSL.CUR_REGION;
    idex   = PX.modules.index(m);

    elems  = [elem for elem in region.ITEMS];

    PEIN   = SHUFFLER(PX.modules, elems, idex);

    global CINPUT_REG; CINPUT_REG={'PEIN':PEIN,
        'INFO':"Press ENTER to relocate a module, ESCAPE to go back"};

    KVNSL.AP_INTCALL("SHUFFIELD_INT", SHUFFIELD_INT, (), 1);

def ORDMODPX():
    KVNSL = GETKVNSL(); KVNSL.CUR_SCREEN.REGIONS["RPANEL"].LABELS[0]="BUILD ORDER";
    PXLISTMODS(ORDMODPX_INT, ORDMODPX, "RPANEL");
    GLOBALINFO("Press ENTER to relocate a module, ESCAPE to go back");

#   ---     ---     ---     ---     ---

def POPMODPX():
    KVNSL = GETKVNSL(); KVNSL.CUR_SCREEN.REGIONS["RPANEL"].LABELS[0]="REMOVE MODULE";
    PXLISTMODS(POPMODPX_INT, POPMODPX, "RPANEL");

def POPMODPX_END():
    global PX; KVNSL = GETKVNSL();

    region = KVNSL.CUR_SCREEN.REGIONS['LPANEL'];

    elem   = region.ITEMS[1];
    elem['info'] = f"Select a module to edit (current: {PX.curmod})";

    elem   = region.ITEMS[4];
    elem['info'] = f"Create a subdirectory within current module ({PX.curmod})";

    elem   = region.ITEMS[5];
    elem['info'] = f"Remove a subdirectory from current module ({PX.curmod})";

    KVNSL.CUR_SCREEN.REGIONS["RPANEL"].LABELS[0]="";
    KVNSL.SWAPWIPE('RPANEL', 'LPANEL');

def POPMODPX_INT(m):
    global PX; KVNSL = GETKVNSL();
    region = KVNSL.CUR_REGION; region.onPagHit=None; region.PAGE=0;

    if m.name != "__main__":
        KVNSL.CLREGION(); region.addItem(1, 1, f"Remove module?  ", style=0);
        KVNSL.FLREGION(); SETFIELD_TOGGLE(PSEUDOPOP(CONFIM_CLEAR,
                                          (PX.popModule, [m],
                                           POPMODPX_END, [ ] )),

                                           "state", 0, ["YES", "NO"]);

        KVNSL.OUT(KVNSL.CUR_SCREEN.INFO.updateBuff(f"Removing module {m.name}"));
        CINPUT_REG['PEIN'].spit = 0;

    else:
        KVNSL.CLREGION(); region = KVNSL.CUR_REGION; region.onPagHit=None;
        region.addItem(1, 1, f"MAIN cannot be deleted. Got it?  ", style=0);
        KVNSL.FLREGION();

        SETFIELD_TOGGLE(PSEUDOPOP(CONFIM_CLEAR,
                        (None, [],
                         POPMODPX_END, [ ] ), valid=[1]),

                        "state", 0, ["OK"]);

        CINPUT_REG['PEIN'].spit = 0;

#   ---     ---     ---     ---     ---

def ADDMODPX():
    global PX, new_mod; KVNSL = GETKVNSL();
    KVNSL = GETKVNSL(); KVNSL.CUR_SCREEN.REGIONS["RPANEL"].LABELS[0]="NEW MODULE";
    KVNSL.CHREGION("RPANEL");

    if not new_mod: new_mod=hxMOD();

    region = KVNSL.CUR_REGION;
    region.addItem(2, 1, f"Name: {new_mod.name}", style=2, info="Name for this module",
                   func=SETFIELD_CINPUT, args={'ob':new_mod, 'field':"name", 'idex':0});

    region.addItem(2, 2, f"Mode: {build_opt[new_mod.mode]}", style=2,
                   info="Filetype this module compiles to",
                   func=SETFIELD_TOGGLE, args={'ob':new_mod, 'field':"mode", 'idex':1,
                                                'l':build_opt});

    region.addItem(2, 4, "Accept", style=1, info="Create module with these settings",
                   func=ADDMODPX_SEND, args={'accept':1});
    region.addItem(2, 5, "Cancel", style=1, info="Discard and return",
                   func=ADDMODPX_SEND, args={'accept':0});

    KVNSL.FLREGION();

def ADDMODPX_SEND(accept=1):
    global PX, new_mod; KVNSL = GETKVNSL();
    if accept:

        repeat = [m for m in PX.modules if new_mod.name == m.name];
        if not repeat:
            PX.modules.append(new_mod); PX.curmod = PX.modules[-1];
        else:
            PX.curmod = repeat[0];

        if not OKPATH(PX.curmod.path):
            MKDIR(PX.curmod.path);

        else:
            KVNSL.CLREGION(); region = KVNSL.CUR_REGION;
            region.addItem(1, 1, "Module directory found. Scan?  ", style=0,);
            KVNSL.FLREGION();

            KVNSL.CUR_SCREEN.REGIONS["RPANEL"].LABELS[0]="";
            SETFIELD_TOGGLE(PSEUDOPOP(CONFIM_CLEAR,
                            (PX.curmod.scandir, (),
                             KVNSL.SWAPWIPE, ("RPANEL", "LPANEL"))),

                            "state", 0, ["YES", "NO"]);

            CINPUT_REG['PEIN'].spit = 0; new_mod = None;
            return;

    new_mod = None;
    KVNSL.CUR_SCREEN.REGIONS["RPANEL"].LABELS[0]="";
    KVNSL.SWAPWIPE("RPANEL", "LPANEL");

#   ---     ---     ---     ---     ---

@dataclass
class hxSUB:                                # has no real reason to exist;
    name: str;

def ADDSUBPX():
    global PX, new_sub; KVNSL = GETKVNSL();
    KVNSL = GETKVNSL(); KVNSL.CUR_SCREEN.REGIONS["RPANEL"].LABELS[0]="NEW MODULE";
    KVNSL.CHREGION("RPANEL");

    if not new_sub: new_sub=hxSUB("SUB");

    region = KVNSL.CUR_REGION;
    region.addItem(2, 1, f"Name: {new_sub.name}", style=2, info="Name for this submodule",
                   func=SETFIELD_CINPUT, args={'ob':new_sub, 'field':"name", 'idex':0});

    region.addItem(2, 3, "Accept", style=1, info="Create submodule",
                   func=ADDSUBPX_SEND, args={'accept':1});
    region.addItem(2, 4, "Cancel", style=1, info="Discard and return",
                   func=ADDSUBPX_SEND, args={'accept':0});

    KVNSL.FLREGION();

def ADDSUBPX_SEND(accept=1):
    global PX, new_sub; KVNSL = GETKVNSL();
    if accept:

        repeat = [sd for sd in PX.curmod.subdirs if new_sub.name == sd];
        if not repeat:
            PX.curmod.subdirs[new_sub.name]={};

        PX.cursub=new_sub.name; fdpath=PX.at;

        if not OKPATH(fdpath):
            MKDIR(fdpath);

        else:
            KVNSL.CLREGION(); region = KVNSL.CUR_REGION;
            region.addItem(1, 1, "Submodule directory found. Scan?  ", style=0,);
            KVNSL.FLREGION();

            KVNSL.CUR_SCREEN.REGIONS["RPANEL"].LABELS[0]="";
            SETFIELD_TOGGLE(PSEUDOPOP(CONFIM_CLEAR,
                            (PX.curmod.scanfold, [fdpath],
                             KVNSL.SWAPWIPE, ("RPANEL", "LPANEL"))),

                            "state", 0, ["YES", "NO"]);

            CINPUT_REG['PEIN'].spit = 0; new_sub = None;
            return;

    new_sub = None;
    KVNSL.CUR_SCREEN.REGIONS["RPANEL"].LABELS[0]="";
    KVNSL.SWAPWIPE("RPANEL", "LPANEL");

#   ---     ---     ---     ---     ---


def POPSUBPX():
    KVNSL = GETKVNSL(); KVNSL.CUR_SCREEN.REGIONS["RPANEL"].LABELS[0]="REMOVE SUBMODULE";
    PXLISTSUBS(POPSUBPX_INT, POPSUBPX, "RPANEL");

def POPSUBPX_END():
    global PX; KVNSL = GETKVNSL();

    region = KVNSL.CUR_SCREEN.REGIONS['LPANEL'];

    """ replace this with cur submodule mambo later
    elem   = region.ITEMS[1];
    elem['info'] = f"Select a module to edit (current: {PX.curmod})";
    """

    KVNSL.CUR_SCREEN.REGIONS["RPANEL"].LABELS[0]="";
    KVNSL.SWAPWIPE('RPANEL', 'LPANEL');

def POPSUBPX_INT(sd):
    global PX; KVNSL = GETKVNSL();
    region = KVNSL.CUR_REGION; region.onPagHit=None; region.PAGE=0;

    if sd != "__main__":
        KVNSL.CLREGION(); region.addItem(1, 1, f"Remove submodule?  ", style=0);
        KVNSL.FLREGION(); SETFIELD_TOGGLE(PSEUDOPOP(CONFIM_CLEAR,
                                          (PX.curmod.popfold, [sd],
                                           POPMODPX_END, [ ] )),

                                           "state", 0, ["YES", "NO"]);

        KVNSL.OUT(KVNSL.CUR_SCREEN.INFO.updateBuff(f"Removing submodule {sd}"));
        CINPUT_REG['PEIN'].spit = 0;

    else:
        KVNSL.CLREGION(); region = KVNSL.CUR_REGION; region.onPagHit=None;
        region.addItem(1, 1, f"MAIN cannot be deleted. Got it?  ", style=0);
        KVNSL.FLREGION();

        SETFIELD_TOGGLE(PSEUDOPOP(CONFIM_CLEAR,
                        (None, [],
                         POPSUBPX_END, [ ] ), valid=[1]),

                        "state", 0, ["OK"]);

        CINPUT_REG['PEIN'].spit = 0;

#   ---     ---     ---     ---     ---

CINPUT_REG=None;
def SETFIELD_CINPUT(ob, field, idex):
    KVNSL = GETKVNSL(); CURSOR = GETKVRSOR();
    region = KVNSL.CUR_REGION; elem = region.ITEMS[idex];
    x, y = elem['x'], elem['y'];

    field_name, value = elem['label'].split(": ");
    in_x = x + (len(field_name) + 2); CURSOR.JUMP(in_x, y);

    PEIN = INPUTFIELD.MAKE(x=in_x, y=y, length=region.pad-in_x, buff=value, prompt='');
    global CINPUT_REG; CINPUT_REG={'PEIN':PEIN, 'ob':ob, 'field':field,
                                   'field_name':field_name, 'idex':idex};

    KVNSL.AP_INTCALL("SETFIELD_CINPUT_INT", SETFIELD_CINPUT_INT, (), 1);

def SETFIELD_CINPUT_INT():
    global CINPUT_REG; PEIN, ob, field, field_name, idex = CINPUT_REG.values();
    KVNSL = GETKVNSL(); CURSOR = GETKVRSOR();
    x = PEIN.RUN(); elem = KVNSL.CUR_REGION.ITEMS[idex];

    if PEIN.state:
        x = x + f"$:KVNSL_H.DL_INTCALL('SETFIELD_CINPUT_INT', 1);>";

        if PEIN.state == -1:
            pass;
        else:
            pass;

        if not PEIN.buff:
            PEIN.buff = getattr(ob, field);
            x = x + PEIN.buff;

        setattr(ob, field, PEIN.buff); elem['label'] = field_name+": "+PEIN.buff;

        del PEIN; CINPUT_REG=None;

    return x;

#   ---     ---     ---     ---     ---

def SETFIELD_TOGGLE(ob, field, idex, l):
    KVNSL = GETKVNSL(); CURSOR = GETKVRSOR();
    region = KVNSL.CUR_REGION; elem = region.ITEMS[idex];
    x, y = elem['x'], elem['y'];

    schar = ": " if ": " in elem['label'] else "? ";
    field_name, value = elem['label'].split(schar);
    in_x = x + (len(field_name) + 2); CURSOR.JUMP(in_x, y);

    PEIN = TOGGLEFIELD(l, ptr=getattr(ob, field), x=in_x, y=y);
    global CINPUT_REG; CINPUT_REG={'PEIN':PEIN, 'ob':ob, 'field':field,
                                   'field_name':field_name, 'idex':idex, 'schar':schar};

    KVNSL.AP_INTCALL("SETFIELD_TOGGLE_INT", SETFIELD_TOGGLE_INT, (), 1);

def SETFIELD_TOGGLE_INT():
    global CINPUT_REG; PEIN, ob, field, field_name, idex, schar = CINPUT_REG.values();
    KVNSL = GETKVNSL(); CURSOR = GETKVRSOR();

    x = PEIN.RUN(); elem = KVNSL.CUR_REGION.ITEMS[idex];

    if PEIN.state:
        x = x + f"$:KVNSL_H.DL_INTCALL('SETFIELD_TOGGLE_INT', 1);>";
        setattr(ob, field, PEIN.ptr); elem['label'] = field_name+schar+PEIN.CURRENT;

        del PEIN; CINPUT_REG=None;

    return x;

#   ---     ---     ---     ---     ---

def SHUFFIELD_INT():
    global CINPUT_REG; PEIN, INFO = CINPUT_REG.values();
    KVNSL = GETKVNSL(); CURSOR = GETKVRSOR(); idex = PEIN.idex;

    x = PEIN.RUN(); elem = KVNSL.CUR_REGION.ITEMS[idex];

    if PEIN.state:
        x = x + f"$:KVNSL_H.DL_INTCALL('SHUFFIELD_INT', 1);>"; GLOBALINFO(INFO);
        del PEIN; CINPUT_REG=None;

    return x;