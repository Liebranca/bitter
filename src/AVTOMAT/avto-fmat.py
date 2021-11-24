#--------------------------o
# AVTO-FMAT                |
# formats my shit          |
#--------------------------o
# LIBRE SOFTWARE           |
# Licensed under GNU GPL3  |
# be a bro and inherit     |
#--------------------------o
# CONTRIBUTORS             |
# lyeb,(=)                 |
#--------------------------o

# test that the formated file compiles!
# gcc -I/cygdrive/d/lieb_git/kvr/src -I/cygdrive/d/lieb_git/kvr/src/mammoth -DKVR_DEBUG=1 -c avtomat/idnt.c -L/cygdrive/d/lieb_git/kvr/bin/x64 -lkvrnel -o avtomat/a.o


#    ---     ---     ---     ---     ---

ff_write   = 0x01;
ff_newline = 0x02;
ff_indent  = 0x04;

ff_lcomm   = 0x08;
ff_bcomm   = 0x10;
ff_prepo   = 0x20;
ff_label   = 0x40;
ff_wsig    = 0x80;

#    ---     ---     ---     ---     ---

class dtdde:

  lvl   = 0;
  state = 0x00;

  i_wid = 2;
  l_wid = 56;
  l_mid = 28;
  c_len = (l_wid-l_mid)-3;

  rgi   = ' '*i_wid;

  chops = [
    ',','-','+','|','&',
    '/','*','%','^','<',
    '>','='

  ];

#    ---     ---     ---     ---     ---

  @classmethod
  def gi(self):
    return (
      self.rgi
    *(self.lvl+((self.state&ff_label)!=0))

    );

#    ---     ---     ---     ---     ---

  @classmethod
  def detchain(self,s):

    i=0;
    for c in s:

      nx='';
      if(i<(len(s)-1)):
        nx=s[i+1];

      if( c in self.chops
      and nx.isalnum()):
        return 1;

      if(c==')'):
        break;

      i+=1;

    return 0;

  @classmethod
  def detlabel(self,s,idex):

    llvl=self.lvl;
    for sub in s[idex:].split(' '):

      if(sub=='\n' or not len(sub)):
        idex+=1;continue;

      elif( (':'       in sub)
      or    ("case"    in sub)
      or    ("default" in sub) ): break;

      elif('{' in sub or '}' in sub):
        llvl+=len([br for br in sub if br=='{']);
        llvl-=len([br for br in sub if br=='}']);

      if(llvl<self.lvl):
        break;

      idex+=len(sub)+1;

    return idex;

#    ---     ---     ---     ---     ---

  @classmethod
  def opnparn(self,seg):

    parens=0;i=0;
    lsplit=0;do_split=0;opch=0;

    seg_cpy=str(seg);

    for c in seg_cpy:
      if(c in ' \n\t'):
        if(c=='\n'):
          lsplit=i+1;
          do_split=(
            len(seg[lsplit:])
            >self.l_mid
          );

        i+=1;continue;
      
      if(c=='('):
        parens=1;

      elif(c==')'):
        parens=0;

      if(parens):
        continue;

#    ---     ---     ---     ---     ---

      if(c in self.chops and do_split):

        if(c==','):
          pad=1+len(self.rgi)+opch;
          seg=(
            seg[:i+1]+'\n'
           +('\n'*opch)
           +self.gi()
           +self.rgi
           +seg[i+1:]

          );opch=0;

#    ---     ---     ---     ---     ---

        else:
          pad=2;
          opch=1;
          seg=(
            seg[:i]+'\n'
           +self.gi()
           +' '
           +seg[i:]

          );

        i+=pad+(self.i_wid*self.lvl);

#    ---     ---     ---     ---     ---

      i+=1;

    return seg;

#    ---     ---     ---     ---     ---

  @classmethod
  def accom(self,row,chain):

    if(not len(row)):
      return '';

    if(len(row)>self.l_mid):
      pass;

    if(chain):
      row=self.opnparn(row);

    return row;

