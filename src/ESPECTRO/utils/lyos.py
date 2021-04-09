#   ---     ---     ---     ---     --- #
#   lyos.py                             #
#                                       #
#   shorthands and aliases for os       #
#   and sys calls; some path ops        #
#                                       #
#   console app pimping out             #
#                                       #
#   ---     ---     ---     ---     --- #

from   ESPECTRO.PYZJC import *;
from   msvcrt         import getch, kbhit;
from   pathlib        import Path;
from   time           import time as rnow, sleep;

import os, sys, atexit, time;

sys.ps1    = '>';
sys.ps2    = '-';

FATAL      = -255;
ERROR      = -254;

def DISCOUNT_ESCAPES(S, space):

    old_length = len(S); DIFF=0; i=0;
    while (DIFF < space) and (i < old_length):

        scp = 0;
        if S[i:i+2] == "$:":
            i+=2; scp = 1;
            while(S[i:i+2] != ";>"):
                i+=1;
                if i > old_length: break;

            i+=2;

        if not scp:
            DIFF+=1; i+=1;

    return (DIFF, i);

#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

WCHROMA =                                   {

     0: ";2;0;0;0m",                        # black
     1: ";2;128;0;0m",                      # red
     2: ";2;0;128;0m",                      # green
     3: ";2;128;128;0m",                    # yellow
     4: ";2;0;0;128m",                      # blue
     5: ";2;128;0;128m",                    # magenta
     6: ";2;0;128;128m",                    # cyan
     7: ";2;192;192;192m",                  # white

    10: ";2;64;64;64m",                     # bright black
    11: ";2;255;0;0m",                      # bright red
    12: ";2;0;255;0m",                      # bright green
    13: ";2;255;255;0m",                    # bright yellow
    14: ";2;0;0;255m",                      # bright blue
    15: ";2;255;0;255m",                    # bright magenta
    16: ";2;0;255;255m",                    # bright cyan
    17: ";2;255;255;255m",                  # bright white

                                            };

def MSCPX(s):
    result=""; coms=s.split("$:");
    for com in coms:
        if com:
            com=com[:-2];nt=eval(com);
            if nt: result=result+nt;

    return result;

def SCPX(s):
    return f"\x1b{s}";

def ANSIC(f=1, b=1):

    if f > 17:
        f = "$:SCPX('39m');>";
    else:
        f = f"$:SCPX('[38{WCHROMA[f]}');>";

    if b > 17:
        b = "$:SCPX('[49m');>";

    else:
        b = f"$:SCPX('[48{WCHROMA[b]}');>";

    return f + b;

#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

BOXCHARS =                                  {

    "VV": " │ │ ║ ║█",
    "VR": " ├ ╞ ╟ ╠▌",
    "VL": " ┤ ╡ ╢ ╣▐",
    "VS": " · ░ ▒ ▓█",
    "TL": " ┌ ╒ ╓ ╔▄",
    "BL": " └ ╘ ╙ ╚▀",
    "TR": " ┐ ╕ ╖ ╗▄",
    "BR": " ┘ ╛ ╜ ╝▀",
    "HH": " ─ ═ ─ ═■",
    "HD": " ┬ ╤ ╥ ╦▄",
    "CC": " ┼ ╪ ╫ ╬█",
    "HU": " ┴ ╧ ╨ ╩▀",

    "LABEL_S": "< ",
    "LABEL_E": " >",

    "PTR_S"  : "•",
    "PTR_E"  : "•"

                                            };

BORDER_DRAW    = 0x01;
BORDER_THICK_H = 0x02;
BORDER_THICK_V = 0x04;

def PKBORDER(d, h, v):
    return  ( (d * BORDER_DRAW   )
            | (h * BORDER_THICK_H)
            | (v * BORDER_THICK_V) );

CORNERDICT =                                {

    1: "VV",                                # up                    0b0001
    2: "HH",                                # left                  0b0010
    3: "BR",                                # up|left               0b0011

    4: "VV",                                # down                  0b0100
    5: "VV",                                # up|down               0b0101
    6: "TR",                                # left|down             0b0110
    7: "VL",                                # up|left|down          0b0111

    8: "HH",                                # right                 0b1000
    9: "BL",                                # up|right              0b1001
   10: "HH",                                # left|right            0b1010
   11: "HU",                                # up|left|right         0b1011
   12: "TL",                                # down|right            0b1100
   13: "VR",                                # up|down|right         0b1101
   14: "HD",                                # left|down|right       0b1110
   15: "CC"                                 # up|left|down|right    0b1111
                                            };

BOX_CORNER_W = 0x01;
BOX_CORNER_A = 0x02;
BOX_CORNER_S = 0x04;
BOX_CORNER_D = 0x08;

def PKCORNER( w, a, s, d):
    return  ( (w * BOX_CORNER_W)
            | (a * BOX_CORNER_A)
            | (s * BOX_CORNER_S)
            | (d * BOX_CORNER_D) );

#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

class KEYBOARD_CONTROLLER:

    LOG  = 27;
    SPEC = 0;
    TPWT = 0;

    KEYS =                                  {

        "BACKSPACE" :  8,
        "TAB"       :  9,
        "RETURN"    : 13,
        "EXIT"      : 26,
        "ESCAPE"    : 27,

        "ARROWLEFT" : 75,
        "ARROWRIGHT": 77,
        "ARROWUP"   : 72,
        "ARROWDOWN" : 80,

        "SPACE"     : 32,

        "REPAG"     : 73,
        "AVPAG"     : 81,
        "DELETE"    : 83,
        "START"     : 71,
        "END"       : 79,
        "SEMICOLON" : 59,

        "ORDM"      : 167,

        "SPEC"      : 224

                                            };

    VAL = list(KEYS.values());
    ID  = list(KEYS.keys  ());

#   ---     ---     ---     ---     ---

    @staticmethod
    def RUN(cases={}, spec_cases={}, typewriter=0):

        KEYS = KEYBOARD_CONTROLLER.KEYS;
        VAL  = KEYBOARD_CONTROLLER.VAL;
        ID   = KEYBOARD_CONTROLLER.ID;

        char = getch().upper(); x = ord(char);

        KEYBOARD_CONTROLLER.LOG  = x;
        KEYBOARD_CONTROLLER.SPEC = 0;
        KEYBOARD_CONTROLLER.TPWT = 0;

        if x == KEYS["SPEC"]:
            y = ord(getch());
            KEYBOARD_CONTROLLER.SPEC = 1;

            if y not in VAL:
                return ERROR;

            i = VAL.index(y); key = ID[i];

            if key in spec_cases:
                return spec_cases[key][0](*spec_cases[key][1]);

        elif callable(typewriter) and x not in [8, 9, 13, 26, 27, 167]:
            KEYBOARD_CONTROLLER.TPWT = 1;
            return typewriter();

        elif typewriter and (90 >= x >= 65):
            KEYBOARD_CONTROLLER.TPWT = 1;
            return chr(x);

        else:

            if x not in VAL:
                return ERROR;

            i = VAL.index(x); key = ID[i];

            if key in cases:
                return cases[key][0](*cases[key][1]);

        return 0;

#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

hxDRAWSPACE_X = 76;
hxDRAWSPACE_Y = 23;

hxDRAWUSED    =  0;
hxDRAWHEIGHT  =  0;

hxDEBUG       = -1;
hxEPRINT      =  1;

def CPRINT(s, flush=0, file=None):
    print(s, sep='', end='', flush=flush, file=file);

def DEBUG_PRINT(d=None):

    global hxDEBUG;

    if d:
        hxDEBUG = d;
        return None;

    return hxDEBUG;

def DISABLE_EPRINT(d=None):

    global hxEPRINT;

    if d:
        hxEPRINT = d;
        return None;

    return hxEPRINT;

hxNVRASK = 0;

def DISABLE_CONFIRM(d=None):

    global hxNVRASK;

    if d:
        hxNVRASK = d;
        return None;

    return hxNVRASK;

ERRLEVELS =                                 {

   -1: "LBK",
    0: "SYS",
    1: "WRN",
    2: "ERR",
    3: "FTL"

                                            };

PALETTE   = {};                             # color dict. filled @STARTUP

import inspect;

#   ---     ---     ---     ---     ---

class COORD:

    def __init__(self, x, tx, bx, y, ty, by):

        self.tx = max(tx, 0            );
        self.bx = min(bx, hxDRAWSPACE_X);

        self.ty = max(ty, 0            );
        self.by = min(by, hxDRAWSPACE_Y);

        self.x  = max(self.tx, min(x, self.bx));
        self.y  = max(self.ty, min(y, self.by));

#   ---     ---     ---     ---     ---

    def CLAMPX(self):
        self.x = max(self.tx, min(self.x, self.bx));

    def SETX(self, value):
        self.x = value; self.CLAMPX();

    def MODX(self, value):
        self.x += value; self.CLAMPX();

    def CLAMPY(self):
        self.y = max(self.ty, min(self.y, self.by));

    def SETY(self, value):
        self.y = value; self.CLAMPY();

    def MODY(self, value):
        self.y += value; self.CLAMPY();

