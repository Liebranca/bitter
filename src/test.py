import os,sys;

if("d:/lieb_git/kvr/src" not in sys.path):
    sys.path.append("d:/lieb_git/kvr/src");

from ESPECTRO.PYZJC import *;
test=GTCLIB("d:/lieb_git/kvr","test.dll");
_sayhi=wrap_cfunc(test,"sayhi",voidstar,[voidstar]);
def sayhi(ptr):
  _sayhi(mcstr(ptr));
  
sayhi("Hello there!");