#    ---     ---     ---     ---     ---

  @classmethod
  def iatst(self,row):
    if(row==('\n'*len(row))):
      return 0;

    _gi=self.gi(); return (
      not row.startswith(_gi)
      and not row[1:].startswith(_gi)

    );

#    ---     ---     ---     ---     ---

  @classmethod
  def format(self,s):

    self.state=ff_wsig;

    last   = '';nx    = '' ;i   =0 ;
    result = "";indent  = "" ;row ="";
    chain  = 0 ;clchain = 0  ;

    last_nl= 0 ;tok     = "_";tokb="";
    lblspan= 0 ;labl="";

#    ---     ---     ---     ---     ---

    for c in s:

      nx='';
      if(i<(len(s)-1)):
        nx=s[i+1];

      if((self.state&ff_wsig) and c!=' '):
        row=row+c;self.state&=~ff_wsig;

      elif((self.state&ff_wsig)==0):
        row=row+c;

#    ---     ---     ---     ---     ---

      if(c==';'):
        self.state|=ff_write;

      elif(c==':'):
        labl=row;
        self.state|=ff_label|ff_indent;
        lblspan=self.detlabel(s,i);

#    ---     ---     ---     ---     ---

      elif(c==' '):
        igws=not self.state&(
          ff_lcomm
         |ff_bcomm
         |ff_prepo

        );

        if(igws):
          if(row[-2]=='\n'):
            row=row[:-1];

      elif(c=='#'):
        if(not last_nl):
          row=row[:-1]+'\n'+c;

        self.state|=ff_prepo;

#    ---     ---     ---     ---     ---

      elif(c=='\n'):

        self.state|=(
          ff_write
          |(ff_indent
           *(not self.iatst(row))
           *(not (self.state&ff_lcomm))
          )
          |ff_newline

        );row=row[:-1];

        self.state&=~ff_lcomm;
        self.state&=~(
          ff_prepo*(last!='\\')

        );

#    ---     ---     ---     ---     ---

      elif(c+nx=='//'):
        self.state|=ff_lcomm;
        if(not last_nl and not len(row)):
          row=row[:-1]+'\n'+c;

      elif(c+nx=='/*'):
        self.state|=ff_bcomm;

      elif(c+nx=='*/'):
        self.state&=~ff_bcomm;

#    ---     ---     ---     ---     ---

      elif(c in '{('):

        if(c=='('):
          chain+=self.detchain(s[i:]);

        if(chain or c=='{'):
          self.lvl+=1;
          self.state|=(
            ff_write
           |ff_indent
           |(ff_newline*(nx!='\n'))

          );

#    ---     ---     ---     ---     ---

      elif(c=='}' or (c==')' and chain)):
        clchain=chain!=0;
        self.lvl-=1;chain=chain-(c==')');

        row=(
          row[:-1].rstrip(' ')
         +('\n'*(chain==0)*(not last_nl))
         +('\n'*(last!=c)*(not last_nl))
         +self.gi()+c

        );self.state|=ff_write;

      elif(i==len(s)-1):
        self.state|=ff_write;

#    ---     ---     ---     ---     ---

      if(self.state&ff_write):

        if(lblspan and i>=lblspan):
          lblspan=0;

        result=result+self.accom(row,clchain);
        row=(

          ('\n'*((self.state&ff_newline)!=0))

         +(self.gi()*((self.state&ff_indent)!=0))

        );last_nl=self.state&ff_newline;

        self.state&=~(
          ff_write
         |ff_indent
         |ff_newline
         |ff_label*(lblspan==0)

        );self.state|=ff_wsig;
        clchain=0;

#    ---     ---     ---     ---     ---

      if(ord(c)>0x20):
        last=c;
        if(c.isalnum() or c==':'):
          tokb=tokb+c;

      else:
        tok=tokb if tokb else "_";tokb="";