#   ---     ---     ---     ---     ---

    @property
    def CO(self):
        return (self.x, self.y);

    @property
    def CHARNUM(self):
        return (self.bx - self.tx) * ((self.by + 1) - self.ty);

#   ---     ---     ---     ---     ---

    def WRAP(self, CO):

        changed = [0,0];
        if CO.x > self.bx:
            CO.SETX(self.tx); changed[0] = 1;
            if not CO.y >= self.by: CO.MODY(1);
        elif CO.x < self.tx:
            CO.SETX(self.bx); changed[0] = -1;
            if not CO.y <= self.ty: CO.MODY(-1);

        if CO.y > self.by:
            CO.SETY(self.by)
            changed[1] = 1;
        elif CO.y < self.ty:
            CO.SETY(self.ty);
            changed[1] = -1;

        return changed;

class CSTR:

    def __init__(self, buff="", linenum=20, linesize=74, pages=4):

        self.buff   =[ "" for i in range(linesize*linenum*pages)  ];
        self.escapes=[                                            ];

        if buff:
            i=0; j=0; k=0; escape=0;
            while j < len(buff):
                char=buff[j];

                if char=="$":
                    if buff[j:j+2] == "$:":
                        escape=1; self.escapes.append([i, "$:"]); j+=2;
                        continue;

                elif char==";":
                    if buff[j:j+2] == ";>":
                        escape=0; self.escapes[k][1] = self.escapes[k][1]+";>"; k+=1; j+=2;
                        continue;

                if not escape:
                    self.buff[i]=char; i+=1;
                else:
                    self.escapes[k][1]=self.escapes[k][1]+char;

                j+=1;

        self.linesize = linesize;
        self.linenum  = linenum;
        self.pages    = pages;
        self.pagesize = linesize*linenum;
        self.maxsize  = self.pagesize*self.pages;

    def COPY(self, buff):
        return CSTR(buff=buff, linesize=self.linesize, linenum=self.linenum, pages=self.pages);

    def __len__(self):
        return len("".join(self.buff));

    def __iter__(self):
        return self.buff.__iter__();

    def __getitem__(self, idex):

        if isinstance(idex, slice):
            l=[ i for i in range( *idex.indices(self.pagesize*self.pages) ) ];
            if len(l)>1: start, stop=l[0], l[-1]+1;
            else: start, stop=0,1;
        else:
            start, stop=idex, idex+1;

        if self.escapes:
            i=0; j=start; s="";
            for escape in self.escapes:
                i, code=escape;
                if stop>=i>=start:
                    s=s+"".join(ch for ch in self.buff[j:i])+code; j=i;

            return s+"".join(ch for ch in self.buff[j:stop]);

        else:
            return "".join(self.buff[start:stop]);

    def __setitem__(self, idex, value):
        self.buff[idex] = value;

    def __contains__(self, value):
        return item in self.buff;

    def __repr__(self):

        if self.escapes:
            i=0; j=0; s="";
            for escape in self.escapes:
                i, code=escape; s=s+"".join(ch for ch in self.buff[j:i])+code; j=i;

            return s+"".join(ch for ch in self.buff[j:]);

        else:
            return "".join(self.buff[:]);

#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

REGION_ITEMS_NEXT  = -1;
REGION_ITEMS_CURR  = -2;
REGION_ITEMS_PREV  = -3;

REGION_ITEMS_UP    = -4;
REGION_ITEMS_LEFT  = -5;
REGION_ITEMS_DOWN  = -6;
REGION_ITEMS_RIGHT = -7;

class REGION:

    def __init__(self, name, mode="BLANKOUT", items=None, items_ptr=-1, buff=None,
                 rect=COORD(1,1,76,22,22,23), borders=(1,1,1,1), corners=(12,6,9,3),
                 labels=None, align=(0,0,0),                                        ):

        if not labels:
            labels = ["", ""];

        if not items:
            items = [];

        self.BORDERS   = borders;
        self.ITEMS     = items; self.buildItemMap();
        self.ITEMS_PTR = items_ptr;

        self.MODE      = mode;
        self.NAME      = name;

        self.RECT      = rect;
        self.CORNERS   = corners;

        self.BUFF      = buff if buff != None else "";
        self.BUFF_PTR  = 0;
        self.BUFF_WRIT = 0;

        self.ALIGN     = align;
        self.LABELS    = labels;

        self.pad       = (self.RECT.bx-1) - (self.RECT.tx+1);

        self.PAGE      = 0;
        self.PAGES     = 0;
        self.PAGESIZE  = ( (self.RECT.by-1) - (self.RECT.ty+1) - 1 ) * 2;
        self.ITEMSIZE  = int( (self.pad-6)/2 )

        self.isCurrent = 0;

        self.onPagHit  = None;
        self.onEscHit  = ();

#   ---     ---     ---     ---     ---

    def getPageOffset(self):
        return self.PAGE * self.PAGESIZE;

    def AVPAG(self, trigger=1):
        if self.PAGE < (self.PAGES-1):
            self.PAGE+=1;
        else:
            self.PAGE =0;

        if self.onPagHit and trigger: self.onPagHit();

    def REPAG(self, trigger=1):
        if self.PAGE > 0:
            self.PAGE-=1;
        else:
            self.PAGE =(self.PAGES-1);

        if self.onPagHit and trigger: self.onPagHit();

    def ESC(self):
        if self.onEscHit: self.onEscHit[0](*self.onEscHit[1]);

    def buildItemMap(self):

        self.ITEMS_MAP = {};
        for elem in self.ITEMS:
            if elem["y"] not in self.ITEMS_MAP:
               self.ITEMS_MAP[ elem['y'] ] = { elem['x']: elem["i"] };
            else:
                self.ITEMS_MAP[ elem['y'] ][ elem['x'] ] = elem["i"];

    def addItem(self, x, y, label, style=0, info="", func=None, args={}):

        #self.ITEMS_PTR      += 1;
        new_elem = { "x"    : self.RECT.tx+(self.BORDERS[1]&1)+x,
                     "y"    : self.RECT.ty+(self.BORDERS[0]&1)+y,
                     "i"    : len(self.ITEMS),                  #
                                                                #
                     "label": label,                            #
                     "style": style,                            #
                     "info" : info,                             #
                     "func" : func,                             #
                     "args" : args                              };

        self.ITEMS.append(new_elem);
        if new_elem['y'] not in self.ITEMS_MAP:
            self.ITEMS_MAP[ new_elem['y'] ] = { new_elem['x']: new_elem["i"] };
        else:
            self.ITEMS_MAP[ new_elem["y"] ][ new_elem["x"] ] = new_elem["i"];

        return new_elem;

