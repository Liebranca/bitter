#   ---     ---     ---     ---     --- #
#   lylist.py                           #
#                                       #
#   commops with lists                  #
#                                       #
#   ---     ---     ---     ---     --- #

from ESPECTRO import CATPATH;

def LFLAT(lol):

    ex = "";
    for i in range( len(lol) ):
        ex = ex + "lol[%i]"%i;
        if i < len(lol) - 1: ex = ex + " + ";

    return eval(ex);

def LKEYS(d, i):
    return list(d.keys())[i]

#   ---     ---     ---     ---     ---

class lyDICT:

    def __init__(self, items = {}):

        if items:
            self.items = items;

        else:
            self.items = {};

    def __contains__(self, item):

        if   type(item) != str: return item in list(self.items.values());
        else                  : return item in self.items;

        return False;

    def key(self, idex): return LKEYS(self.items, idex);

    def __iter__(self): return self.items.__iter__();

    def __len__(self): return len(self.items);

    def __getitem__(self, key):
        if   type(key) == int: return list(self.items.values())[key];
        elif type(key) == str: return self.items[key];

        return False;

    def __setitem__(self, key, item):
        if   type(key)  == int: key = list(self.items.keys())[key];
        self.items[key] = item;

    def __delitem__(self, key):
        if   type(key)  == int: key = list(self.items.keys())[key];
        del self.items[key];

    def keys  (self): return self.items.keys();
    def items (self): return self.items.items();
    def values(self): return self.items.values();

#   ---     ---     ---     ---     ---
