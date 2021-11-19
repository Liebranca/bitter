#--------------------------o
# AVTO-LINT                |
# formats my shit          |
#--------------------------o
# LIBRE SOFTWARE           |
# Licensed under GNU GPL3  |
# be a bro and inherit     |
#--------------------------o
# CONTRIBUTORS             |
# lyeb,(=)                 |
#--------------------------o

root    = "/".join((__file__.split("/"))[:-1]);

lvl     = 0;
i_wid   = 2;
l_wid   = 56;
l_mid   = 28;
c_len   = (l_wid-l_mid)-3;

llb     = [' ', ',', '(', ';' ];

bx_cap  = "//"+('*'*(l_mid-2))+"\n";
bx_fld  = "//"+(' '*(l_mid-3))+"*\n";
com_fld = "// "+(' '*c_len)+'\n';
lcm_fld = "// "+(' '*l_wid)+'\n';
cde_fld = " "+(' '*(l_wid))+'\n';

cde_fa  = cde_fld[:(l_mid)];
cde_fb  = cde_fld[(l_mid):];

#    ---     ---     ---     ---     ---

def __fill(s, base, pos=3):

  ned     = 1+(int(len(s)/len(base)));
  bx      = [];

  l_start = pos;
  pad     = 0;

  space   = len(base)-pos;

  for c in s.lstrip().rstrip():

    if(pos==l_start):
      if(c==' ' or c=='\n'):
        continue;

      bx.extend(c for c in base);

    bx[pos+pad]=c; pos+=1;
    if(pos==space):
      pos=l_start; pad+=len(base);

  return ''.join(c for c in bx);

#    ---     ---     ---     ---     ---

def bx_fill(s):
  return __fill(s, bx_fld);

def com_fill(s):
  return __fill(s, com_fld);

def lcm_fill(s):
  return __fill(s, lcm_fld);

def cde_fill(s):
  return __fill(s, cde_fld, 0);

def cfa_fill(s):
  return __fill(s, cde_fa,  0);

def cfb_fill(s):
  return __fill(s, cde_fb,  0);

#    ---     ---     ---     ---     ---

def docbox(fname):

  excp  = [
    ' ', '_', '(', ')',
    '[', ']', '$', '#',
    '&', ',', '%', '@'

  ];

  mname = "";
  desc  = ""; desc_done = 0;
  lic   = []; lic_done  = 0;
  cont  = []; cont_done = 0;

#    ---     ---     ---     ---     ---

  atl=0;
  with open(root+"/"+fname, "r") as src:
    while(1):

      line=src.readline();
  
      if(not len(line.rstrip().lstrip())):
        break;

      atl+=1;

      if("CONTRIBUTORS" in line):
        lic_done=1; continue;

      line=''.join(ch for ch in line \
                   if ch.isalnum() or ch in excp);

      line=line.lstrip(' ').rstrip(' ');

#    ---     ---     ---     ---     ---

      if(len(line)):
        if(not len(mname)):
          mname=line;

        elif(not desc_done):
          desc=desc+line;

        elif(not lic_done):
          lic.append(line);

        else:
          cont.append(line);

      else:
        if(len(mname) and len(desc) and not desc_done):
          desc_done=1;

#    ---     ---     ---     ---     ---

  s="";

  s=s+bx_cap;
  s=s+bx_fill(mname);
  s=s+bx_fill(desc);
  s=s+bx_fld;

  for l in lic:
    s=s+bx_fill(l);

  s=s+bx_fld;

  s=s+bx_fill("CONTRIBUTORS");
  s=s+bx_fill(''.join(c for c in cont));
  s=s+bx_cap+"\n";

  lines=[];
  with open(root+"/"+fname, "r") as src:
    lines=src.readlines();

  final=s+''.join(lines[atl:]);
  with open(root+"/IDNT.c", "w+") as dst:
    dst.write(final);

#    ---     ---     ---     ---     ---

docbox("idntest.c");
import os;