#   ---     ---     ---     ---     ---

    def ITEM_ACCESS(self, func_id, idex):
        if len(self.ITEMS):

            if -4 < idex < 0:
                self.ITEMS_PTR += (idex+2);
                arrtop = len(self.ITEMS) - 1;
                if   self.ITEMS_PTR > arrtop:
                    self.ITEMS_PTR = 0;
                elif self.ITEMS_PTR < 0:
                    self.ITEMS_PTR = arrtop;

                idex = self.ITEMS_PTR;

            elif -7 <= idex <= -4:

                elem    = self.ITEMS[self.ITEMS_PTR];
                x, y    = elem["x"], elem["y"];

                if idex in (REGION_ITEMS_RIGHT, REGION_ITEMS_LEFT):

                    xvalues = sorted( list(self.ITEMS_MAP[y].keys()) );

                    h_idex  = xvalues.index(x);
                    h_idex -= 1 if idex == REGION_ITEMS_RIGHT else -1;

                    arrtop  = len(xvalues) - 1;
                    if   h_idex > arrtop:
                        h_idex = 0;
                    elif h_idex < 0:
                        h_idex = arrtop;

                    idex    = self.ITEMS_MAP[y][xvalues[h_idex]];

                else:

                    yvalues = sorted( list(self.ITEMS_MAP.keys()) );

                    v_idex  = yvalues.index(y);
                    v_idex += 1 if idex == REGION_ITEMS_DOWN else -1;

                    arrtop  = len(yvalues) - 1;
                    if   v_idex > arrtop:
                        v_idex = 0;
                    elif v_idex < 0:
                        v_idex = arrtop;

                    xvalues = list(self.ITEMS_MAP[yvalues[v_idex]].keys());
                    if x in xvalues:
                        idex = self.ITEMS_MAP[yvalues[v_idex]][x];
                    else:
                        idex = self.ITEMS_PTR;

            elif idex >= len(self.ITEMS) or idex < -7:
                return ERROR;

            self.ITEMS_PTR = idex;
            return REGION.ITEM_FUNCS[func_id](self, self.ITEMS[idex]);

        return ERROR;

    def popItem(self, elem):

        hasptr    = elem["style"] > 1;

        ptrchar_s = f"{BOXCHARS['PTR_S']}" if self.ALIGN[1] in (1, 2) and hasptr else "";
        ptrchar_e =    BOXCHARS['PTR_E']   if self.ALIGN[1] in (0, 2) and hasptr else "";
        SEL       = "SEL%s"%elem["style"];

        s         = f"$:CURSOR.JUMP{elem['x']-len(ptrchar_s), elem['y']};>"\
                  + PALETTE['BACK']\
                  + " " * (len(ptrchar_s) + len(elem['label']) + len(ptrchar_e))

        del self.ITEMS_MAP[elem['y']][elem['x']];
        elem = self.ITEMS.pop(elem['i']);

        for item in self.ITEMS[elem['i']:]:
            item['i'] -= 1;

        self.buildItemMap();
        return (s, elem);

    def selectItem(self, elem):

        hasptr    = elem["style"] > 1;
        label     = elem["label"];
        maxlen    = self.ITEMSIZE-4 if len(self.ITEMS) > int(self.PAGESIZE/2) else self.pad-6;

        if (len(label) > maxlen):
            label = elem['label'] = label[:maxlen]+"..";

        ptrchar_s = f"{BOXCHARS['PTR_S']}" if self.ALIGN[1] in (1, 2) and hasptr else "";
        ptrchar_e =    BOXCHARS['PTR_E']   if self.ALIGN[1] in (0, 2) and hasptr else "";
        SEL       = "SEL%s"%elem["style"];

        return f"$:CURSOR.JUMP{elem['x']-len(ptrchar_s), elem['y']};>"\
               + PALETTE['PTR'] + ptrchar_s\
               + PALETTE[SEL] + label\
               + PALETTE['PTR'] + ptrchar_e;

    def deselectItem(self, elem):

        hasptr    = elem["style"] > 1;
        label     = elem["label"];
        maxlen    = self.ITEMSIZE-4 if len(self.ITEMS) > int(self.PAGESIZE/2) else self.pad-6;

        if (len(label) > maxlen):
            label = elem['label'] = label[:maxlen]+"..";

        length_s  = len(BOXCHARS['PTR_S']);
        length_e  = len(BOXCHARS['PTR_E']);

        ptrchar_s = " "*length_s if self.ALIGN[1] in (1, 2) and hasptr else "";
        ptrchar_e = " "*length_e if self.ALIGN[1] in (0, 2) and hasptr else "";
        DSEL      = "DSEL%s"%elem["style"];

        return f"$:CURSOR.JUMP{elem['x']-len(ptrchar_s), elem['y']};>"\
               + PALETTE['BOX'] + ptrchar_s\
               + PALETTE[DSEL] + label\
               + PALETTE['BOX'] + ptrchar_e;

    def callItem(self, elem):
        if elem['func']:
            return elem['func'](**elem['args']);

        return None;

    ITEM_FUNCS = { "select"  : selectItem,
                   "deselect": deselectItem,
                   "pop"     : popItem,
                   "call"    : callItem
                 };

#   ---     ---     ---     ---     ---

    def GETBOXLINE(self):
        m = self.drawTop();
        for y in range(self.RECT.ty+1, self.RECT.by):
            m = m + self.drawMid(y);

        return m + self.drawBottom();

    def WRITE(self, S):

        global hxSGY;

        if S == "$B":
            S  = self.GETBOXLINE();

        LENGTH = DISCOUNT_ESCAPES(S, len(S))[1];
        BUFF   = S[:LENGTH].split("$:");

        for sub in BUFF:

            DOJUMP = self.RECT.WRAP(CURSOR);
            if DOJUMP[0] or DOJUMP[1]: CURSOR.UPDATE();

            if ";>" in sub:
                com, sub = sub.split(";>"); nt=eval(com);
                if nt: sub=nt+sub;

            CPRINT(sub);

        CURSOR.x += LENGTH;
        if not LENGTH: return "";
        return S[LENGTH:];

    def SPACEAT(self):
        return self.RECT.bx - CURSOR.x;

    def GOTOP(self):
        CURSOR.JUMP(self.RECT.tx, self.RECT.ty);

    def GOBUTT(self):
        CURSOR.JUMP(self.RECT.bx, self.RECT.by);

    def getCornerChar(self, x, h, v):

        if not x:
            return "$:CURSOR.GORIGHT();>";

        thick = (1 + ((self.BORDERS[h] & BORDER_THICK_H)
                     |(self.BORDERS[v] & BORDER_THICK_V)) );

        key = CORNERDICT[x];
        return BOXCHARS[key][thick];

    def drawLabels(self, top=1):

        if top:
            thick  = self.BORDERS[0];
            hchar  = (BOXCHARS["HH"][thick] if thick < 8 else BOXCHARS["TR"][8]);
            label  = self.LABELS[0];
            align  = self.ALIGN[0];
        else:
            thick  = self.BORDERS[2];
            hchar  = BOXCHARS["HH"][thick] if thick < 8 else BOXCHARS["BR"][8];
            label  = self.LABELS[1];
            align  = self.ALIGN[2];

        if label:
            pdchr_s = BOXCHARS["LABEL_S"]; lpdc_s = len(pdchr_s);
            pdchr_e = BOXCHARS["LABEL_E"]; lpdc_e = len(pdchr_e);

            rlength = len(label); space = (self.pad-(lpdc_s+lpdc_e+(len(hchar)*2)))
            if rlength > space:
                label   = label[:(space-2)]+"..";
                rlength = len(label);

            length  = (lpdc_s + rlength + lpdc_e);

            if top and self.isCurrent:
                label = (f"{pdchr_s}{PALETTE['SEL1']+label+PALETTE['BOX']}{pdchr_e}");
            else:
                label = (f"{pdchr_s}{PALETTE['DSEL0']+label+PALETTE['BOX']}{pdchr_e}");

        else:
            length = 0;

        space = self.pad;
        if align == 1:
            wsp_e = hchar; space -= len(hchar);
            space -= length; wsp_s = hchar * space;

        elif align == 2:
            w = int((self.pad)/2) - int(length/2);
            wsp_s = (hchar * (w)); space -= length+w; wsp_e = hchar * space;

        else:
            wsp_s = hchar; space -= len(hchar);
            space -= length; wsp_e = hchar * space;

        return wsp_s + label + wsp_e;

    def drawTop(self):
        return  ( f"$:CURSOR.JUMP{self.RECT.tx, self.RECT.ty};>"

                + PALETTE["BOX"]
                + self.getCornerChar(self.CORNERS[0], 0, 1      )

                + self.drawLabels   (                           )

                + PALETTE["BOX"]
                + self.getCornerChar(self.CORNERS[1], 0, 3      ) );

    def drawMid(self, y):

        vchar_s = f"$:CURSOR.JUMP{self.RECT.tx,   y};>"\
                + BOXCHARS["VV"][self.BORDERS[1]];

        vchar_e = f"$:CURSOR.JUMP{self.RECT.bx-1, y};>"\
                + BOXCHARS["VV"][self.BORDERS[3]];

        s = "";
        if y in self.ITEMS_MAP:

            head = len(BOXCHARS['PTR_S']) if self.ALIGN[1] in (1, 2) else 0;
            tail = len(BOXCHARS['PTR_E']) if self.ALIGN[1] in (0, 2) else 0;

            for x in self.ITEMS_MAP[y]:

                idex    = self.ITEMS_MAP[y][x];
                SEL     = self.ITEMS_PTR == idex;
                label   = self.ITEM_ACCESS("select" if SEL else "deselect", idex);

                s       = s + label;

        return PALETTE["BOX"] + vchar_s + s + PALETTE["BOX"] + vchar_e;

    @property
    def VSPACE(self):
        return ( self.RECT.by - (self.RECT.ty+1) );

    def spitBuff(self, y):

        color  = PALETTE["BACK"] if not isinstance(self.BUFF, CSTR) else "";

        space  = self.pad;
        tjump  = f"$:CURSOR.JUMP{self.RECT.tx+1, y};>"
        sub    = color+self.BUFF[self.BUFF_PTR:self.BUFF_PTR+space];
        s      = tjump + sub; self.BUFF_PTR += space;

        return s;

    def addBuff(self, new_s):

        if not new_s: return 0;

        frmtd = ""; cur=0; PAGESIZE=self.VSPACE*self.pad;
        for y in range( (self.VSPACE*self.PAGES) ):

            new_s=CSTR(buff=new_s, linenum=20, linesize=74, pages=4);
            line=[]; space=self.pad; breakline=0;

            for sub in new_s[cur:cur+space].split(" "):

                if "\n" in sub:
                    breakline=1; sub=sub.replace("\n", "");

                sub = CSTR(sub + " ", linenum=20, linesize=74, pages=4);

                length=len(sub);

                if (length-1) < space:
                    line.append(repr(sub)); space-=length; cur+=length;

                else        : break;
                if breakline: break;

            if not len([sub for sub in line if sub.replace(" ", "")]): continue;
            line.append(" "*(space));
            frmtd = frmtd + ("".join(line));
            if cur > len(new_s): break;

        BUFFMAX=PAGESIZE*self.PAGES;
        while (len(self.BUFF) + len(new_s)) > BUFFMAX:
            self.BUFF=self.BUFF.COPY(self.BUFF[PAGESIZE:]); self.REPAG(0);

        new_s=CSTR(buff=frmtd, linenum=1, linesize=len(frmtd), pages=1);
        self.BUFF=self.BUFF.COPY(self.BUFF[:]+new_s[:]); trigger=0; ptr=self.PAGE*PAGESIZE;
        while (ptr+PAGESIZE) < len(self.BUFF):
            ptr+=PAGESIZE; self.AVPAG(0); trigger=1;

        return trigger;

    def appBuff(self):
        s=""; length=len(self.BUFF);
        self.BUFF_PTR=(self.VSPACE*self.pad*self.PAGE)+(self.pad*self.BUFF_WRIT);
        for y in range(self.RECT.ty+1+self.BUFF_WRIT, self.RECT.by):
            s = s + self.spitBuff(y); self.BUFF_WRIT+=1;
            if self.BUFF_PTR >= length: break;

        return s;

    def refBuff(self):

        for y in range(self.RECT.ty+1, self.RECT.by):
            CPRINT(f"\x1b[{y};{self.RECT.tx+1}H" + (" "*self.pad));

        s=""; self.BUFF_PTR=(self.VSPACE*self.pad)*self.PAGE;

        i=0;length=len(self.BUFF);
        for y in range(self.RECT.ty+1, self.RECT.by):
            s = s + self.spitBuff(y);
            if self.BUFF_PTR >= length: break;
            i+=1;

        self.BUFF_WRIT=i;

        return s;

    def updateBuff(self, new_s):

        if new_s == self.BUFF:
            return "";

        old_length = len(self.BUFF);
        new_length = len(new_s    );

        for y in range(self.RECT.ty+1, self.RECT.by):
            CPRINT(f"\x1b[{y};{self.RECT.tx+1}H" + (" "*self.pad));

        s = ""; self.BUFF_PTR = 0;
        if len(new_s) > self.pad:

            frmtd = ""; cur=0;
            for y in range(self.RECT.ty+1, self.RECT.by):

                line=[]; count=0; space=self.pad;
                for sub in new_s[cur:cur+self.pad].split(" "):
                    sub = sub + " "; length = len(sub);
                    if (length-1) < space:
                        line.append(sub); space-=length; cur+=length;

                line.append(" "*(space));
                frmtd = frmtd + ("".join(line));
                if cur > len(new_s): break;

            new_s = frmtd;

        self.BUFF = new_s;
        for y in range(self.RECT.ty+1, self.RECT.by):
            s = s + self.spitBuff(y);
            if self.BUFF_PTR >= new_length: break;

        return s;

    def drawBottom(self):
        return  ( f"$:CURSOR.JUMP{self.RECT.tx, self.RECT.by};>"

                + PALETTE["BOX"]
                + self.getCornerChar(self.CORNERS[2], 2, 1            )

                + self.drawLabels   (top=0                            )

                + PALETTE["BOX"]
                + self.getCornerChar(self.CORNERS[3], 2, 3            ) );

