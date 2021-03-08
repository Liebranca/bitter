#    ---     ---     ---     ---     ---    #
#   OBMANG.PY                               #
#                                           #
#   central manager for KVR data-blocks     #
#                                           #
#    ---     ---     ---     ---     ---    #

from ESPECTRO import hxPX, LKEYS;

#    ---     ---     ---     ---     ---

DTYPES =                                    {

    "PX": hxPX

                                            };

class hxMANG:

    def __init__(self):
        self.dt  = { key:{} for key in DTYPES };
        self.sel = None;

    def insert(self, key, name, block):
        self.sel = self.dt[key][name] = block;

#    ---     ---     ---     ---     ---

MANG = hxMANG();

def LDBLOCK(cls_key, block_name):
    block = getattr(DTYPES[cls_key], "load")(block_name);
    MANG.insert(cls_key, block_name, block);

def GTBLOCK(cls_key=None, block_name=None):
    if not cls_key:
        return MANG.sel;

    elif not block_name and MANG.dt[cls_key]:
        return MANG.dt[cls_key][LKEYS(MANG.dt[cls_key], 0)];

    return MANG.dt[cls_key][block_name];

