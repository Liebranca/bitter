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

    clnstr,
    clnpth,

    ALL_LANGS,
    COMP_LANG,
    MKSRC,
    MKHED,

    CATPATH,
    OKPATH,
    OKFILE,

    FILESIZE,
    READSIZE,

    WALK,

    MKDIR,
    DELF,
    DELD,

    CHOICE,
    MULTICHOICE,

    LKEYS,
    lyDICT,

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

build_opt = ["Executable", "Static library", "Dynamic library"];
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

        i         = MULTICHOICE             ("-Extension:", ALL_LANGS);
        self.ext  = ALL_LANGS[i-1];

        global PX;

        mod = PX.curmod;
        sub = PX.cursub;

        r = CHOICE("Current submodule is %s: %s. Choose another?"%(mod.name, sub));
        if r == 1:
            i = MULTICHOICE("Choose a subdir for %s:"%(self.name), mod.reprdirs());
            key = LKEYS(mod.subdirs, i-1);

        else: key = sub;

        PX.cursub = key;                    # set cwd to folder we add to
        mod.subdirs[key][name] = self;

#   ---     ---     ---     ---     ---

        self.hed = 0;

        if self.ext in COMP_LANG:
            r     = CHOICE("Would you like to create a header file?");
            if r == 1:

                self.hed = 1;
                f        = PX.at + "\\" + name + ".h";

                with open(f, "w+") as fh: fh.write("".join(MKHED(name, self.ext)));

#   ---     ---     ---     ---     ---

        f = PX.at + "\\" + name + "." + self.ext;
        if not OKFILE(f):

            lines = MKSRC(name, self.ext);
            if self.hed: lines[6] = '#include "%s.h"\n\n'%(name);
            with open(f, "w+") as fh: fh.write("".join(lines));

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

    def __init__(self, name = 'MOD', subdirs = {}, mode = 1):

        self.name = name;
        self.mode = mode;

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

        i            = MULTICHOICE("Build %s as..."%self.name, build_opt);
        self.mode    = i - 1;

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

        self.subdirs[newsub] = [];
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
                if fname in self.subdirs[sd]:
                    sd = d;
                    fh = self.subdirs[sd].pop(fname);
                    break;

#   ---     ---     ---     ---     ---

        if fh:
            r = CHOICE("File removed from project. Delete?");
            if r == 1:

                PX.cursub = PX; fdpath = PX.at;
                DELF(fdpath + "\\" + fname + "." + fh.ext);
                if fh.hed:
                    DELF(fdpath + "\\" + fname + ".h");

            del fh;

        else:
            print("Cannot find file <%s>"%fname);

#   ---     ---     ---     ---     ---

class hxPX:

    def __init__(self, name   ="", includes=[], lib =[], lib64=[],
                       modules=[], order   =[], mode=0           ):

        self.name         = name;
        self.mode         = mode;

        self.curmod       = "";
        self.cursub       = "";

        self.add_includes = includes;

        self.add_lib      = lib;
        self.add_lib64    = lib64;

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
            "lib"     : self.add_lib,
            "lib64"   : self.add_lib64,

            "order"   : self.build_order,

            "modules" : []

                                            };

#   ---     ---     ---     ---     ---

        for m in self.modules:

            d =                             {

                "name"   : m.name,
                "mode"   : m.mode,
                "subdirs": {}

                                            };

            for sd in m.subdirs:

                sm = d["subdirs"][sd] = {};
                for fname, fdata in m.subdirs[sd].items():

                    sm[fname] =             {

                            "ext": fdata.ext,
                            "hed": fdata.hed

                                            };

            px_dict["modules"].append(d);

        with open(self.path + ("\\%s.px"%self.name), 'wb') as pxfile:
            pxfile.write(bytes(str(px_dict), encoding='ascii'));

#   ---     ---     ---     ---     ---

    @staticmethod
    def load(name):

        path = CATPATH(ROOT(), name, "\\%s.px"%name);

        if not OKFILE(path):
            print("Project <%s> not found"%path)
            return None;

        d = b""; s = 1;
        with open(path, 'rb') as file:

            size = FILESIZE(path);
            while size:

                rb = min(size, READSIZE()); # bytes to read  clamped to bytes left

                s  = file.read(rb);         # read, cat, substract
                d  = d + s; size -= rb;

        d = eval(d.decode('ascii'));
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
    def libs(self):
        return ( [ CATPATH(self.path, "lib", TARGET()) ] #

               +   self.add_lib if   TARGET() == "x86"   #
                                else self.add_lib64      )

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

    def setlib(self, libs, m=0):

        if TARGET() == "x86":
            if not m:
                self.add_lib = libs;

            else    :
                self.add_lib.extend(libs);

        else:
            if not m:
                self.add_lib64 = libs;

            else    :
                self.add_lib64.extend(libs);

#   ---     ---     ---     ---     ---

    def mkpaths(self):

        if not OKPATH(self.path):
            MKDIR(self.path);

            if not OKPATH(self.path):
                print("Path creation failed: you're in deep shit"); return None;

        folds = [ self.path + "\\src", self.path + "\\include",

                  self.path + "\\lib",
                  self.path + "\\lib\\x64",      self.path + "\\lib\\x86",

                  self.path + "\\release",
                  self.path + "\\release\\x64",  self.path + "\\release\\x86",

                  self.path + "\\trashcan",
                  self.path + "\\trashcan\\x64", self.path + "\\trashcan\\x86" ];

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

        i            = MULTICHOICE("Build %s as..."%self.name, build_opt);
        self.mode    = i - 1;

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

    def addModule(self):
        return self.modules.append(hxMOD.new());

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
            self.build_order[x] = i-1; tmp[i-1] = 0; x += 1;

        self.build_order[-1] = self.modules.index( [ m for m in self.modules
                                                       if  m.name == '__main__' ][0] );

    def orderedModules(self):

        if len(self.build_order) != len(self.modules):
            self.setBuildOrder();

        return [self.modules[x] for x in self.build_order];

#   ---     ---     ---     ---     ---

    def build(self, brute=0):

        global PX; PX = self;

        outbase = self.intdir;
        cc      = CC();

        gcc     = cc + "gcc.exe";
        ar      = cc + "ar.exe"

        abort   = 0;

        AVTO_SETINC(self.includes);
        AVTO_SETLIB(self.libs    );

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

            if len(mfiles):

                if fcount != -len(mfiles) or force:

                    if m.name == '__main__':
                        n = self.name;
                        AVTO_SETOUT(f"{self.path}\\release\\{TARGET()}")

                    else:
                        n = m.name;
                        AVTO_SETOUT(m.path);

                    if   m.mode == 0: AVTO_MKEXE(mfiles, gcc, n);
                    elif m.mode == 1: AVTO_MKLIB(mfiles, ar,  n);
                    else            : AVTO_MKDLL(mfiles, gcc, n);

                else: print(f"{m.name} is up to date");

#   ---     ---     ---     ---     ---

    def clean(self):

        global PX; PX = self;
        outbase = self.intdir;

        for m in self.modules:

            self.curmod = m;
            outdir = outbase + "\\" + m.name
            if not OKPATH(outdir): continue;

            AVTO_SETOUT(outdir);
            AVTO_CLEAN("o"); AVTO_CLEAN("d");

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

        return s;

#   ---     ---     ---     ---     ---