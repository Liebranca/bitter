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

def mkline(s,indent,pad):
  result="";i=0;subs=s.split('\n');
  for sub in subs:

    nl='\n' if i<(len(subs)-1) else '';

    if(i):
      result=result+indent+pad+sub+nl;

    else:
      result=result+indent+sub+nl;

    i+=1;

  return result;

def bwfllb(s,fr):
  c='';w='';
  for i in range(len(s)-(len(s)-fr),0,-1):

    if(i+1>len(s)-1):
      continue;

    c=s[i+1];
    w=s[i]+c;

    if(w in llb):
      return (s[:i],s[i:]);

    elif(c in llb):
      return (s[:i+1],s[i+1:]);

  return 0;

#    ---     ---     ---     ---     ---

def __fillstrg(no_goto):
  bx,l_start,pad,space,s,pos,base=no_goto;

  sub_cnt=1+int(len(s)/(space-1));
  sub_pos=0;

  bx.extend(c for c in base);
  pos=l_start;pad+=len(base);

  spad=pad;

#    ---     ---     ---     ---     ---

  for i in range(sub_cnt):

    sub_end=((space-1) if len(s[sub_pos:])>=space
                       else len(s[sub_pos:]));

    sub=(

      ('"' if i else '')
      +s[sub_pos:sub_pos+sub_end]
      +('"' if sub_end!=len(s[sub_pos:]) else '')

    );

    if(sub=='"'):
      break;

#    ---     ---     ---     ---     ---

    sub_pos+=sub_end;
    bx[pos+pad:pad+space]=sub;

    pad+=len(base);

    if(sub_end==(space-1) and i<(sub_cnt-1)):
      if(len(s[sub_pos:])<1):
        break;

      bx.extend(c for c in base);

  return [bx,l_start,pad,space,s,pos,base];

#    ---     ---     ---     ---     ---

def __fillcode(no_goto):
  bx,l_start,pad,space,s,pos,base=no_goto;

  for sub in s.split(' '):

    if(pos==l_start):
      if(not len(sub)):
        continue;

      bx.extend(c for c in base);

#    ---     ---     ---     ---     ---

    if(len(sub)>=(space-pos)):

      if(len(sub)>=(space)):
        while(len(sub)>=(space)):

          sub,nsub=bwfllb(sub,space-(pos-l_start));

          bx[pos+pad:space+pad]=(
            sub[:space-(pos-l_start)]

          );sub = nsub;

          pos   = l_start;
          pad  += len(base);

          bx.extend(c for c in base);

        if(len(sub)):
          bx[pos+pad:pos+pad+len(sub)]=(
            sub[:]

          );pos+=len(sub);sub='';

#    ---     ---     ---     ---     ---

      else:
        bx.extend(c for c in base);
        pad += len(base);
        bx[l_start+pad:l_start+pad+len(sub)]=sub[:];

        pos  = l_start+len(sub);

    else:
      bx[pos+pad:pos+pad+len(sub)]=sub[:];
      pos+=len(sub);

#    ---     ---     ---     ---     ---

    if(pos==space):
      pos=l_start;pad+=len(base);

    elif(pos>l_start and bx[pos+pad-1]!=' '
    and  pos+pad<len(bx)):
      bx[pos+pad]=' ';pos+=1;

  return [bx,l_start,pad,space,s,pos,base];

#    ---     ---     ---     ---     ---

def __fill(s, base,pos=3,cap=4,trim=0):

  if(not len(s)): return '';

  bx      = [];

  l_start = pos;
  pad     = 0;

  space   = len(base)-cap-l_start;
  s       = s.strip();

  no_goto = [bx,l_start,pad,space,s,pos,base];

#    ---     ---     ---     ---     ---

  strseg=sepcstr(s);
  hasstr=(
    len(strseg)
    *(not (base==com_fld or base==lcm_fld))
    *(s[0]!='#')

  );

  if(not hasstr or len(s)<space):
    no_goto=__fillcode(no_goto);

#    ---     ---     ---     ---     ---

  else:
    beg_cde,end_cde=0,0;
    beg_str,end_str=0,0;

    for i in range(0,len(strseg),2):
      beg_str=strseg[i]

      end_str=strseg[i+1];
      end_cde=beg_str;

      cde_seg=s[beg_cde:end_cde];
      if(len(cde_seg)):
        no_goto[4]=cde_seg;
        no_goto=__fillcode(no_goto);

      no_goto[4]=s[beg_str:end_str];
      no_goto=__fillstrg(no_goto);


      beg_cde=end_str;