class LAYOUT:

    def __init__(self, regions, info_region=0):

        self.REGIONS = { region.NAME:region for region in regions };
        self.SEL     = regions[0].NAME;
        self.INFO    = regions[0];

    @property
    def ACTIVE_REGION(self):
        return self.REGIONS[self.SEL];

class KVRSOR(COORD):

    def __init__(self, x, tx, bx, y, ty, by):
        COORD.__init__(self, x, tx, bx, y, ty, by);

        self.saved     = (1,1);

#   ---     ---     ---     ---     ---

    def GORIGHT(self, x=1):
        CPRINT(f"\x1b[{x}C"); self.MODX( x);

    def GOLEFT(self, x=1):
        CPRINT(f"\x1b[{x}D"); self.MODX(-x);

    def XJUMP(self, x=1):
        CPRINT(f"\x1b[{x}G"); self.SETX( x);

#   ---     ---     ---     ---     ---

    def GODOWN(self, y=1):
        CPRINT(f"\x1b[{y}B"); self.MODY( y);

    def GOUP(self, y=1):
        CPRINT(f"\x1b[{y}A"); self.MODY(-y)

    def YJUMP(self, y=1):
        CPRINT(f"\x1b[{y};{self.x}H"); self.SETY(y);

#   ---     ---     ---     ---     ---

    def JUMP(self, x=1, y=1):
        CPRINT(f"\x1b[{y};{x}H");
        self.SETY(y); self.SETX(x);

    def UPDATE(self):
        CPRINT(f"\x1b[{self.y};{self.x}H");

#   ---     ---     ---     ---     ---

    def SAVE(self):
        self.saved = (self.y, self.x);
    def LOAD(self):
        self.JUMP(*self.saved);

CURSOR = KVRSOR(1,1,75,1,1,23);
def GETKVRSOR(): return CURSOR;

#   ---     ---     ---     ---     ---

class KVNSL:

    def __init__(self):
        self.S         = None;
        self.SCREENS   = {};

        self.INTCALLS  = {};
        self.KINTCALLS = {};
        self.DEBUGREG  = None;
        self.PREVSCR   = "";

        CURSOR.JUMP(1,22);

    def AP_INTCALL(self, ID, call, args, k=0):
        if k:
            self.KINTCALLS[ID] = (call, args);
        else:
            self.INTCALLS[ID]  = (call, args);

    def DL_INTCALL(self, ID, k=0):
        if k:
            del self.KINTCALLS[ID];
        else:
            del self.INTCALLS[ID];

    def NEW_SCREEN(self, ID, layout):
        self.SCREENS[ID] = layout;
        self.S           = ID;

        region = self.CUR_REGION;
        region.isCurrent = 1;

    @property
    def CUR_SCREEN(self):
        return self.SCREENS[self.S];

    @property
    def CUR_REGION(self):
        return self.CUR_SCREEN.ACTIVE_REGION;

    def CHREGION(self, new_ID):

        region = self.CUR_REGION;
        region.isCurrent = 0;

        self.OUT(region.drawTop());

        self.ID    = new_ID;
        SCREEN     = self.CUR_SCREEN;
        SCREEN.SEL = self.ID;

        region = self.CUR_REGION;
        region.isCurrent = 1;

        self.OUT(region.drawTop());

    def CLREGION(self, ID=None, title=None):

        if not ID:
            region = self.CUR_REGION;
        else:
            region = self.CUR_SCREEN.REGIONS[ID];

        if not title:
            title = region.LABELS[0];

        yvalues = [elem['y'] for elem in region.ITEMS];
        ws      = "";

        for i in range(len(region.ITEMS)):
            s, elem = region.ITEM_ACCESS("pop", 0);
            ws      = ws + s;

        region.LABELS[0] = title;
        self.OUT(ws + region.drawTop());

    def FLREGION(self, ID=None):
        if not ID:
            region = self.CUR_REGION;
        else:
            region = self.CUR_SCREEN.REGIONS[ID];

        region.ITEMS_PTR = 0;
        INFO = self.CUR_SCREEN.INFO;

        s = "";
        for elem in region.ITEMS:
            s = s + region.drawMid(elem['y']);

        itm, info = self.RNAVIGATE(0);
        self.OUT(s+itm, flush=0); self.OUT(info, region=INFO, flush=1);

    def FILL(self, ID=None):

        if ID:
            region = self.CUR_SCREEN.REGIONS[ID];
        else:
            region = self.CUR_REGION;

        region.GOTOP();
        self.OUT(" "*region.RECT.CHARNUM, region);

    def FILLSCREEN(self):
        SCREEN = self.CUR_SCREEN;
        for region in SCREEN.REGIONS.values():
            region.GOTOP(); self.OUT("$B", region=region, flush=0);

        CPRINT("", 1);

    def CLS(self):
        col=MSCPX(PALETTE['BACK']);
        for y in range(1, hxDRAWSPACE_Y):
            CPRINT(f"{col}\x1b[{y};1H\x1b[K");

        CPRINT("", 1);

    def OUT(self, s, region=None, flush=1):

        if not region: region = self.CUR_REGION;
        if not isinstance(s, str): s = str(s);
        while s: s = region.WRITE(s);

        #self.NO_YSCROLL();
        CPRINT("\x1b[0m", flush);

    def NO_YSCROLL(self):
        if CURSOR.y > (hxDRAWSPACE_Y-1): CPRINT("\x1b[T");

    def REXEC(self):
        region = self.CUR_REGION;
        return region.ITEM_ACCESS("call", REGION_ITEMS_CURR);

    def RNAVIGATE(self, dirn):
        region = self.CUR_REGION;
        if not region.ITEMS: return "", "";

        info   = self.CUR_SCREEN.INFO;
        oldlen = len(info.BUFF);
        s      = "";

        s      = s + region.ITEM_ACCESS("deselect", REGION_ITEMS_CURR );
        s      = s + region.ITEM_ACCESS("select",   dirn              );
        info_s = info.updateBuff(region.ITEMS[region.ITEMS_PTR]["info"]);

        return s, info_s;

    def RNAVIGATE_NI(self, dirn):
        region = self.CUR_REGION;
        s      = "";

        s      = s + region.ITEM_ACCESS("deselect", REGION_ITEMS_CURR );
        s      = s + region.ITEM_ACCESS("select",   dirn              );

        return s;

    def SWAPWIPE(self, src, tgt):

        INFO    = self.CUR_SCREEN.INFO;
        region  = self.CUR_SCREEN.REGIONS[src];
        ws      = "";

        for i in range(len(region.ITEMS)):
            s, elem = region.ITEM_ACCESS("pop", 0);
            ws      = ws + s;

        self.OUT(ws); self.CHREGION(tgt);
        itm, info = self.RNAVIGATE(self.CUR_REGION.ITEMS_PTR);
        self.OUT(itm, flush=0); self.OUT(info, region=INFO, flush=1);

    def DEBUG_TOGGLE(self):

        self.CLS();

        if not self.PREVSCR:
            self.PREVSCR=self.S;
            self.S="DEBUGWIN";
            self.DEBUGREG.isCurrent=1;
        else:
            self.S=self.PREVSCR;
            self.PREVSCR=""; self.DEBUGREG.isCurrent=0;

        self.FILLSCREEN();
        region=self.CUR_REGION;

        if self.PREVSCR:
            self.OUT(self.DEBUGREG.refBuff());

        else:
            for region in self.CUR_SCREEN.REGIONS.values():
                if region.ITEMS:
                    itm, info = self.RNAVIGATE(region.ITEMS_PTR);
                    self.OUT(itm, flush=0);
                    if region.isCurrent: self.OUT(info, region=self.CUR_SCREEN.INFO, flush=0);

    def DEBUG_OUT(self, args, kwargs):
        for s in args:
            if not isinstance(s, str): s=str(s);

        ERRPRINT(*args, **kwargs); self.OUT(self.DEBUGREG.appBuff());

    def DEBUG_SPIT(self):
        self.OUT(self.DEBUGREG.appBuff(), region=self.DEBUGREG);

    def RUN(self):

        ptr = 0; s = "►▼◄▲";

        CURSOR.SAVE(); CURSOR.JUMP(1, 23);
        col0=MSCPX(PALETTE['BACK']); col1=MSCPX(PALETTE['SEL1']);

        CPRINT(f"\x1b[?25l{col0}\x1b[K");

        # TODO: make an actual clock to propertly handle this loop
        FPS=60; frametime=(1/FPS); framedelta=0.0;
        framestart=0.0; framestop=0.0; fBy = 0.0;

        CURSOR.LOAD(); shift=0; animframe=0.0; animrate=frametime*6;

