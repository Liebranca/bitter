#   ---     ---     ---     ---     ---   #
#   console.py                            #
#                                         #
#   convenience functions for totally     #
#   pimping out the kvnslrat experience   #
#                                         #
#   ---     ---     ---     ---     ---   #

from   ESPECTRO import DOS, CLS;
from   msvcrt   import getch;

#   ---     ---     ---     ---     ---

def CHOICE(mess, choices = ["Y", "N"], repeat = 0):

    choices = [ch.upper() for ch in choices];
    chs     = "\\".join(choices);

    chb     = [bytes(ch, 'ascii') for ch in choices];

    if not repeat:
        print(mess + " [" + chs + "]");

    c   = getch().upper();
    if c in chb:
        return 1 + chb.index(c);

    else:
        return CHOICE(mess, choices, 1);

alpha = [ bytes(chr(x), 'ascii') for x in range(65, 91) ];
def MULTICHOICE(mess, items, repeat = 0):

    if not repeat:

        print(mess); i = 0;
        for item in items:

            print("%s) %s"%(alpha[i].decode('ascii'), item));
            i += 1;

    c = getch().upper();
    if c in alpha:
        return 1 + alpha.index(c);

    else:
        return MULTICHOICE(mess, items, 1);

#   ---     ---     ---     ---     ---
