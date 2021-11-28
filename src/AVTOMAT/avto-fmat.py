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

ff_write   = 0x0001;
ff_newline = 0x0002;
ff_indent  = 0x0004;

ff_lcomm   = 0x0008;
ff_bcomm   = 0x0010;
ff_prepo   = 0x0020;
ff_label   = 0x0040;
ff_wsig    = 0x0080;

ff_lnlup   = 0x0100;
ff_nnl     = 0x0200;
ff_ncnl    = 0x0400;
ff_string  = 0x0800;

#    ---     ---     ---     ---     ---

llb        = [');', '};', '];', '!=', '|=', '&=',
              '++', '--', '+=', '*=', '/=', '%=',
              '||', '&&', '^=', '~' , '!(','&=~',
              '->', '>=', '<=', '<<', '>>', ' ' ,
              '-=', 
              ',' , '(' , ')' , ';' , '{' , '}' ,
              '-' , '/' , '&' , '!' , '|' , '%' ,
              '+' , '^' , '>' , '<' , '=' , '*)'  ];

lbrk       = [');', '};', '];', '!=', '|=', '&=',
              '++', '--', '+=', '*=', '/=', '%=',
              '||', '&&', '^=', '~' , '!(','&=~',
              '>=', '<=', '<<', '>>', ' ' ,
              '-=', 
              ',' , '(' , ')' , ';' , '{' , '}' ,
              '/' , '&' , '!' , '|' , '%' ,
              '+' , '^' , '=' , '*)'              ];

i_wid      = 2;
l_wid      = 56;
l_mid      = 28;
c_len      = (l_wid-l_mid)-3;

bx_cap     = "//"+('*'*(l_mid-2))+"\n";
bx_fld     = "//"+(' '*(l_mid-3))+"*\n";
com_fld    = "// "+(' '*c_len)+'\n';
lcm_fld    = "// "+(' '*l_wid)+'\n';
cde_fld    = " "+(' '*(l_wid))+'\n';

cde_fa     = cde_fld[:(l_mid)];
cde_fb     = cde_fld[(l_mid):];

culsp      = "//   ---     ---     ---" \
             +"     ---     ---";

#    ---     ---     ---     ---     ---

lvl        = 0;
state      = 0x00;

rgi        = ' '*i_wid;
chops      =[
  ',','-','+','|','&',
  '/','*','%','^','<',
  '>','='
];

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
  return __fill(s, bx_fld,cap=-1);

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
  with open(fname, "r") as src:
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
  s=s+bx_fill(mname.upper());
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

def gi():
  return rgi*lvl;

def ri(line):
  indent="";
  for c in line:
    if c=='\n':
      continue;

    elif c not in ' \t':
      break;

    indent=indent+c;

  return indent;

def detchain(s,line):

  rl=despace(line);
  rl=rl.split("//")[0];
  rl=rl.split("/*")[0];

  if(len(rl)<l_wid):
    return 0;

  i=0;
  for c in s:

    nx='';
    if(i<(len(s)-1)):
      nx=s[i+1];

    if( c in chops
    and nx.isalnum()):
      return 1;

    if(c==')'):
      return 1;

    i+=1;

  return 0;

def detlabel(s,idex):

  llvl = lvl;row  ="";
  i    = 0       ;found=0 ;

  for c in s[idex:]:

    row=row+c;

    if( (':'       in row)
    or  ("case"    in row)
    or  ("default" in row) ):
      i-=len(row)+1;found=1;
      break;

    elif(c in ' ;\n'):
      row="";

    i+=1;

  if(i==-1):
    return 0;

  return (idex+i)*found;

#    ---     ---     ---     ---     ---