#   ---     ---     ---     ---     ---

        while True:

            framestart = rnow();
            if framestop: framedelta += (framestart - framestop);

            CURSOR.SAVE();

#   ---     ---     ---     ---     ---

            if kbhit():

                if not self.KINTCALLS:
                    spec_cases = {

                        "ARROWUP"   : [ self.RNAVIGATE, [REGION_ITEMS_UP   ] ],
                        "ARROWLEFT" : [ self.RNAVIGATE, [REGION_ITEMS_LEFT ] ],
                        "ARROWDOWN" : [ self.RNAVIGATE, [REGION_ITEMS_DOWN ] ],
                        "ARROWRIGHT": [ self.RNAVIGATE, [REGION_ITEMS_RIGHT] ],

                        "AVPAG"     : [ self.CUR_REGION.AVPAG, []            ],
                        "REPAG"     : [ self.CUR_REGION.REPAG, []            ],

                    };

                    cases = {

                        "RETURN"    : [ self.REXEC,        []                ],
                        "TAB"       : [ self.DEBUG_TOGGLE, []                ],
                        "ORDM"      : [ self.DEBUG_OUT,    [["test debug"], {'err':-1} ]]

                    };

                    x = KEYBOARD_CONTROLLER.RUN(cases, spec_cases, 1);

#   ---     ---     ---     ---     ---

                    if isinstance(x, str):
                        if KEYBOARD_CONTROLLER.TPWT:
                            CURSOR.JUMP(2+shift, 21);
                            CPRINT(x); shift+=1;

                    elif KEYBOARD_CONTROLLER.SPEC:

                        if isinstance(x, tuple):
                            itm, info  = x;
                            inforegion = self.CUR_SCREEN.INFO;

                            self.OUT(itm,                     flush=0);
                            self.OUT(info, region=inforegion, flush=0);

                    elif KEYBOARD_CONTROLLER.LOG == 27:

                        if self.CUR_REGION.NAME=="RPANEL":
                            self.CUR_REGION.LABELS[0]="";
                            self.SWAPWIPE("RPANEL", "LPANEL");

                        elif self.CUR_REGION.onEscHit:
                            self.CUR_REGION.ESC(); self.CUR_REGION.onEscHit = [];

                        else:
                            break;

#   ---     ---     ---     ---     ---

                else:
                    x = "";
                    for func, args in self.KINTCALLS.values():
                        x = x + func(*args);

                    self.OUT(x);

#   ---     ---     ---     ---     ---

            CURSOR.JUMP(2, 23);

            if animframe >= animrate:

                if ptr < 3:
                    ptr += 1; CPRINT(f"\x1b[1G{col1}"\
                                     f" {s[ptr]}");
                elif ptr == 3:
                    ptr += 1;
                else:
                    ptr = 0; CPRINT(f"\x1b[1G{col1}"\
                                     f" {s[ptr]}");

                animframe = animframe - animrate;

            framestop = rnow(); framedelta += (framestop - framestart);
            fBy = 1.0 * framedelta; animframe += fBy;

            if framedelta < frametime:
                sleep((frametime - framedelta)); framedelta = 0;
            else:
                framedelta = framedelta - frametime;

            CPRINT("\x1b[0m", 1);
            CURSOR.LOAD();

        CPRINT("\x1b[?25h");
        CPRINT("", 1);

#   ---     ---     ---     ---     ---

KVNSL_H  = KVNSL();
DEBUGREG = REGION ( "DEBUG",

                    rect=COORD( 1,  1, 76, 1, 1, 22 ),
                    labels=["DEBUG", "1/4"],
                    borders=tuple(PKBORDER(1,1,1) for i in range(4)),
                    align=(0,0,1),

                    buff=CSTR(linenum=20, linesize=74, pages=4));

KVNSL_H.NEW_SCREEN("DEBUGWIN", LAYOUT([DEBUGREG]));

def DEBUG_ONPAG():
    DEBUGREG.LABELS[1]=f"{DEBUGREG.PAGE+1}/{DEBUGREG.PAGES}"
    KVNSL_H.OUT(DEBUGREG.drawBottom()+DEBUGREG.refBuff());

DEBUGREG.PAGES=4; DEBUGREG.onPagHit=DEBUG_ONPAG; KVNSL_H.DEBUGREG=DEBUGREG;

#   ---     ---     ---     ---     ---

def QLNAME(frame):

    code = frame.f_code;
    name = code.co_name;
    qual = None;

    for cls in ( obj for obj in frame.f_globals.values()  #
                     if  inspect.isclass(obj)             ):

        if hasattr(cls, name):

            member = getattr(cls, name);
            if not inspect.isfunction(member): continue;

            if member.__code__ == code:
                qual = member.__qualname__;
                break;

    return qual;

def GETERR(err):
    return PALETTE[err] + ERRLEVELS[err];

def ERRPRINT(*args, err=0, rec=1, sep=' ', end=''):

    global hxEPRINT;
    if not hxEPRINT: return None;

#   ---     ---     ---     ---     ---

    global hxDEBUG; rec = max(1, min(rec, 7));
    mess = f"{sep.join(s for s in args)}{end}";

    prev   = sys._getframe(rec);
    kls    = QLNAME(prev);

    caller = prev.f_code.co_name if not kls else kls;
    file   = prev.f_code.co_filename.replace(f"{ROOT()}\\", "").split("\\");

    if rec > 1:
        last     = sys._getframe(1);
        reporter = last.f_code.co_name;

    else:
        reporter = caller;

    mess   = mess.replace("$CALLER", caller  );
    mess   = mess.replace("$LAST",   reporter);

#   ---     ---     ---     ---     ---

    if hxDEBUG <= err:

        x      = len(file);

        if x == 3:
            file = f"<{file[0]}> {file[2]}";

        elif x == 4:
            file = f"<{file[2]}> {file[3]}";

        elif x == 5:
            file = f"<{file[2]}\\{file[3]}> {file[4]}";

        else:
            file = file[-1];

        line   = prev.f_lineno;
        mess   = f"{GETERR(err)}: {caller} on {file} @{line} " + mess;

    else:
        mess = f"{GETERR(err)}: {mess}"

#   ---     ---     ---     ---     ---

    # for automatic AVPAG
    if KVNSL_H.DEBUGREG.addBuff(mess + "\x1b[0m"):
        if DEBUGREG.isCurrent:
            DEBUG_ONPAG();
        else:
            DEBUGREG.LABELS[1]=f"{DEBUGREG.PAGE+1}/{DEBUGREG.PAGES}";

