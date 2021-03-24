#   ---     ---     ---     ---     ---   #
#   HXPX.py                               #
#                                         #
#   for creating and loading *.px files   #
#   make and manage large prjxs with      #
#   multiple modules, build automation    #
#                                         #
#   ---     ---     ---     ---     ---   #

from ESPECTRO import (

    ROOT,
    CC,
    TARGET,

    CPRINT,
    INPUTFIELD,
    TOGGLEFIELD,

    COORD,
    REGION,
    LAYOUT,
    GETKVNSL,
    GETKVRSOR,

    DOS,

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

build_opt = ["EXEE", "LIB", "DLL"];
PX        = None;

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

    def __init__(self, name = 'MOD', subdirs=None, lib=None, lib64=None, mode = 1):

        if not lib  : lib  =[];
        if not lib64: lib64=[];

        self.name  = name;
        self.mode  = mode;

        self.lib   = lib;
        self.lib64 = lib64;

        if subdirs:
            for sd in subdirs:
                files       = subdirs[sd];
                subdirs[sd] = { fname:hxSRC(**fdata) for fname, fdata in files.items() };

            self.subdirs    = lyDICT({ sd:files for sd, files in subdirs.items() });
        else: self.subdirs  = lyDICT({                                           });

        self.makecur();

    def __repr__(self): return f"MODULE {self.name}";

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

        if not OKPATH(mdpath):
            MKDIR(mdpath);

        else:
            r = CHOICE                      ("Found module directory. Scan?"           );
            if r == 1: self.scandir();

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

            PX.cursub = sub; fdpath = PX.at;
            r = CHOICE("Delete submodule %s?"%sub);
            if r == 1: DELD(fdpath);

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

    def __init__(self, name="", includes=[], modules=[], order=[], mode=0):

        self.name         = name;
        self.mode         = mode;

        self.curmod       = "";
        self.cursub       = "";

        self.add_includes = includes;
        self.build_order  = order;

        global PX; PX     = self;

        if modules: self.modules = [ hxMOD(**m) for m in modules ];
        else      : self.modules = [                             ];

#   ---     ---     ---     ---     ---

    def save(self):

        px_dict =                           {

            "name"    : self.name,
            "mode"    : self.mode,

            "includes": self.add_includes,

            "order"   : self.build_order,

            "modules" : []

                                            };

#   ---     ---     ---     ---     ---

        for m in self.modules:

            d =                             {

                "name"   : m.name,
                "mode"   : m.mode,
                "subdirs": {},

                "lib"    : m.lib,
                "lib64"  : m.lib64

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
        return CATPATH(self.path, "lib", TARGET());

    @property
    def intdir(self):
        return CATPATH(self.path, "trashcan", TARGET());

    @property
    def outdir(self):
        return CATPATH(self.path, "release", TARGET());

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

        folds = [ f"{p}\\src",                f"{p}\\include",

                  f"{p}\\lib\\x64",           f"{p}\\lib\\Win32",
                  f"{p}\\release\\x64\\bin",  f"{p}\\release\\Win32\\bin",
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

#   ---     ---     ---     ---     ---

    def addFile(self):
        m = self.modSelect();
        m.addfile();

    def popFile(self):
        m = self.modSelect();
        d = m.dirSelect();

        m.popfile(f"{d}\\{m.fileSelect()}");

#   ---     ---     ---     ---     ---

    def setBuildOrder(self):

        tmp = [ m for m in self.modules if m.name != '__main__' ]; tmp.append(0);

        if len(self.build_order) != len(self.modules):
            self.build_order = [ i for i in range(len(self.modules)) ];

        x = 0;
        for m in self.modules:

            if len([t for t in tmp if t]) == 1:

                                            # solid competitor for ugliest shit ever written
                                            # worst part is i understand why it works

                self.build_order[x] =       (

                                            self.modules.index(                              #
                                                [ m for m, t in zip(self.modules, tmp)       #
                                                    if  m == t and m.name != '__main__' ][0] #
                                            )                                                );

                continue;

            if m.name == '__main__': continue;

            i = MULTICHOICE(f"SLOT {x}", [m for m in tmp if m]);
            if not i: break;

            self.build_order[x] = i-1; tmp[i-1] = 0; x += 1;

        self.build_order[-1] = self.modules.index( [ m for m in self.modules
                                                       if  m.name == '__main__' ][0] );

    def orderedModules(self):

        if len(self.build_order) != len(self.modules):
            print(f"Set the build order for {self.name}")
            self.setBuildOrder();

        return [self.modules[x] for x in self.build_order];

#   ---     ---     ---     ---     ---

    def build(self, brute=0):

        global PX; PX = self;

        BTIME('rnow');                      # sets time value for all timestamps to right now
                                            # ensures there's no variance in mod/access time
                                            # for all files built by this call

        outbase = self.intdir;
        release = self.outdir;
        libdir  = self.libdir;

        cc      = CC();

        gcc     = cc + "gcc.exe";
        ar      = cc + "ar.exe"

        abort   = 0;

        AVTO_SETINC(self.includes);
        AVTO_LIBDIR(libdir       );

        for m in self.orderedModules():

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
                    if cf in [2, 3]:
                        if   cf == 2: print("ABORT: Compile warnings (FATAL_WARNINGS is ON)");
                        elif cf == 3: print("ABORT: Compile errors");

                        abort = True; break;

                    mfiles.append(ofile); fcount += cf;

                if abort: break;

#   ---     ---     ---     ---     ---

            if abort: break;

            if m.name == '__main__':
                n = self.name; AVTO_SETOUT(release);
                DEPS = AVTO_CHKDEPS(m, f"{n}.exe");

            elif m.mode == 0:
                n = m.name; AVTO_SETOUT(f"{release}\\bin");
                DEPS = AVTO_CHKDEPS(m, f"{n}.exe");

            else:
                n = m.name; DEPS = 0;
                AVTO_SETOUT(libdir);

            if len(mfiles) or DEPS:

                if fcount != -len(mfiles) or brute or DEPS:

                    if    m.mode == 0: inc, failure = AVTO_MKEXE(mfiles, gcc, n);
                    elif  m.mode == 1: inc, failure = AVTO_MKLIB(mfiles, ar,  n);
                    else             : inc, failure = AVTO_MKDLL(mfiles, gcc, n);

                    if failure:
                        abort = 1;
                        break;

                else:
                    print(f"{m.name} is up to date");

#   ---     ---     ---     ---     ---

        if self.mode == 0 and not abort:    # ask run after compile
            i = CHOICE("\nBuild ready. Run?")
            if i == 1: self.run();

    def run(self):
        if self.mode == 0:
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
                AVTO_SETOUT(libbase);
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

new_mod=None; new_src=None;

def PXUI():

    # default project hardcoded while we work on it
    global PX; PX = hxPX.load("KVR");

    KVNSL = GETKVNSL();

    INFO       = REGION ( "INFO",                           #
                          rect=COORD( 1,  1, 76, 18, 18, 22 ),
                          borders=(0,1,1,1),
                          corners=(1,1,9,3),
                          labels=["", "BACK"],
                          align=(0,0,1));

    ACTIONS    = REGION ( "ACTIONS",                        #
                          rect=COORD( 1,  1, 16,  1,  1, 18 ),
                          borders=(1,1,3,5),
                          corners=(12,14,13,11),
                          labels =["ACTIONS", "INFO"],
                          align=(0,1,0));

    OPTIONS    = REGION ( "OPTIONS",                        #
                          rect=COORD(16, 16, 76,  1,  1, 18 ),
                          borders=(1,0,3,1),
                          align=(0,1,0),
                          labels=["OPTIONS", ""],
                          corners=(2,6,2,7));

    ACTIONS.addItem(1, 2, "SAVE", style=0, info="", func=SAVEPX);

    ACTIONS.addItem(1, 4, "NEW MODULE", style=0, info="", func=ADDMODPX);
    ACTIONS.addItem(1, 5, "NEW FILE", style=0, info="", func=None);

    ACTIONS.addItem(1, 7, "LIBS", style=0, info="", func=None);
    ACTIONS.addItem(1, 8, "ORDER", style=0, info="", func=None);

    ACTIONS.addItem(1, 10, "BUILD", style=0, info="Compiles the current project", func=None);

    ACTIONS.ITEMS_PTR = 0;
    PX_LAYOUT = LAYOUT ([INFO, ACTIONS, OPTIONS]);

    DOS("@ECHO OFF && CLS");
    KVNSL.CLS();
    KVNSL.NEW_SCREEN("USER", PX_LAYOUT); KVNSL.CHREGION("ACTIONS"); KVNSL.FILLSCREEN();
    DOS('TITLE %__SLAVE%%_PLATFORM% (ESPECTRO)'); itm, info = KVNSL.RNAVIGATE(0);
    KVNSL.OUT(itm, flush=0); KVNSL.OUT(info, region=INFO, flush=1);
    CPRINT("\x1b[0m");

def SAVEPX():
    global PX; PX.save();

def ADDMODPX():
    global PX, new_mod; KVNSL = GETKVNSL();
    KVNSL.CHREGION("OPTIONS"); INFO = KVNSL.CUR_SCREEN.INFO;

    if not new_mod: new_mod=hxMOD();

    region = KVNSL.CUR_REGION;
    region.addItem(2, 1, f"Name: {new_mod.name}", style=2, info="Name for this module",
                   func=SETFIELD_CINPUT, args={'ob':new_mod, 'field':"name", 'idex':0});

    region.addItem(2, 2, f"Mode: {build_opt[new_mod.mode]}", style=2,
                   info="Filetype this module compiles to",
                   func=SETFIELD_TOGGLE, args={'ob':new_mod, 'field':"mode", 'idex':1,
                                                'l':build_opt});

    region.addItem(2, 4, "Accept", style=1, info="Create module with these settings",
                   func=KVNSL.SWAPWIPE, args={'src':"OPTIONS", 'tgt':"ACTIONS"});
    region.addItem(2, 5, "Cancel", style=1, info="Discard and return",
                   func=KVNSL.SWAPWIPE, args={'src':"OPTIONS", 'tgt':"ACTIONS"});

    for elem in region.ITEMS:
        KVNSL.OUT(region.drawMid(elem['y']));

    itm, info = KVNSL.RNAVIGATE(0);
    KVNSL.OUT(itm, flush=0); KVNSL.OUT(info, region=INFO, flush=1);

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


def SETFIELD_TOGGLE(ob, field, idex, l):
    KVNSL = GETKVNSL(); CURSOR = GETKVRSOR();
    region = KVNSL.CUR_REGION; elem = region.ITEMS[idex];
    x, y = elem['x'], elem['y'];

    field_name, value = elem['label'].split(": ");
    in_x = x + (len(field_name) + 2); CURSOR.JUMP(in_x, y);

    PEIN = TOGGLEFIELD(l, ptr=getattr(ob, field), x=in_x, y=y);
    global CINPUT_REG; CINPUT_REG={'PEIN':PEIN, 'ob':ob, 'field':field,
                                   'field_name':field_name, 'idex':idex};

    KVNSL.AP_INTCALL("SETFIELD_TOGGLE_INT", SETFIELD_TOGGLE_INT, (), 1);

def SETFIELD_TOGGLE_INT():
    global CINPUT_REG; PEIN, ob, field, field_name, idex = CINPUT_REG.values();
    KVNSL = GETKVNSL(); CURSOR = GETKVRSOR();

    x = PEIN.RUN(); elem = KVNSL.CUR_REGION.ITEMS[idex];

    if PEIN.state:
        x = x + f"$:KVNSL_H.DL_INTCALL('SETFIELD_TOGGLE_INT', 1);>";
        setattr(ob, field, PEIN.ptr); elem['label'] = field_name+": "+PEIN.CURRENT;

        del PEIN; CINPUT_REG=None;

    return x;