#    ---     ---     ---     ---     ---

      #if(tok!='_'):
      #  if(tok[-1]==':'):
      #    self.state|=ff_label|ff_indent;
      #
      #  tok='_';

      i+=1;
    print(result);

#    ---     ---     ---     ---     ---

root    = "/".join((__file__.split("/"))[:-1]);

dtdde.format(open(root+"/fmat_test.c").read());

exit();

lvl     = 0;
llb     = [');', '};', '];', '!=', '|=', '&=',
           '++', '--', '+=', '*=', '/=', '%=',
           '||', '&&', '^=', '~' , '!(','&=~',
           '->', '>=', '<=', '<<', '>>', ' ' ,
           '-=', 
           ',' , '(' , ')' , ';' , '{' , '}' ,
           '-' , '/' , '&' , '!' , '|' , '%' ,
           '+' , '^' , '>' , '<' , '=' , '*)'  ];

lbrk    = [');', '};', '];', '!=', '|=', '&=',
           '++', '--', '+=', '*=', '/=', '%=',
           '||', '&&', '^=', '~' , '!(','&=~',
           '>=', '<=', '<<', '>>', ' ' ,
           '-=', 
           ',' , '(' , ')' , ';' , '{' , '}' ,
           '/' , '&' , '!' , '|' , '%' ,
           '+' , '^' , '=' , '*)'              ];

bx_cap  = "//"+('*'*(l_mid-2))+"\n";
bx_fld  = "//"+(' '*(l_mid-3))+"*\n";
com_fld = "// "+(' '*c_len)+'\n';
lcm_fld = "// "+(' '*l_wid)+'\n';
cde_fld = " "+(' '*(l_wid))+'\n';

cde_fa  = cde_fld[:(l_mid)];
cde_fb  = cde_fld[(l_mid):];

#    ---     ---     ---     ---     ---

class coldde:

  cln = 1;
  clw = 4;

#    ---     ---     ---     ---     ---

  @staticmethod
  def test(line,seps,pad):

    col_i  = 0;
    row_i  = 0;
    result = "";
    row    = "";

#    ---     ---     ---     ---     ---

    for c in line:

      row=row+c;
      col_i=int(len(row)/coldde.clw);

      if( col_i==coldde.cln
      and c in seps):
        result=result+(pad*seps[c])+row+'\n';
        col_i=0;row_i+=1;row="";

#    ---     ---     ---     ---     ---

    for c in row:
      if(c in seps):
        result=result+(pad*seps[c])+row+'\n';

    return result;

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
  c='';w='';r=(s,"");
  for i in range(len(s)-(len(s)-fr),0,-1):

    if(i+1>len(s)-1):
      continue;

    c=s[i+1];
    w=s[i]+c;

    if(w in lbrk):
      if(w in ('};',');','];')):
        r=(s[:i+2],s[i+2:]);

      else:
        r=(s[:i],s[i:]);

    elif(c in lbrk):
      if(c in "{[(,)]};"):
        r=(s[:i+2],s[i+2:]);

      else:
        r=(s[:i+1],s[i+1:]);

    if(len(r[0])<=fr/2):
      break;

  return r;

#    ---     ---     ---     ---     ---

no_goto=[[],0,0,0,0,0,0,0];

def __fillstrg():

  global no_goto;
  bx,l_start,pad,space,s,pos,base=no_goto;
  sub_pos=0;

  if(pos>=space-2):
    pos=l_start;pad+=len(base);

#    ---     ---     ---     ---     ---

  while(sub_pos<len(s)):

    if(pos==l_start):
      bx.extend(c for c in base);

    sub_len=min(
      space-pos-2,
      len(s[sub_pos:])

    );

    if(sub_len<=0):
      pos=l_start;pad+=len(base);
      continue;

#    ---     ---     ---     ---     ---

    sub=s[sub_pos:sub_pos+sub_len];
    sub_pos+=sub_len;

    if(not len(sub)):
      break;

    if(sub[0]!='"'):
      sub='"'+sub;

    if(sub[-1]!='"'):
      sub=sub+'"';