hxWARNS = True;

def FATAL_WARNINGS(d=None):

    global hxWARNS;

    if d:
        hxWARNS = d;
        return None;

    return hxWARNS;

hxWALL = False;

def FKWALL(d=None):

    global hxWALL;

    if d:
        hxWALL = d;
        return None;

    return hxWALL;

#   ---     ---     ---     ---     ---

hxROOT = "";

def ROOT(r=None):

    global hxROOT;

    if r:
        hxROOT = r;
        return None;

    return hxROOT;

#   ---     ---     ---     ---     ---

MCHUNK = 0;
def READSIZE(s=None):

    global MCHUNK;

    if s:
        MCHUNK = s;
        return None;

    return MCHUNK;

#   ---     ---     ---     ---     ---

CCPATH   = "";
CC64     = "";
PLATFORM = "";

def CC(c=None, c64=None, p=None):

    global CCPATH, CC64, PLATFORM;

    if c and c64 and p:
        CCPATH = c; CC64 = c64; PLATFORM = p;
        return None;

    elif PLATFORM == "x64":
        return CCPATH + CC64 + "\\bin\\"

    return CCPATH + "\\bin\\"

def TARGET(p=None):

    global PLATFORM;

    if not p:
        return PLATFORM;

    if p in ["64", "x64", 0]: PLATFORM = "x64";
    else                    : PLATFORM = "Win32";

    ERRPRINT("TARGET%s"%PLATFORM);

hxBTIME = 0.0;
def BTIME(f=None):

    global hxBTIME;
    if f:

        if f == 'rnow': f = time.time();

        hxBTIME = f;
        return None;

    return hxBTIME;

#   ---     ---     ---     ---     ---

def DOS(comm): os.system(comm );
def CLS(    ): DOS      ('cls');

#   ---     ---     ---     ---     ---

def SHPATH(path):
    return path.replace(f"{ROOT()}", "ROOT");

def OKPATH(path):
    return os.path.isdir (path) if os.path.exists(path) else False;

def OKFILE(path):
    return os.path.isfile(path) if os.path.exists(path) else False;

def FILESIZE(path):

    if OKFILE(path) : return os.path.getsize(path);
    else: ERRPRINT(f"<{SHPATH(path)}> -- unexistent or not a file", err=2, rec=2)

    return 0;

def DIRSIZE(path):

    if OKPATH(path) : return os.path.getsize(path);
    else: ERRPRINT(f"<{SHPATH(path)}> -- unexistent or not a directory", err=2, rec=2)

    return 0;

def WALK(path):
    return list(os.walk(path));

def LISTDIR(path, exts=[]):

    if exts:

        flist  = os.listdir(path);
        select = [];

        for e in exts:
            l = [f for f in flist if f".{e}" in f];
            if len(l): select.extend(l);

        return select;

    return os.listdir(path);

def EXTSOLVE(name, full):

    if len(name) >= 4 and len(full) >= 4:
        if name == full[:-4]: return 1;

    return 0;

#   ---     ---     ---     ---     ---

CHOICE_RECMAX = 20;

def CHOICE(mess, choices = ["Y", "N"], repeat = 0):

    choices = [ch.upper() for ch in choices];
    chs     = "\\".join(choices);

    chb     = [bytes(ch, 'ascii') for ch in choices];

    if not repeat:
        print(mess + " [" + chs + "]");

    c   = getch().upper();
    if c in chb:
        return 1 + chb.index(c);

    elif repeat < CHOICE_RECMAX:
        return CHOICE(mess, choices, repeat + 1);

    else:
        ERRPRINT("$LAST exceeded number of invalid inputs", err=2, rec=2);
        return 0;

def MULTICHOICE(mess, items, chain=0):

    i = ASCIBOX.MAKE(items, title=mess, chain=chain);

    if i == -1:
        #ERRPRINT("$LAST cancelled out of process", err=2, rec=2);
        return 0;

    return i;

#   ---     ---     ---     ---     ---

def MKDIR(path, recursive=1):

    if recursive:

        d = path.split("\\"); r = d[0]; 
        for sp in d[1:]:
            r = r + f"\\{sp}";
            if not OKPATH(r): os.mkdir(r);

        return 0;

    else:

        try:
            os.mkdir(r); return 0;

        except:
            ERRPRINT(f"$LAST can't create dir <{path}> because the path is invalid",
                      "$CALLER may have meant to turn on the recursive flag?",
                      err=2, rec=2, sep="\n"                                       );

            return 1;

#   ---     ---     ---     ---     ---

def SEP_DIRFRFILE(path):
    return "\\".join( x for x in (path.split("\\")[:-1]));

def SEP_FILEFRDIR(path):
    return path.split("\\")[-1];

#   ---     ---     ---     ---     ---

def FLFILE(path, ask=1):

    global hxNVRASK;
    if hxNVRASK: ask=0;

    if not OKFILE(path):
        ERRPRINT(f"File <{SHPATH(path)}> doesn't exist, cannot flush", err=2, rec=2);

    else:

        if ask:
            i = CHOICE(f"Erase contents of <{SHPATH(path)}> ?")
            if i == 2:
                ERRPRINT(f"Denied $CALLER-requested flush", rec=2);
                return None;

        open(path, 'w').close();

#   ---     ---     ---     ---     ---

def RDFILE(path, b=0, rh=0, rl=0, dec=0, rl_sep="\n", trunc=0, ask=1):

    global hxNVRASK;
    if hxNVRASK: ask=0;

    if not OKFILE(path):
        ERRPRINT(f"File <{SHPATH(path)}> doesn't exist, cannot read", err=2, rec=2);
        return (None, None);

    if trunc and rh: rh = 0;
    if dec and not b: dec = 0;

    size = FILESIZE(path);
    if ask:
        i = CHOICE(f"File <{SHPATH(path)}> is over ({(READSIZE()/1024):.2f}KB)\n",
                   f"Read anyway?"                                               );

        if i == 2:
            ERRPRINT("$CALLER attempted reading large file, but $LAST denied", rec=2);
            return None;

#   ---     ---     ---     ---     ---

    if not rl:

        if b:
            m = 'rb+' if trunc else 'rb';
            d = b"";

        else:
            m = 'r+' if trunc else 'r';
            d = "";

#   ---     ---     ---     ---     ---

        file = None; ERRPRINT(f"Opened file <{SHPATH(path)}>", rec=2);
        if rh:

            file = open(path, m);
            while size:

                rb = min(size, READSIZE()); # bytes to read clamped to bytes left

                s  = file.read(rb);         # read, cat, substract
                d  = d + s; size -= rb;

        else:

            with open(path, m) as file:

                while size:

                    rb = min(size, READSIZE());

                    s  = file.read(rb);
                    d  = d + s; size -= rb;

                if trunc: file.truncate(0);

            ERRPRINT(f"Closed file <{SHPATH(path)}>", rec=2);

#   ---     ---     ---     ---     ---

        if dec:

            try:
                d = d.decode('ascii');

            except UnicodeDecodeError:
                d = d.decode                ( 'utf-8'                                       );
                ERRPRINT                    ( "Your staunch defiance of ASCII is",          #
                                              "looked down uppon by dark forces unknown...",#
                                              err=-1, rec=2                                 );

        if rh: return (d, file);
        return (d, None);

#   ---     ---     ---     ---     ---

    file  = None; ERRPRINT(f"Opened file <{SHPATH(path)}>", rec=2);
    file  = open(path, 'r+');
    lines = rl_sep.join(line for line in file);

    if trunc:
        file.truncate(0);

    if not rh:
        file.close();
        ERRPRINT(f"Closed file <{SHPATH(path)}>", rec=2);

    return (lines, file);

#   ---     ---     ---     ---     ---

def MKFILE(path, s="", b=0, rh=0, ask=0):

    global hxNVRASK;
    if hxNVRASK: ask=0;

    d = SEP_DIRFRFILE(path);

    if not OKPATH(d):

        i = CHOICE(f"Unexistent directory <{SHPATH(d)}>. Create?");

        if i == 1:
            MKPATH(d);

        else:
            ERRPRINT("BAD PATH: $LAST cannot create file for $CALLER", err=2, rec=2);
            return None;

    if OKFILE(path) and ask:
        i = CHOICE(f"File <{SHPATH(path)}> already exists. Overwrite?");

        if i == 2:
            ERRPRINT("$CALLER attempted overwrite, but $LAST denied", rec=2);
            return None;

#   ---     ---     ---     ---     ---

    success_mess = f"$CALLER created file <{SHPATH(path)}>";
    m = 'wb' if b else 'w';

    if s:

        if b:
            if type(s) != bytes:

                try:
                    s = bytes(s, encoding='ascii')

                except UnicodeEncodeError:

                    s = bytes               ( s, encoding='utf-8'                           );
                    ERRPRINT                ( "Your staunch defiance of ASCII is",          #
                                              "looked down uppon by dark forces unknown...",#
                                              err=-1, rec=2                                 );

