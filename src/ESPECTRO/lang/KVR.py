#   ---     ---     ---     ---     --- #
#   KVR.py                              #
#                                       #
#   language syntax definitions         #
#   interpreter for user input          #
#                                       #
#   char conversions for pesolang       #
#                                       #
#   ---     ---     ---     ---     --- #

from  ESPECTRO import getch;
from .DTMANG   import LDBLOCK, GTBLOCK;

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

kBACKSPACE  =  8;
kRETURN     = 13;
kESCAPE     = 27;

kARROWLEFT  = 75;
kARROWRIGHT = 77;

kDELETE     = 83;
kSTART      = 71
kEND        = 79;

def PEIN():

    global OCTAVE;

    i   = 0; char = ""; s = "";
    ptr = 0; print("\x1b[22m\x1b[24m\x1b[48;2;0;32;128m\x1b[38;2;0;192;0m$:"\
                  +(" "*64) + ("\x1b[D"*64),

                   end='', flush=1);

    while char != "\r":

        char = getch().upper();
        x = ord(char);

#   ---     ---     ---     ---     ---

        if x == 224:

            y = ord(getch());

            if y == kARROWLEFT and ptr > 0:
                ptr -= 1; print("\x1b[D", end='', flush=1);

            elif y == kARROWRIGHT and ptr < len(s):
                ptr += 1; print("\x1b[C", end='', flush=1);

            elif y == kSTART:
                print("\r\x1b[2C", end='', flush=1); ptr = 0;

            elif y == kEND:
                n = len(s) - ptr;
                if n: print(f"\x1b[{n}C", end='', flush=1);

                ptr = len(s);

            elif y == kDELETE and ptr != len(s):
                s = s[:ptr] + s[ptr+1:];
                print(" \b", end='', flush=1);

                for pos in range(ptr, len(s), 1):
                    print(s[pos], end='', flush=1);

                print(" \b", end='', flush=1);

                for pos in range(ptr, len(s), 1):
                    print("\b", end='', flush=1);

            else:
                pass; #print(x, end='', flush=1);

#   ---     ---     ---     ---     ---

        else:

            i = GTPEKEY(char);

            if x == 27 or x == 13: break;

            elif x == kBACKSPACE:

                if ptr > 0:

                    if (ptr) == len(s):
                        ptr -= 1; s = s[:-1];
                        print("\b \b", end='', flush=1);

                    else:
                        ptr -= 1; s = s[:ptr] + s[ptr+1:];
                        for pos in range(ptr, len(s) - 1, 1):
                            print(s[pos+1], end='', flush=1);

                        print(" \b", end='', flush=1);

                        for pos in range(ptr, len(s)-1, 1):
                            print("\b", end='', flush=1);

                        print(f"\b{s[ptr]}\b", end='', flush=1);

#   ---     ---     ---     ---     ---

            else:
                if i != -1:

                    SPSHIFT = i>12; SPCTRL = not i%13;

                    if SPCTRL:
                        CHOCT(SPSHIFT);
                        char = f"↑{OCTAVE}" if SPSHIFT else f"↓{OCTAVE}";

                    else:
                        i    = i - 1 if SPSHIFT else i;
                        char = INMORPH(i + (24 * OCTAVE));

#   ---     ---     ---     ---     ---

                else:
                    char = char.decode('utf-8');

                if (len(s) < 64) and (ptr == len(s)):
                    ptr += 1; s = s + char;
                    print(char, end='', flush=1);

                elif ptr != len(s):
                    s = s[:ptr] + char + s[ptr+1:]; ptr += 1;
                    print(char, end='', flush=1);

    print(f"\x1b[0m\n");
    return s;

#   ---     ---     ---     ---     ---

CMD =                                       {

#    "NEW" : MKBLOCK,                        # ☺
#    "DEL" : DLBLOCK,                        # ☻
#    "SAVE": SVBLOCK,                        # ♥
    "LOAD": LDBLOCK,                        # ♦
    "GET" : GTBLOCK

                                            };

def INTPRT():

    TKN = [t for t in PEIN().split(" ") if t];

    if TKN:
        VERB = TKN[0];
        if VERB == "EXIT": return VERB;

        if len(TKN) >= 2:
            SUBJ = TKN[1];

            if len(TKN) > 2:
                ARGS = TKN[2:];
                return CMD[VERB](SUBJ, *ARGS);

            return CMD[VERB](SUBJ);

        return CMD[VERB]();

#   ---     ---     ---     ---     ---