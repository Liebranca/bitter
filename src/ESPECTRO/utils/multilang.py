#   ---     ---     ---     ---     --- #
#   multilang.py                        #
#                                       #
#   helper for common copypastes        #
#   and file creation in various langs  #
#                                       #
#   ---     ---     ---     ---     --- #

from ESPECTRO import LFLAT;

#   ---     ---     ---     ---     ---

EXT_PESOLIKE = [ "kvr", "pe" ];

def HED_INFO_PESOLIKE(name):

    lines  =                [

        "GUARD %s IFN REG\n"%name.upper(),
        "GUARD CPP EXTRC END\n\n",

        "//   ---     ---     ---     ---     ---\n\n",

        "GUARD CPP BKSCOPE END\n",
        "END ::%s\n"%name.upper()

                            ];

    return lines;

def SRC_INFO_PESOLIKE(name):

    mdblk = name.upper() + (" " * (38 - len(name)));
    lines =                     [

        "/*   ---     ---     ---     ---     ---    *\n",
        " *  " + mdblk +                          "  *\n",
        " *                                          *\n",
        " *  info box for this source file           *\n",
        " *                                          *\n",
        " *   ---     ---     ---     ---     ---    */\n\n",
        "// includes here\n\n",
        "//   ---     ---     ---     ---     ---\n\n",
                                ];

    return lines;

#   ---     ---     ---     ---     ---

EXT_CLIKE = [ "c", "cpp" ];

def HED_INFO_CLIKE(name):

    guard  = "__" + name.upper() + "_H" + "__";
    lines  =                [

        "#ifndef %s\n"%guard,
        "#define %s\n\n"%guard,

        "#ifdef __cplusplus\n",
        'extern "C" {\n',
        "#endif\n\n",

        "//   ---     ---     ---     ---     ---\n\n",

        "#ifdef __cplusplus\n",
        "}\n",
        "#endif\n\n",
        
        "#endif //%s\n"%guard

                            ];

    return lines;

def SRC_INFO_CLIKE(name):

    mdblk = name.upper() + (" " * (38 - len(name)));
    lines =                     [

        "/*   ---     ---     ---     ---     ---    *\n",
        " *  " + mdblk +                          "  *\n",
        " *                                          *\n",
        " *  info box for this source file           *\n",
        " *                                          *\n",
        " *   ---     ---     ---     ---     ---    */\n\n",
        "// includes here\n\n",
        "//   ---     ---     ---     ---     ---\n\n",
                                ];

    return lines;

#   ---     ---     ---     ---     ---

EXT_PYLIKE = [ "py" ];

def SRC_INFO_PYLIKE(name):

    mdblk = name.upper() + (" " * (38 - len(name)));
    lines =                     [

        "#    ---     ---     ---     ---     ---    #\n",
        "#   " + mdblk +                          "  #\n",
        "#                                           #\n",
        "#   info box for this source file           #\n",
        "#                                           #\n",
        "#    ---     ---     ---     ---     ---    #\n\n",
        "# imports here\n\n",
        "#    ---     ---     ---     ---     ---\n\n",
                                ];

    return lines;

#   ---     ---     ---     ---     ---

def MKSRC(name, ext):
    if   ext in EXT_CLIKE   : return SRC_INFO_CLIKE   (name + "." + ext);
    elif ext in EXT_PESOLIKE: return SRC_INFO_PESOLIKE(name + "." + ext);
    elif ext in EXT_PYLIKE  : return SRC_INFO_PYLIKE  (name + "." + ext);

def MKHED(name, ext):
    if   ext in EXT_CLIKE   : return HED_INFO_CLIKE   (name);
    elif ext in EXT_PESOLIKE: return HED_INFO_PESOLIKE(name);

COMP_LANG = LFLAT                           ([EXT_CLIKE                          ]);
ALL_LANGS = LFLAT                           ([EXT_CLIKE, EXT_PESOLIKE, EXT_PYLIKE]);

#   ---     ---     ---     ---     ---