#   ---     ---     ---     ---     ---

        if rh:
            file.open(path, m);
            file.write(s); ERRPRINT(success_mess, rec=2);

            return file;                    # danger, no auto-close hook
                                            # TODO: handle this with a class maybe

        with open(path, m) as file:
            file.write(s); ERRPRINT(success_mess, rec=2);

        return 1;

#   ---     ---     ---     ---     ---

    if rh:
        open(path, m); ERRPRINT(success_mess, rec=2);

        return file;                        # same problem as above

    open(path, m).close(); ERRPRINT(success_mess, rec=2);
    return 1;

def CATPATH(*args):
    return '\\'.join(s for s in args);

#   ---     ---     ---     ---     ---

def DELF(path, ext=0):

    if OKFILE(path):

        DOS("@del %s"%path);
        ERRPRINT(f"$CALLER deleted file <{SHPATH(path)}>", rec=3 if ext else 2);

    else: ERRPRINT(f"Error deleting <{SHPATH(path)}>", err=2, rec=3 if ext else 2);

def DELD(path):

    if OKPATH(path):

        flist = LISTDIR(path);
        if len(flist):
            i = CHOICE(f"Directory <{SHPATH(path)}> is not empty.\n"\
                       +"All files in it will be lost. Is that OK?" );

            if i == 1:
                for f in flist:
                    DELF(f"{path}\\{f}", 1);

            else:
                ERRPRINT(f"$LAST aborted deletion of <{SHPATH(path)}>", rec=2);

        DOS("@rmdir %s"%path);
        ERRPRINT(f"$CALLER deleted directory <{SHPATH(path)}>", rec=2);

    else: ERRPRINT(f"Error deleting <{SHPATH(path)}>", err=2, rec=2);

#   ---     ---     ---     ---     ---

def FKTOUCH(f):
    t = BTIME();
    os.utime(f, (t, t));

def TOUCH(f):
    Path(f).touch();

def MODTIME(path):
    return os.path.getmtime(path);

def MTIMEVS(f1, f2):
    return MODTIME(f1) != MODTIME(f2);

def MTIMELT(flist):

    t      = MODTIME(flist[0]);
    lesser = flist[0];

    for f in flist[1:]:
        new_t = MODTIME(f);
        if new_t < t:
            t      = new_t;
            lesser = f;

    return lesser;

def MTIMEGT(flist):

    t       = MODTIME(flist[0]);
    greater = flist[0];

    for f in flist[1:]:
        new_t = MODTIME(f)
        if new_t > t:
            t       = new_t;
            greater = f;

    return greater;

#   ---     ---     ---     ---     ---

KVRLOG = None;
KVRIN  = None;

def CLEANUP():
    global KVRLOG, KVRIN, KVNSL_H;
    DELF(KVRIN); DELF(KVRLOG); del KVNSL_H;

def STARTUP(SETTINGS):

    UI_BKG=10; UI_FRG=7; UI_COL0=1; UI_COL1=0;

    PALETTE["SEL0" ] = ANSIC(UI_FRG,     UI_COL0   );
    PALETTE["SEL1" ] = ANSIC(UI_COL0+10, UI_BKG    );
    PALETTE["SEL2" ] = ANSIC(UI_FRG,     UI_BKG    );
    PALETTE["SEL3" ] = ANSIC(UI_COL0+10, UI_BKG    );

    PALETTE["DSEL0"] = ANSIC(UI_FRG,     UI_BKG    );
    PALETTE["DSEL1"] = ANSIC(UI_COL0,    UI_BKG    );
    PALETTE["DSEL2"] = ANSIC(UI_COL1,    UI_BKG    );
    PALETTE["DSEL3"] = ANSIC(UI_COL1,    UI_BKG    );

    PALETTE["BOX"  ] = ANSIC(UI_COL1,    UI_BKG    );
    PALETTE["PTR"  ] = ANSIC(UI_COL0+10, UI_BKG    );
    PALETTE["BACK" ] = ANSIC(UI_FRG,     UI_BKG    );
    PALETTE["KVRSE"] = ANSIC(UI_BKG,     UI_FRG    );

    PALETTE[-1     ] = ANSIC(15, UI_BKG);
    PALETTE[ 0     ] = ANSIC(16, UI_BKG);
    PALETTE[ 1     ] = ANSIC(12, UI_BKG);
    PALETTE[ 2     ] = ANSIC(13, UI_BKG);
    PALETTE[ 3     ] = ANSIC(11, UI_BKG);

#   ---     ---     ---     ---     ---

    r         = SETTINGS[ 0]; ROOT           (r              );

    ccd       = SETTINGS[ 2];
    ccd64     = SETTINGS[ 3];
    p         = SETTINGS[ 4]; CC             (ccd, ccd64, p  );

    read_size = SETTINGS[ 5]; READSIZE       (read_size      ); 
    warnfatal = SETTINGS[ 6]; FATAL_WARNINGS (warnfatal      );
    debugprnt = SETTINGS[ 7]; DEBUG_PRINT    (debugprnt      );
    disblprnt = SETTINGS[ 8]; DISABLE_EPRINT (not disblprnt  );
    nvrask    = SETTINGS[ 9]; DISABLE_CONFIRM(nvrask         );
    fkwall    = SETTINGS[10]; FKWALL         (fkwall         );

    global KVRLOG, KVRIN;
    logpath = ROOT() + "\\KVR\\trashcan\\log";
    if not OKPATH(logpath): MKDIR(logpath);

#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

    KVRLOG     = logpath + "\\KVNSLOG"; MKFILE(KVRLOG, ask=0);
    KVRIN      = logpath + "\\KVNSIN";  MKFILE(KVRIN,  ask=0);

    atexit.register(CLEANUP);

def GETKVNSL(): return KVNSL_H;

def LOGOS():
    global KVRLOG; return KVRLOG;

#\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

def SYSFLUSH():
    global KVRLOG; FLFILE(KVLOG, 0);

def SYSREAD(i=0, clear=1):
    global KVRLOG, KVRIN;

    F = [KVRLOG, KVRIN][i];

    global hxEPRINT; hxEPRINT = 0;
    log="";

    try:
        log = RDFILE(F, rl=1, rl_sep=" ", trunc=clear, ask=0)[0];
    finally:
        hxEPRINT = 1;

    return log;

def SYSDUMP():
    pass;

#   ---     ---     ---     ---     ---