#    ---     ---     ---     ---     ---

    bx[pos+pad:pad+pos+len(sub)]=sub;
    pos+=len(sub);

    if(pos==space-2):
      pos=l_start;pad+=len(base);

  no_goto[2]=pad;
  no_goto[5]=pos;

#    ---     ---     ---     ---     ---

def __fillcode():

  global no_goto;
  bx,l_start,pad,space,s,pos,base=no_goto;

  if(pos>=space):
    pos=l_start;pad+=len(base);

  for sub in s.split(' '):

    if(pos==l_start):
      if(not len(sub)):
        continue;

      bx.extend(c for c in base);

#    ---     ---     ---     ---     ---

    if(len(sub)>=(space-pos)):

      if(len(sub)>=(space)):
        while(len(sub)>=(space)):

          sub,nsub=bwfllb(sub,space-pos);
          if(len(sub)>space-pos):
            nsub=sub[space-pos:]+nsub;
            sub=sub[:space-pos];

          bx[pos+pad:pos+pad+len(sub)]=(sub

          );sub = nsub;

          pos   = l_start;
          pad  += len(base);

          bx.extend(c for c in base);

#    ---     ---     ---     ---     ---

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

  no_goto[2]=pad;
  no_goto[5]=pos;

#    ---     ---     ---     ---     ---

def __fill(s, base,pos=3,cap=4,trim=0):

  if(not len(s)): return '';

  bx      = [];

  l_start = pos;
  pad     = 0;

  space   = len(base)-cap-l_start;
  s       = s.strip();

  global no_goto;
  no_goto = [bx,l_start,pad,space,s,pos,base];

#    ---     ---     ---     ---     ---

  strseg=sepcstr(s);
  hasstr=(
    len(strseg)
    *(not (base==com_fld or base==lcm_fld))
    *(s[0]!='#')

  );

  if(not hasstr or len(s)<space):
    __fillcode();

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
        __fillcode();

      no_goto[4]=s[beg_str:end_str];
      __fillstrg();


      beg_cde=end_str;

#    ---     ---     ---     ---     ---

    end_cde=len(s);
    cde_seg=s[beg_cde:end_cde];

    if(len(cde_seg)):
      no_goto[4]=cde_seg;
      __fillcode();

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
  for ch in [')(', '){', '*)', '} ;']:
    l2=l1.split(ch);
    l2=[sub for sub in l2];
    if(j<2):
      ch=ch[0]+' '+ch[1]

    elif(j==2):
      ch=ch+' ';

    else:
      ch=ch[0]+ch[2];

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

  return (s, atl);

#    ---     ---     ---     ---     ---

docf  = docbox("idntest.c");

culsp = "//   ---     ---     ---     ---     ---";
culsg = 0;

s     = "";

n_nl  = 0;

lines = [];
cs_nx = 0;
cs_sw = 0;

with open(root+"/idntest.c", "r") as src:
  lines=src.readlines()[docf[1]:];

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

    if(line[0:8]=="//   ---"):
      line=line.replace(culsp,'').strip();
      line=line.replace('// ', '');
      line=' '+line if len(line) else '';
      ln='#:'+hex(culsg).upper()+';>';
      ln=ln[0:3]+'x'+ln[4:];

      line=culsp+'\n// '+ln+line; culsg+=1;

    s=s+line+('\n' if line[-1]!='\n' else '');
    continue;

#    ---     ---     ---     ---     ---

  if(line[0:6]=="switch"):
    cs_sw=lvl;

  elif(line[0:5]=="case "   \
  or   line[0:8]=='default '):
    cs_nx=0;

  elif(cs_sw):
    cs_nx=lvl<=cs_sw+1;

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

      frun='\n';            #:CURLY_END;>

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

s=docf[0]+s;

with open(root+"/IDNT.c", "w+") as dst:
  dst.write(s);