def opnparn(seg):

  global state;

  parens=0;i=0;j=0;
  lsplit=0;do_split=0;opch=0;

  seg_cpy=str(seg);

  for c in seg_cpy:

    if(seg[i]=='"'):

      if(not (state&ff_string)):
        state|=ff_string;

      else:
        state&=~ff_string;

    if(state&ff_string):
      i+=1;j+=1;continue;

    elif(seg[i] in ' \n\t'):
      if(seg[i]=='\n'):
        lsplit=i+1;
        do_split=(
          len(seg[lsplit:])
          >l_mid
        );

      i+=1;continue;
    
    if(seg[i]=='('):
      parens=1;

    elif(seg[i]==')'):
      parens=0;

    if(parens):
      continue;

#    ---     ---     ---     ---     ---

    if(seg[i] in chops and do_split):

      print(seg[i]);

      if(seg[i]==','):
        pad=1+len(rgi)+opch;
        seg=(
          seg[:i+1]+'\n'
         +('\n'*opch)
         +gi()
         +rgi
         +seg[i+1:]

        );opch=0;

        i+=pad+(i_wid*lvl);j=0;

#    ---     ---     ---     ---     ---

      elif(j>=l_mid):
        pad=2;
        opch=1;
        seg=(
          seg[:i]+'\n'
         +gi()
         +' '
         +seg[i:]

        );

        i+=pad+(i_wid*lvl);j=0;

      do_split=0;

#    ---     ---     ---     ---     ---

    i+=1;j+=1;

  return seg;

#    ---     ---     ---     ---     ---

def accom(row,chain):

  if(not len(row)):
    return '';

  if(len(row)>l_mid):
    pass;

  if(chain):
    row=opnparn(row);

  return row;

#    ---     ---     ---     ---     ---

def iatst(row):
  if(row==('\n'*len(row))):
    return 0;

  _gi=gi(); return (
    not row.startswith(_gi)
    and not row[1:].startswith(_gi)

  );

#    ---     ---     ---     ---     ---

def cmtp(line):

  if(len(line)>=8):
    if(line[0:8]=="//   ---"):
      return 0x01;

  if(len(line)>=3):

    if((line[0:3]=="//*")

    or(line[0:2]=="//"
    and  line[ -1]=="*"  )):
      return 0x02;

  if(len(line)>=2):
    if(line[0:2]=="/*"):
      return 0x04;

  return 0x00;

#    ---     ---     ---     ---     ---

def format(fname):

  global lvl,state;

  docf   = docbox(fname);
  state  = ff_wsig|ff_lnlup;

  lf_flb = 0x01;

  last   = '';nx      = '' ;i   =0 ;
  result = "";row     = "" ;
  chain  = 0 ;clchain = 0  ;

  last_nl= 0 ;tok     = "_";tokb="";
  lblspan= 0 ;culsg   = 0  ;

#    ---     ---     ---     ---     ---
# fmat:top

  lines=[];
  with open(fname, 'r') as file:
    lines=file.readlines()[docf[1]:];

  flines=[];
  for line in lines:

    stline=line.strip();
    if(not len(stline)):

      line=('\n'
        if not (state&ff_nnl)
        else ''

      );state|=ff_nnl;

    else:
      state&=~ff_nnl;

#    ---     ---     ---     ---     ---
# fmat:lcomm

    x=cmtp(stline);

    if(x):

      if(culsp in line):
        line=line.replace(culsp,'').strip();
        line=line.replace('// ', '');
        line=' '+line if len(line) else '';
        ln='#:'+hex(culsg).upper()+';>';
        ln=ln[0:3]+'x'+ln[4:];
        
        line=culsp+'\n// '+ln+line; culsg+=1;

      if(len(row) and row[-1]!='\n'):
        row=row+'\n';

      line=line+(
        '\n' if  line[-1]!='\n'
             and line[-1]!='*' else ''

      );

    flines.append(line);