class INPUTFIELD:

    @staticmethod
    def MAKE(x=1, y=1, length=hxDRAWSPACE_X, buff="", col="BACK", col_i="KVRSE", prompt='$:'):

        self        = INPUTFIELD();

        self.pos    = (x, y);
        self.buff   = buff;
        self.ptr    = len(buff);

        self.col    = PALETTE[col  ];
        self.col_i  = PALETTE[col_i];
        self.prompt = prompt;
        self.length = length;

        self.state  = 0;

        KVNSL_H.OUT(self.CCOLUMN());
        return self;

    @property
    def ch(self):
        if not len(self.buff): return " ";
        return self.buff[self.ptr] if self.ptr < len(self.buff) else " ";

    def RUN(self):

        spec_cases =                        {

            "ARROWLEFT" : [self.LEFT,      ()],
            "ARROWRIGHT": [self.RIGHT,     ()],
            "DELETE"    : [self.DELCUR,    ()],
            "START"     : [self.FCOLUMN,   ()],
            "END"       : [self.LCOLUMN,   ()]

                                            };

        cases =                             {

            "RETURN"    : [self.END,       ()],
            "ESCAPE"    : [self.END,       ()],
            "EXIT"      : [self.END,       ()],
            "ORDM"      : [self.END,       ()],

            "SPACE"     : [self.PASTECHAR, ()],
            "SEMICOLON" : [self.PASTECHAR, ()],
            "BACKSPACE" : [self.DELPREV,   ()]

                                            };

        s = KEYBOARD_CONTROLLER.RUN(cases, spec_cases, self.PASTECHAR);
        if isinstance(s, int): s = "";
        return self.CCOLUMN()+s;

    def WIPE(self):
        self.ptr = 0; self.buff = ""; self.LINEDRAW();

    def LINEDRAW(self):
        return ( f"$:CURSOR.XJUMP({self.pos[0]});>"+self.col+self.prompt+self.buff
               + ' ' * (self.length - len(self.buff)) );

    def RIGHT(self):
        if self.ptr == len(self.buff): return "";
        s = self.col+self.ch; self.ptr += 1;
        return s +self.col_i+self.ch+self.col+"$:CURSOR.GOLEFT();>";

    def LEFT(self):
        if self.ptr == 0: return "";
        s = "$:CURSOR.GORIGHT(2);>"+self.col+self.ch;
        self.ptr -= 1; return s;

    def CCOLUMN(self):
        s = f"$:CURSOR.JUMP({self.pos[0] + self.ptr}, {self.pos[1]});>";
        return s + self.col_i+self.ch+self.col+"$:CURSOR.GOLEFT(2);>";

    def FCOLUMN(self):
        s = f"$:CURSOR.XJUMP({self.pos[0]+self.ptr});>"+self.col+self.ch; self.ptr = 0;
        s = s + f"$:CURSOR.XJUMP({self.pos[0] + len(self.prompt)});>";
        return s + self.col_i+self.ch+self.col+"$:CURSOR.GOLEFT();>";

    def LCOLUMN(self):
        s = f"$:CURSOR.XJUMP({self.pos[0]+self.ptr});>"+self.col+self.ch;
        self.ptr = len(self.buff);
        s = s + f"$:CURSOR.XJUMP({self.pos[0] + len(self.prompt) + len(self.buff)});>";
        return s + self.col_i+self.ch+self.col+"$:CURSOR.GOLEFT();>";

    def PASTECHAR(self):

        x = KEYBOARD_CONTROLLER.LOG;
        if len(self.buff) < self.length:
            ch = chr(x)
            self.buff = self.buff[:self.ptr] + ch + self.buff[self.ptr:]; self.ptr += 1;
            s = self.col+ch;
            return s + self.col_i+self.ch+self.col+"$:CURSOR.GOLEFT();>";

        return "";

    def DELPREV(self):

        if self.ptr > 0:
            s = self.LEFT();

            old_length   = len(self.buff);
            self.buff    = self.buff[:self.ptr] + self.buff[self.ptr+1:] + " ";
            tail         = self.buff[self.ptr:];

            if self.ptr  == old_length:
                self.ptr -= 1;

            s = s + f"$:CURSOR.GOLEFT(2);>"+self.col+tail+f"$:CURSOR.GOLEFT({len(tail)});>";
            self.buff = self.buff[:-1];

            return s + self.col_i+self.ch+self.col+"$:CURSOR.GOLEFT();>"

        else: return self.DELCUR();

    def DELCUR(self):

        if len(self.buff) and self.ptr != len(self.buff):

            old_length   = len(self.buff);
            self.buff    = self.buff[:self.ptr] + self.buff[self.ptr+1:] + " ";
            tail         = self.buff[self.ptr:];

            if self.ptr  == old_length:
                self.ptr -= 1;

            s = f"$:CURSOR.GORIGHT();>"+self.col+tail+f"$:CURSOR.GOLEFT({len(tail)});>";
            self.buff = self.buff[:-1];

            return s + self.col_i+self.ch+self.col+"$:CURSOR.GOLEFT();>"

        return "";

    def END(self):
        self.state = 1; self.ptr = len(self.buff);
        return self.LINEDRAW() + self.LCOLUMN() + self.col+self.ch+"$:CURSOR.GOLEFT();>"

#   ---     ---     ---     ---     ---

class TOGGLEFIELD:

    def __init__(self, items, x=1, y=1, ptr=0, col="BACK", col_i="KVRSE", push="SEL1"):

        pad = 0;
        for s in items:
            length = len(str(s));
            if length > pad:
                pad = length;

        self.pad   = pad;

        self.pos   = (x, y);

        self.col   = PALETTE[col  ];
        self.col_i = PALETTE[col_i];
        self.push  = PALETTE[push ];

        self.items = items;
        self.ptr   = ptr;
        self.state = 0;
        self.spit  = 1;

        KVNSL_H.OUT(self.HIGHLIGHT());

    @property
    def CURRENT(self):
        return str(self.items[self.ptr]);

    def HIGHLIGHT(self, d=0):
        s     = str(self.CURRENT);
        pad   = " " * int((self.pad-len(s)));
        s     = s+pad;
        arr_s = self.push+"◄"+self.col_i if d == -1 else "◄"+self.col_i;
        arr_e = self.push+"►"+self.col   if d ==  1 else self.col+"►";

        return f"$:CURSOR.JUMP({self.pos[0]-1}, {self.pos[1]});> "+arr_s+s+arr_e;

    def CANCEL(self):
        self.ptr = -1; return self.END();

    def END(self):
        self.state = 1;
        if self.spit:
            s = str(self.CURRENT); pad = " " * int((self.pad-len(s)));
        else:
            s = " "*self.pad; pad="";

        return f"$:CURSOR.JUMP({self.pos[0]-1}, {self.pos[1]});> "+self.col+s+pad+"  ";

    def RIGHT(self):
        self.ptr = self.ptr+1 if self.ptr < (len(self.items)-1) else 0;
        return self.HIGHLIGHT(1);

    def LEFT(self):
        self.ptr = self.ptr-1 if self.ptr > 0 else (len(self.items)-1);
        return self.HIGHLIGHT(-1);

    def RUN(self):

        spec_cases =                        {

            "ARROWLEFT" : [self.LEFT,      ()],
            "ARROWRIGHT": [self.RIGHT,     ()],

                                            };

        cases =                             {

            "RETURN"    : [self.END,       ()],
            "ESCAPE"    : [self.CANCEL,    ()],
            "EXIT"      : [self.CANCEL,    ()],
            "ORDM"      : [self.CANCEL,    ()]

                                            };

        s = KEYBOARD_CONTROLLER.RUN(cases, spec_cases, False);
        if isinstance(s, int): s = "";
        return s;


#   ---     ---     ---     ---     ---

class SHUFFLER:

    def __init__(self, items, elems, idex, col="BACK", col_i="KVRSE"):

        self.elems  = elems;

        self.items  = items;
        self.idex   = idex;
        self.top    = len(items)-1;

        self.col    = PALETTE[col  ];
        self.col_i  = PALETTE[col_i];

        self.state  = 0;

        KVNSL_H.OUT(self.HIGHLIGHT());

    def HIGHLIGHT(self):

        s=""; i=0;
        for elem in self.elems:
            s = s+KVNSL_H.CUR_REGION.ITEM_ACCESS('deselect', i);
            elem['style'] = 3;
            s = s+KVNSL_H.CUR_REGION.ITEM_ACCESS('deselect', i);
            i+=1;

        s = s+KVNSL_H.CUR_REGION.ITEM_ACCESS('select', self.idex);
        return s;

    def SHUP(self):

        if self.idex != 0:
            i = self.idex;
            self.items[i-1], self.items[i] = self.items[i], self.items[i-1];

            labels = [self.elems[i]['label'], self.elems[i-1]['label']];
            length = len(labels[0]);
            if len(labels[1]) > length:
                length    = len(labels[1]);
                labels[0] = labels[0]+(" "*(length-len(labels[0])))
            else:
                labels[1] = labels[1]+(" "*(length-len(labels[1])))

            self.elems[i-1]['label'], self.elems[i]['label'] = labels;
            self.elems[i-1]['args'], self.elems[i]['args'] = (
                self.elems[i]['args'], self.elems[i-1]['args']
            )

            self.idex -=1; return KVNSL_H.RNAVIGATE_NI(REGION_ITEMS_PREV);

        return "";

    def SHDOWN(self):

        if self.idex != self.top:
            i = self.idex;
            self.items[i+1], self.items[i] = self.items[i], self.items[i+1];

            labels = [self.elems[i]['label'], self.elems[i+1]['label']];
            length = len(labels[0]);
            if len(labels[1]) > length:
                length    = len(labels[1]);
                labels[0] = labels[0]+(" "*(length-len(labels[0])))
            else:
                labels[1] = labels[1]+(" "*(length-len(labels[1])))

            self.elems[i+1]['label'], self.elems[i]['label'] = labels;
            self.elems[i+1]['args'], self.elems[i]['args'] = (
                self.elems[i]['args'], self.elems[i+1]['args']
            )

            self.idex +=1; return KVNSL_H.RNAVIGATE_NI(REGION_ITEMS_NEXT);

        return "";

    def RUN(self):

        spec_cases =                        {

            "ARROWUP"  : [self.SHUP,      ()],
            "ARROWDOWN": [self.SHDOWN,    ()],

                                            };

        cases =                             {

            "RETURN"    : [self.END,       ()],
            "ESCAPE"    : [self.END,       ()],
            "EXIT"      : [self.END,       ()],
            "ORDM"      : [self.END,       ()]

                                            };

        s = KEYBOARD_CONTROLLER.RUN(cases, spec_cases, False);
        if isinstance(s, int): s = "";
        return s;

    def END(self):

        s=""; i=0;
        for elem in self.elems:
            s = s+KVNSL_H.CUR_REGION.ITEM_ACCESS('deselect', i);
            elem['style'] = 2;
            s = s+KVNSL_H.CUR_REGION.ITEM_ACCESS('deselect', i);
            i+=1;

        s = s+KVNSL_H.CUR_REGION.ITEM_ACCESS('select', self.idex);

        self.state = 1; return s;

#   ---     ---     ---     ---     ---

class PSEUDOPOP:

    def __init__(self, call, args, valid=[0]):

        object.__setattr__(self, "state", 0);

        self.call  = call;
        self.args  = args;
        self.valid = valid;

    def __setattr__(self, attr, value):

        if attr == "state":
            self.call(value in self.valid, *self.args);

        else:
            object.__setattr__(self, attr, value);