#    ---     ---     ---     ---     ---

    end_cde=len(s);
    cde_seg=s[beg_cde:end_cde];

    if(len(cde_seg)):
      no_goto[4]=cde_seg;
      no_goto=__fillcode(no_goto);

    bx,l_start,pad,space,s,pos,base=no_goto;

#    ---     ---     ---     ---     ---

  st = ''.join(c for c in bx);

  if(trim):
    st=(
      '\n'.join(sub.strip()
      for sub in st.split('\n'))

    );

  return st;

#    ---     ---     ---     ---     ---

def bx_fill(s):
  return __fill(s, bx_fld,cap=1);

def com_fill(s):
  return __fill(s, com_fld);

def lcm_fill(s):
  return __fill(s, lcm_fld);

def cde_fill(s,cap=4):
  return __fill(s, cde_fld, 0, cap=cap, trim=1);

def cfa_fill(s,cap=1):
  return __fill(s,cde_fa,0,cap=cap);

def cfb_fill(s):
  return __fill(s, cde_fb,  0);

#    ---     ---     ---     ---     ---

def pickllb(s):
  c='';w='';l=len(s);i=0;
  hp=[];lp=[];

  for c in s:

    if(i<l-1):
      w=c+s[i+1];

    else:
      w='';

    if(w in llb):
      if(w not in hp):
        hp.append(w);

    elif(c in llb):
      if(c not in lp):
        lp.append(c);

    i+=1;

  return hp+lp;

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

def llbsplit(l1):

  llb_cpy=pickllb(l1);
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

  return l1;

#    ---     ---     ---     ---     ---

def despace(line):

  hasstr=0;
  if(line.startswith('#')):
    return line;

  strseg=sepcstr(line);
  if(not len(strseg)):
    strseg=[len(line)];

  else:
    hasstr=1;

#    ---     ---     ---     ---     ---

  start=0;result="";
  for i in range(len(strseg)):
    end=strseg[i];l1=llbsplit(line[start:end]);

    if((i+1)<len(strseg)):
      start=strseg[i+1];

    else:
      start=len(line);

    sub=line[end:start];
    if(len(sub) and sub[0]!='"'):
      result=result+l1+llbsplit(sub);

    else:
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
    cs_sw=lvl;

  elif(line[0:5]=="case "   \
  or   line[0:8]=='default '):
    cs_nx=0;

  elif(cs_sw):
    cs_nx=lvl<=cs_sw;

  curly_beg='}' in line[0:2];

  lvl_dw = len([br for br in line if br=='}']);
  lvl_up = len([br for br in line if br=='{']);

  if( (lvl_up!=lvl_dw) \
  and (curly_beg)      ):
    lvl -= lvl_dw;

  indent = ' '*(i_wid*(lvl+cs_nx));

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

      line=cfa_fill(line,cap=1);
      line=line[:l_mid-len(indent)];

    else:                   #:FULINE;>
      s=s+('\n'.join(com)+'\n');

#    ---     ---     ---     ---     ---

  else:                     #:NOCOM;>

    line   = cde_fill(despace(line),cap=5+len(indent));
    line   = line[:-1].rstrip(' ')+'\n';

#    ---     ---     ---     ---     ---

  if(len(line.strip())<=1):
    if(not len(line) and line not in llb):
      continue;

#    ---     ---     ---     ---     ---

  curly_end='';

  if(lvl_up!=lvl_dw):
    lvl += lvl_up;

    if(not curly_beg):
      lvl -= lvl_dw;

#    ---     ---     ---     ---     ---

      frun='\n';
      while('}' in line[-3:]):
        if(lvl_up): break;

        if(line[-3:-1]=='};'):
          curly_end=curly_end+'};';
          line=line[:-3]+'\n'+frun;
          frun='';

        elif(line[-2]=='}'):
          curly_end=curly_end+'}';
          line=line[:-2]+'\n'+frun;
          frun='';

        else:
          break;

#    ---     ---     ---     ---     ---

  if(cs_sw and (  line[0:5]=="case "   \
               or line[0:8]=='default ')):

    cs_nx=-1;

  if(lvl<=cs_sw and cs_sw):
    cs_sw=0;cs_nx=0;

  line=mkline(              #:MKLINE;>
    line,indent,
    (' '*i_wid)

  )+app_com;

  line=line.rstrip();

  if(line[-1]!='\n'):
    line=line+'\n';

  elif(line==indent+'\n'\
  or   line==indent     \
  or   line=='\n'       ):
    line='';

  s=s+line+curly_end;

#    ---     ---     ---     ---     ---

with open(root+"/IDNT.c", "w+") as dst:
  dst.write(s);

#print(s);