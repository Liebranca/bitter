#   ---     ---     ---     ---     --- #
#   KVR.py                              #
#                                       #
#  -manager for user input              #
#  -char conversions for pesolang       #
#                                       #
#   ---     ---     ---     ---     --- #

from  ESPECTRO import getch, CHOICE;
from .DTMANG   import (

    DTYPES,
    DOPS,

    GETWED,
    WDTYPE,

    LDBLOCK,
    GTBLOCK,

);

PE_OPCHRS     = """
                Ñ

                &?≤≥÷±½¼q≈²ⁿ
                |!«»%~°()=√τ
                µαπ¢εƒδφ¥Ω♪♀
                £@#$ΣÆßøΘ♫♂x
                ☺♥♣•○¯⌂▲◄↑←↕
                ☻♦♠◘◙≡∞▼►↓→↔

                """.replace(" ", "").replace("\n", "");

PE_DRCHRS     = """
                ││║║█
                ├╞╟╠▌
                ┤╡╢╣▐
                ·░▒▓█
                ┌╒╓╔▄
                └╘╙╚▀
                ┐╕╖╗▄
                ┘╛╜╝▀
                ─═─═■
                ┬╤╥╦▄
                ┼╪╫╬█
                ┴╧╨╩▀
                """.replace(" ", "").replace("\n", "");

PE_OPCHRS  = { i:k for k, i in zip(PE_OPCHRS, range(len(PE_OPCHRS))) };
iPE_OPCHRS = { k:i for i, k in PE_OPCHRS.items()                     };

PE_DRCHRS  = { i:k for k, i in zip(PE_DRCHRS, range(len(PE_DRCHRS))) };
iPE_DRCHRS = { k:i for i, k in PE_DRCHRS.items()                     };

PE_KEYS = b"\xA7\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x27\xAD"\
        + b"\xA6\x21\x22\xFA\x24\x25\x26\x2F\x28\x29\x3D\x3F\xA8"

#   ---     ---     ---     ---     ---

INCNTX = 0;
OCTAVE = 0;
INCHRS =                                    [

    [PE_OPCHRS, iPE_OPCHRS],
    [PE_DRCHRS, iPE_DRCHRS],

                                            ];

def INMORPH(char):

    if   isinstance(char, int):
        try:
            x = INCHRS[INCNTX][0][char];
        except:
            x = INCHRS[INCNTX][0][0];

    else:
        try:
            x = INCHRS[INCNTX][1][char];
        except:
            x = 0;

    return x;

def GTPEKEY(char):
    try:
        return PE_KEYS.index(char);
    except:
        return -1;

def CHOCT(SHIFT):
    global OCTAVE;

    if SHIFT:
        OCTAVE = OCTAVE + 1 if OCTAVE < 2 else 0

    else:
        OCTAVE = OCTAVE - 1 if OCTAVE > 0 else 2

#   ---     ---     ---     ---     ---

CMD =                                       {

#    "NEW" : MKBLOCK,
#    "DEL" : DLBLOCK,
#    "SAVE": SVBLOCK,
    "LOAD": LDBLOCK,
    "GET" : GTBLOCK,
    "WED" : WDTYPE,
                                            };

def VALIDATE_TOKENS(TKN, wed):

    if len(TKN):

        verb  = None;
        subj  = None;
        args  = [];

        order = 0;

#   ---     ---     ---     ---     ---

        if TKN[0] == "EXIT":
            return "EXIT";
        elif TKN[0] == "CHOICE":
            return "CHOICE";

        elif wed:

            subj    = wed;
            data_op = TKN[0] in DOPS[wed];

#   ---     ---     ---     ---     ---

            if data_op:
                verb  = DOPS[wed][TKN[0]];
                order = 1;

            elif TKN[0] in CMD:
                verb = CMD[TKN[0]];

            else: return "FAIL";

            if len(TKN) > 1:
                try   : args = [GTBLOCK(wed, TKN[1])];
                except: args: args = TKN[1:]

            if order: return (order, subj, verb, args);
            return (order, verb, subj, args);

#   ---     ---     ---     ---     ---

        else:
            if TKN[0] not in CMD:
                if TKN[0] in DTYPES and len(TKN) >= 2:
                    order = 1;

                    subj = TKN[0];
                    data_op = TKN[1] in DOPS[wed];

                    if data_op:
                        verb = DOPS[subj][TKN[1]];

                    else: return "FAIL";

                    if len(TKN) > 2:
                        args = TKN[2:];

                    return (order, subj, verb, args);

                else: return "FAIL";

#   ---     ---     ---     ---     ---

            else:
                if len(TKN) >= 2:

                    verb = CMD[TKN[0]];
                    subj = TKN[1];

                    if len(TKN) > 2:
                        args = TKN[2:];

                    return (order, verb, subj, args);

    return "FAIL";

#   ---     ---     ---     ---     ---

def INTPRT():

    exps = [e for e in PEIN().split(";") if e];
    ret  = [];

    for e in exps:

        TKN = [t for t in e.split(" ") if t];

        tup = VALIDATE_TOKENS(TKN, GETWED());
        if tup == "EXIT":
            ret.append("EXIT"); break;
        elif tup == "CHOICE":
            CHOICE("TEST?"); continue;

        elif tup == "FAIL":
            print( "\x1b[22m\x1b[24m\x1b[48;2;128;0;0m\x1b[38;2;192;192;192m"\
                 +f"F:{e}" + (" "*(64-len(e)))                               );

            continue;

#   ---     ---     ---     ---     ---

        order = tup[0];

        if order:
            subj, verb, args = tup[1:];
            cls = DTYPES[subj];
            mth = getattr(cls, verb);

            if args:
                ret.append(mth(*args)); continue;

            ret.append(mth()); continue;

#   ---     ---     ---     ---     ---

        verb, subj, args = tup[1:];

        if args:
            ret.append(verb(subj, *args)); continue;

        ret.append(verb(subj));

    return ret;

#   ---     ---     ---     ---     ---