#    ---     ---     ---     ---     ---
# fmat:rdline

  s=''.join(flines);
  for line in flines:

    if(culsp in line
    or (line.startswith("//")
    and ("#:" in line
    and ";>" in line))):

      if(len(result)):
        if(result[-1]!='\n'):
          result=result+'\n';

      if(culsp in line):
        result=result+'\n';
        i+=1;

      result=result+line;
      i+=len(line);
      continue;

    if(len(line)<2):
      result=result+line;
      i+=len(line);
      continue;

    
    j=0;state&=~ff_ncnl;
    for c in line:

      nx='';
      if(i<(len(s)-1)):
        nx=s[i+1];

      if((state&ff_wsig) and c!=' '):
        row=row+c;state&=~ff_wsig;

      elif((state&ff_wsig)==0):
        row=row+c;

#    ---     ---     ---     ---     ---
# fmat:h_lcomm

      if(state&ff_lcomm):
        if(c=='\n' and j==len(line)-1):
          state&=~(ff_lcomm|ff_bcomm);
          state|=(
            ff_write
           |(ff_newline*line[-2]!='*')
           |ff_indent

          );

      elif(state&ff_bcomm):
        if(c+nx=='*/'):
          state&=~ff_bcomm;
          state|=ff_write;

      elif(state&ff_prepo):
        if(c=='\n' and last!='\\'):
          state&=~ff_prepo;
          state|=ff_write;
          row=row[:-1];

#    ---     ---     ---     ---     ---
# fmat:string

      elif(c=='"'):
        if(not (state&ff_string)):
          state|=ff_string;

        else:
          state&=~ff_string;

      elif(state&ff_string):
        pass;

#    ---     ---     ---     ---     ---
# fmat:case

      elif(c==';'):
        if( (state&ff_label)
        and (state&ff_ncnl ) ):
          state&=~ff_ncnl;
          state|=ff_newline|ff_indent;

        state|=(
          ff_write
         |(ff_newline*(nx=='}'))

        );

      elif(c==':'):
        pass; #out of order!

        #state|=ff_ncnl;
        #state|=ff_label;
        #lblspan=detlabel(s,i+1);

        #lvl+=(state&ff_lnlup)!=0;
        #state&=~ff_lnlup;

#    ---     ---     ---     ---     ---
# fmat:space

      elif(c==' '):
        igws=not state&(
          ff_lcomm
         |ff_bcomm
         |ff_prepo

        );

        if(igws and len(row)>1):
          if(row[-2]=='\n'):
            row=row[:-1];

      elif(c=='#'):
        if(not last_nl):
          row=row[:-1]+'\n'+c;

        state|=ff_prepo;

#    ---     ---     ---     ---     ---
# fmat:nl

      elif(c=='\n'):

        if( (state&ff_label)
        and (state&ff_ncnl) ):
          state&=~ff_ncnl;
          state|=ff_newline|ff_indent;

        state|=(
          ff_write
          |(ff_indent
           *(not iatst(row))
           *(not (state&ff_lcomm))
          )
          |ff_newline

        );row=row[:-1];

        state&=~ff_lcomm;
        state&=~(
          ff_prepo*(last!='\\')

        );

#    ---     ---     ---     ---     ---
# fmat:c_comm

      elif(c+nx=='//'):
        state|=ff_lcomm;

      elif(c+nx=='/*'
      and not (state&ff_lcomm)):
        state|=ff_bcomm;

#    ---     ---     ---     ---     ---
# fmat:chain

      elif(c in '{('):

        if(c=='('):
          chain+=detchain(s[i+1:],line);

        if(chain==1 or c=='{'):
          lvl+=(state&ff_lnlup)!=0;

          state|=(
            ff_write
           |(ff_newline*(nx!='\n')
           *('//' not in line))

          );state|=(
            ff_indent
           *((state&ff_newline)!=0)

          );

