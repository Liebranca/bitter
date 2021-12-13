import sys,os;

argc=len(sys.argv);
argv=None;

#    ---     ---     ---     ---     ---

if(argc>=2):
  argv=sys.argv[1:];
  argc-=1;

else:
  print(
    "Usage: hex string\n\n"\
    "prints out string as a hex literal\n\n"

  );exit();

#    ---     ---     ---     ---     ---

def shx(n):
  return (
    chr(0x30+n) if n<0x0A
    else chr(0x41+(n-0x0A))

  );

#    ---     ---     ---     ---     ---

def nlng():
  l=[];l.extend(['0','x']);

  l.extend(['0' for _ in range(16)]);
  l.extend(['L','L']);
  l.extend([',','\n']);

  return list(l);

#    ---     ---     ---     ---     ---

s=[];cbyte=0;i=0;

for v in argv:

  for c in v:

    if(not cbyte):
      s.extend(nlng());
      cbyte=8;i=len(s)-5;

    o=ord(c);
    d0=o&0x0F;
    d1=(o&0xF0)>>4;

    s[i]=shx(d0);i-=1;
    s[i]=shx(d1);i-=1;

    cbyte-=1;

s=s[:-2];print(''.join(s));

#    ---     ---     ---     ---     ---