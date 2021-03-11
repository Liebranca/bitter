#   ---     ---     ---     ---     --- #
#   KVR.py                              #
#                                       #
#  -manager for user input              #
#  -char conversions for pesolang       #
#                                       #
#   ---     ---     ---     ---     --- #

from  ESPECTRO import PEIN;
from .DTMANG   import (

    DTYPES,
    DOPS,

    GETWED,
    WDTYPE,

    LDBLOCK,
    GTBLOCK,

);

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