#   ---     ---     ---     ---     --- #
#   __knit.py                           #
#                                       #
#   a launcher for ESPECTRO             #
#                                       #
#   usage: just bring up cmd and        #
#   run python -i /path/__knit.py       #
#                                       #
#   youll need python37 (duh)           #
#                                       #
#   ---     ---     ---     ---     --- #

import os, sys, pathlib;
from msvcrt import getch, kbhit;

knitfold = str(pathlib.Path(__file__).parent.absolute());

knitdefault =                               [

    "PARENT_DIR     = 'D:\\lieb_git'",
    "KVR_DIR        = '\\KVR\\src'",

    "CC_DIR         = 'C:\\MINGW'",
    "CC_DIR64       = '\\x64\\mingw64'",
    "PLATFORM       = 'x64'",

    "READ_SIZE      =  16384",
    "FATAL_WARNINGS =  1",
    "DEBUG_MESSAGES = -1",
    "DISABLE_EPRINT =  0",
    "NEVER_ASK      =  0",
    "WALL           =  0"

                                            ];

knitell  = False;                           # tell user to go set their settings
SETTINGS = knitfold + "\\ESPECTRO.knit";    # this file holds all your shit so keep it tidy
if not os.path.exists(SETTINGS):

    knitell = True;
    with open(SETTINGS, 'w') as file:
        file.write("\n".join(knitdefault));

#   ---     ---     ---     ---     ---

if knitell:

    print("Settings file <%s> was just created.\n"%SETTINGS,
          "Be a sport and set up your paths & prefs there.\n", sep = '');

    print("Hit any key once you're done with that... \n");

    while(True):
        char = getch()
        if char: break;

#   ---     ---     ---     ---     ---

LSET = [];

with open(SETTINGS, 'r') as file:

    for line in file:

        com = line.replace("\n", "").rstrip("\\");
        if com.startswith ("//"    ): continue;

        elif com:
            exec(com);
            s = com.split("=")[1];
            if (  not (s.startswith("'"))
            and   not (s.endswith  ("'")) ):
                s = eval(s);

            else:
                s = s.replace("'", "");
                s = s.rstrip(" "); s = s.lstrip(" ");

            LSET.append(s);


if len(LSET) != len(knitdefault):

    os.system("cls");
    print("Well, it seems you've fucked with the settings file wrong\n",
          "Don't delete items. Don't add items. Just change the values.\n\n",

          "C-style comments ala '//blablabla' are okay. So are newlines.\n",
          "Anything else, don't fucking do it. Got it?\n\n",

          "I'm now restoring <%s> to defaults.\n"%SETTINGS,
          "Now get your settings straight and restart me.\n", sep = '');

    print("Press any key to continue... \n");

    while(True):
        char = getch()
        if char: break;

    with open(SETTINGS, 'w') as file:
        file.write("\n".join(knitdefault));

    os.kill(os.getpid(), 1);
    os._exit(1);

if PARENT_DIR+KVR_DIR not in sys.path: sys.path.append(PARENT_DIR+KVR_DIR);

from ESPECTRO import *;

#   ---     ---     ---     ---     ---

from time import sleep;

STARTUP(LSET);
PXUI(); KVNSL = GETKVNSL();
#KVNSL.OUT("DEBUG " * 20);

"""
ptr = 0; s = "►▼◄▲♦";
CPRINT("\x1b[23;0H\x1b[2K\x1b[?25lWait");


while True:

    if kbhit():
        CPRINT(" FUCK!", 1); break;

    if ptr < 3:
        CPRINT(f"\x1b[{5+ptr}G."); ptr += 1;
        CPRINT(f"\x1b[8G {s[ptr]}");

    elif ptr == 3:
        ptr += 1;

    else:
        CPRINT("\x1b[5G   \x1b[5G"); ptr = 0;
        CPRINT(f"\x1b[8G {s[ptr]}");

    sleep(1/10); CPRINT("", 1);

#STARTUP(LSET);
CPRINT("\x1b[f", 1); getch();
"""

"""
MULTICHOICE("BOX ONE", [str(x) for x in (0,1,2,3)], 1);
MULTICHOICE("BOX TWO", [str(x) for x in (0,1,2,3)], 2);
MULTICHOICE("BOX THREE", [str(x) for x in (0,1,2,3)], 3);

while(True):
    ret = INTPRT();
    if "EXIT" in ret: break;
"""

KVNSL.RUN();

CLEANUP();
os._exit(1);

