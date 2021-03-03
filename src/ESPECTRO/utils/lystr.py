#   ---     ---     ---     ---     ---   #
#   lystr.py                              #
#                                         #
#   utils relating to regex and misc      #
#   string conversions that are too       #
#   ugly and verbose to be repeated       #
#                                         #
#   ---     ---     ---     ---     ---   #

import re;

forbid     = [ '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08',
               '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', '\x10', '\x11',
               '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A',
               '\x1B', '\x1C', '\x1D', '\x1E', '\x1F', '\x20', '\x21', '\x22', '\x23',
               '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2A', '\x2B', '\x2C',
               '\x2E', '\x2F', '\x3A', '\x3B', '\x3C', '\x3D', '\x3E', '\x3F', '\x40',
               '\x5B', '\x5C', '\x5D', '\x5E', '\x60', '\x7B', '\x7C', '\x7D', '\x7E',
               '\x7F', '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87',
               '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F', '\x90',
               '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\xFF',
               '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F', '\xA0', '\xA1',
               '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7', '\xA8', '\xA9', '\xAA',
               '\xAB', '\xAC', '\xAD', '\xAE', '\xAF', '\xB0', '\xB1', '\xB2', '\xB3',
               '\xB4', '\xB5', '\xB6', '\xB7', '\xB8', '\xB9', '\xBA', '\xBB', '\xBC',
               '\xBD', '\xBE', '\xBF', '\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5',
               '\xC6', '\xC7', '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE',
               '\xCF', '\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7',
               '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF', '\xE0',
               '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7', '\xE8', '\xE9',
               '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF', '\xF0', '\xF1', '\xF2',
               '\xF3', '\xF4', '\xF5', '\xF6', '\xF7', '\xF8', '\xF9', '\xFA', '\xFB',
               '\xFC', '\xFD', '\xFE',                                                 ];
forbid_reg = re.compile(("[^" + (''.join(forbid)) + "]"));

#   ---     ---     ---     ---     ---

def clnstr(s):
    return ''.join(forbid_reg.findall(s));

def clnpth(s):
    sp = "/" if "/" in s else "\\";
    sx = s.split(sp);

    for sent in sx:
        sent = clnstr(sent);

    return sp.join(sx);

def hxchr(n, width = 2):
    s    = (hex(n)[2:]).upper();
    fill = "0" * (width - (len(s)));
    return r'\x%s'%(fill+s);

#   ---     ---     ---     ---     ---