#    ---     ---     ---     ---     ---
# fmat:clchain

      elif(c in '})'):

        if(chain==1 or c=='}'):

          row=(
            row[:-1].rstrip(' ')
           +('\n'*(not last_nl)
            *(lvl!=0))

           +('\n'*(last!=c)*(not last_nl)
            *(lvl!=0))

          );state|=ff_write;
          lvl-=(lvl!=0)*((state&ff_lnlup)!=0);
          row=row+gi()+c;

        if(c==')'):
          chain-=(chain!=0);
          clchain=1;

#    ---     ---     ---     ---     ---
# fmat:flush

      elif(i==len(s)-1):
        state|=ff_write|ff_newline;

      if(state&ff_write):

        if(lblspan and i>=lblspan):

          #lvl-=(lvl!=0)*((state&ff_lnlup)!=0);
          lblspan=0;

        result=result+accom(row,clchain);
        row=(

          ('\n'*((state&ff_newline)!=0)
               *((state&ff_string )==0))

         +(gi()*((state&ff_indent )!=0))

        );last_nl=(
          (len(row) and row[-1]=='\n')
          or (state&ff_newline)

        );

#    ---     ---     ---     ---     ---

        state|=(
          ff_lnlup
         *((state&ff_newline)!=0)

        );

        state&=~(
          ff_write
         |ff_indent
         |ff_newline
         |(ff_label*(lblspan==0))

        );state|=ff_wsig;
        clchain-=(clchain!=0);

#    ---     ---     ---     ---     ---

      if(ord(c)>0x20):
        last=c;
        if(c.isalnum() or c==':'):
          tokb=tokb+c;

      else:
        tok=tokb if tokb else "_";tokb="";

      i+=1;j+=1;

#    ---     ---     ---     ---     ---
# fmat:dcomm

  s=docf[0];

  lines=([
    sub+'\n' for sub in result.split('\n')

  ]);j=0;

  for line in lines:

    if(line=='\n'):
      s=s+'\n';j+=1;continue;

    indent = ri(line);
    app_com="";

    if(len(line)<2):
      pass;

    elif(line[0:2]=='//'):
      pass;

    elif("//" in line):

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

      if(not len(line)):    #:YESCOM;>
                            #:NOCODE;>
        s=s+'\n'.join(com)+'\n';
        continue;

      elif(not len(com)):
        pass;

      elif((len(indent)+len(line)<=l_mid)):
        com[0]=com[0].lstrip(' ');
        app_com=('\n'.join(com)+'\n');

        line=cfa_fill(line,cap=1);
        line=line[:l_mid-len(indent)];

      else:                 #:FULINE;>
        s=s+'\n'.join(com)+'\n';

#    ---     ---     ---     ---     ---

    else:                   #:NOCOM;>
      line   = cde_fill(despace(line),cap=5+len(indent));

      line   = line[:-1].rstrip(' ')+'\n';

#    ---     ---     ---     ---     ---

    if(len(line.strip())<=1):
      if(not len(line) and line not in llb):
        continue;

    line=mkline(              #:MKLINE;>
      line,indent,
      rgi

    )+app_com;

    line=line.rstrip()+'\n';
    s=s+line;j+=1;

  return s;

#    ---     ---     ---     ---     ---

import sys,os;

argc=len(sys.argv);
argv=None;
if(argc>=2):
  argv=sys.argv[1:];
  argc-=1;

else:
  print(
    "Usage: fmat filepath\n\n"                     \
    "formats a C file to 56 columns\n\n"           \
    "Options:\n"                                   \
    " outpath  writes to file rather than stdout\n"\
    " -r       replaces the original\n\n"          \

  );exit();

root=os.getcwd()+'/';
srcp=argv[0];

oksrc=(os.path.exists(root+srcp)
  if os.path.isfile(root+srcp)
  else 0

);

if(not oksrc):
  print(f"Invalid source <{root+srcp}>");
  exit();

s=format(root+srcp);

if(argc>=2):
  dstp=srcp if argv[1]=="-r" else argv[1];

  with open(root+dstp, "w+") as dst:
    dst.write(s);

else:
  print(s);
