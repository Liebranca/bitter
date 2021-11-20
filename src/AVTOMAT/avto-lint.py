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

llb     = [');', '};', '];', '!=', '|=', '&=',
           '++', '--', '+=', '*=', '/=', '%=',
           '||', '&&', '^=', '~' , '!(','&=~',
           '->', '>=', '<=', '<<', '>>', ' ' ,
           '-=', 
           ',' , '(' , ')' , ';' , '{' , '}' ,
           '-' , '/' , '&' , '!' , '|' , '%' ,
           '+' , '^' , '>' , '<' , '=' , '*)'  ];

lbrk    = [' ' , ',' , '(' , ')' , ';' , '{' ,
           '}'                                 ];

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

def pickllb(s):
  c='';w='';l=len(s);i=0;
  llb_cpy=[];

  for c in s:

    if(i<l-2):
      w=c+s[i+1];

    else:
      w='';

    if(w in llb):
      if(w not in llb_cpy):
        llb_cpy.append(w);

    elif(c in llb):
      if(c not in llb_cpy):
        llb_cpy.append(c);

    i+=1;

  return llb_cpy;

#    ---     ---     ---     ---     ---

def sepcstr(s):
  pos=[];has=0;prev='';i=0;

  for c in s:
    if(c=='"' and prev!='\\'):
      if(has):
        pos.append(i+1);
        has=0;

      else:
        pos.append(i);
        has=1;

    i+=1;

  return pos;

#    ---     ---     ---     ---     ---

def despace(line):

  if(line.startswith('#')):
    return line;

  strseg=sepcstr(line);
  if(not len(strseg)):
    strseg=[len(line), len(line)];

  start=0;result="";
  for i in range(len(strseg)):
    end=strseg[i];l1=line[start:end];

#    ---     ---     ---     ---     ---

    llb_cpy=pickllb(line);
    for ch in llb_cpy:
      l2=l1.split(ch);
      ar='' if ch not in '{' else ' ';
      al='' if ch not in '}' else ' ';

      if(ch==' '):
        l2=[sub.strip() for sub in l2 if len(sub)];

      else:
        l2=[sub.strip() for sub in l2];

      ch=ar+ch+al;
      l1=ch.join(l2);

#    ---     ---     ---     ---     ---

    j=0;
    for ch in [')(', '){', '*)']:
      l2=l1.split(ch);
      l2=[sub for sub in l2];
      ch=ch[0]+' '+ch[1] if j<2 else ch+' ';
      l1=ch.join(l2);

      j+=1;

    if((i+1)<len(strseg)):
      start=strseg[i+1];

    else:
      start=len(line);

    result=result+l1+line[end:start];

  return result.rstrip();

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

#os.system("indent -nhnl -sob -di2 -bad -bbo -bap -bbb -br -brf -brs -d2 -ci0 -lp -ip0 -i2 -c0 -lc56 -cd0 -cp0 -bli0 -cli0 -nsaf -nsai -nsaw -npsl -nfca -nfc1 -ntac -nsc -ntac -nv -pal -nut -l56 avtomat/IDNT.c -o avtomat/IDNT.c");

#    ---     ---     ---     ---     ---

s     = "";
n_nl  = 0;

lines = [];
cs_nx = 0;
cs_lv = 0;
cs_sw = 0;

with open(root+"/IDNT.c", "r") as src:
  lines=src.readlines();

for line in lines:

  line=line.strip();

  if(not len(line)):
    s=s+('\n' if not n_nl else '');
    n_nl=1; continue;

  n_nl=0;

  if(line[0:8]=="//   ---" \

  or line[0:3]=="//*"      \

  or (line[0:2]=="//"      \
    and line[-1]=="*")     ):
    s=s+line+('\n' if line[-1]!='\n' else '');
    continue;

#    ---     ---     ---     ---     ---

  if(line[0:6]=="switch"):
    cs_sw=lvl+1;

  elif(lvl<=cs_sw and cs_sw):
    cs_sw=0; cs_nx=0;

  if(cs_nx):
    cs_lv=((line[0:5]!="case ")!=0)*(cs_sw!=0);

  lvl_dw = len([br for br in line if br=='}']);
  lvl_up = len([br for br in line if br=='{']);

  if(line[0:5]=="case "):
    cs_nx=1;

  if(lvl_up!=lvl_dw):
    lvl -= lvl_dw;

  indent = " "*(i_wid*lvl);

#    ---     ---     ---     ---     ---

  app_com="";
  if("//" in line):

    bits    = line.split("//");
    com     = "".join(b for b in bits[1:]);
    com     = com.rstrip('\n').rstrip(' ');
    com     = com.lstrip(' ');

    bits[0] = bits[0].lstrip(' ').rstrip('\n');
    bits[0] = bits[0].rstrip(' ');

    line    = despace(bits[0]);

#    ---     ---     ---     ---     ---

    com=com_fill(com); i=0;
    com=com.split('\n');

    for sub in com:
      sub=sub.lstrip(' ').rstrip(' ');
      com[i]=com[i].rstrip(' ');

      if(len(sub)):
        com[i]=(' '*l_mid)+sub;

      i+=1;

    com=[sub for sub in com if len(sub)];

#    ---     ---     ---     ---     ---

    if(not len(line)):      #:YESCOM;>
                            #:NOCODE;>
      s=s+('\n'.join(com)+'\n');
      continue;

    if(not len(com)):
      pass;

    elif((len(indent)+len(line)<=l_mid)):
      com[0]=com[0].lstrip(' ');
      app_com=('\n'.join(com)+'\n');

      line=cfa_fill(line);
      line=line[:l_mid-len(indent)];

    else:                   #:FULINE;>
      s=s+('\n'.join(com)+'\n');

#    ---     ---     ---     ---     ---

  else:                     #:NOCOM;>

    line   = cde_fill(despace(line));
    line   = line[:-1].rstrip(' ')+'\n';

#    ---     ---     ---     ---     ---

  if(len(line.strip())<=1):
    if(not len(line) and line not in llb):
      continue;

#    ---     ---     ---     ---     ---

  line=(indent+line+app_com);

  if(lvl_up!=lvl_dw):
    lvl += lvl_up;

  if(line[-1]!='\n'):
    line=line+'\n';

  elif(line==indent+'\n'\
  or   line==indent     \
  or   line=='\n'       ):
    line='';

  s=s+line;

#    ---     ---     ---     ---     ---

with open(root+"/IDNT.c", "w+") as dst:
  dst.write(s);

#print(s);