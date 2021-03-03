#   ---     ---     ---     ---     --- #
#   KVR.py                              #
#                                       #
#   language syntax definitions         #
#   interpreter for user input          #
#                                       #
#   ---     ---     ---     ---     --- #

CMD =                                       {

    "NEW": MKOB

                                            };

def ITRPT(s):

    TKN = s.split(" ");

    COM = TKN[0 ];
    ARG = TKN[1:];

    return CMD[COM](*ARG);

#   ---     ---     ---     ---     ---