#os.system("indent -nhnl -sob -di2 -bls -bad -bbo -bap -bbb -d2 -ci0 -lp -ip0 -i2 -c0 -lc56 -cd0 -cp0 -bli0 -cli0 -nsaf -nsai -nsaw -npsl -nfca -nfc1 -ntac -nsc -ntac -nv -pal -nut -l56 avtomat/IDNT.c -o avtomat/IDNT.c");

#    ---     ---     ---     ---     ---

s     = "";
n_nl  = 0;

lines = [];

with open(root+"/IDNT.c", "r") as src:
  lines=src.readlines();

for line in lines:

  line=line.lstrip(' ').rstrip('\n').rstrip(' ');

  if(not len(line)):
    s=s+'\n';
    continue;

  if(line[0:2]=="//"):
    s=s+line+('\n' if line[-1]!='\n' else '');
    n_nl=1; continue;

  lvl_dw = len([br for br in line if br=='}']);
  lvl_up = len([br for br in line if br=='{']);

  if(lvl_up!=lvl_dw):
    lvl -= lvl_dw;

  indent = " "*(i_wid*lvl);

#    ---     ---     ---     ---     ---

  app_com="";
  if("//" in line):
    bits  = line.split("//");
    com   = "".join(b for b in bits[1:]);
    com   = com.rstrip('\n').rstrip(' ');
    com   = com.lstrip(' ');

#    ---     ---     ---     ---     ---

    if(not len(bits[0])):   #:YESCOM;>
      s=s+com_fill(com);    #:NOCODE;>
      continue;

    bits[0]=bits[0].lstrip(' ').rstrip('\n');
    bits[0]=bits[0].rstrip(' ');

    com=com_fill(com); i=0;
    com=com.split('\n');

    for sub in com:
      sub=sub.lstrip(' ').rstrip(' ');
      com[i]=com[i].rstrip(' ');

      if(len(sub)):
        com[i]=(' '*l_mid)+sub;

      i+=1;

    com=[sub for sub in com if len(sub)];

    if(not len(com)):
      line=bits[0];

    elif((len(indent)+len(bits[0])<=l_mid)):
      com[0]=com[0].lstrip(' ');
      app_com=('\n'.join(com)+'\n');

      line=line[:l_mid-len(indent)];
      line=cfa_fill(bits[0]);
      if("stack" in app_com):
        print(line+app_com);

    else:                   #:FULINE;>
      s=s+('\n'.join(com)+'\n');
      line=bits[0];

#    ---     ---     ---     ---     ---

  else:                     #:NOCOM;>
    line   = cde_fill(line);
    line   = line[:-1].rstrip(' ')+'\n';

#    ---     ---     ---     ---     ---

  if(not len(line)):
    continue;

  if(n_nl):
    indent = '\n'+indent; n_nl=0;

  line     = (indent+line+app_com);

#    ---     ---     ---     ---     ---

  #if(len(line)>l_wid):
  #  for pos in range(l_wid,0,-1):
  #    if(line[pos] in llb):
  #
  #      if(line[pos]==' '):
  #        line = line[:pos]+'\n' \
  #             + indent+(line[pos:].lstrip(' '));
  #
  #      else:
  #        line = line[:pos]+'\n'+indent+line[pos:];
  #
  #      break;

#    ---     ---     ---     ---     ---

  if(lvl_up!=lvl_dw):
    lvl += lvl_up;

  s=s+line;

#    ---     ---     ---     ---     ---

with open(root+"/IDNT.c", "w+") as dst:
  dst.write(s);

#print(s);

#os.system("indent -hnl -sob -di2 -bls -bad -bbo -bap -bbb -d2 -ci0 -lp -ip0 -i2 -c0 -lc56 -cd0 -cp0 -bli0 -cli0 -nsaf -nsai -nsaw -npsl -nfca -nfc1 -ntac -nsc -ntac -nv -pal -nut -l56 avtomat/idntd.c");