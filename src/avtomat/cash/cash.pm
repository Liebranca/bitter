#!/usr/bin/perl

#***************************
# CASH                     *
# rich string utilities    *
#                          *
# LIBRE SOFTWARE           *
# Licensed under GNU GPL3  *
# be a bro and inherit     *
#                          *
# CONTRIBUTORS             *
# lyeb,                    *
#***************************

# deps
  use strict;
  use warnings;

  use Term::ReadKey qw(GetTerminalSize);

package cash;

# ---   *   ---   *   ---
# settings

  use constant {    
    SC_W   => 56,
    SC_M   => 28,
    SC_Q   => 14,
    PAD    =>  2,
    DEBUG  =>  0,

  };

# ---   *   ---   *   ---
# dirty stuff, don't touch

  use constant {
  
    # escape delimiters
    PE_BEG   =>  '$:'         ,
    PE_END   =>  ';>'         ,
    PE_NIHIL =>  "#:NIHIL;>\n",

  };

# ---   *   ---   *   ---
# standard and user defined color schemes

# default palette for $:col;>
  my @DEFPAL=(
    0x000020,   # black
    0x7F0000,   # red
    0x208020,   # green
    0xD09820,   # yellow
    
    0x004080,   # blue
    0x40A040,   # magenta
    0x006060,   # cyan
    0x8080C0,   # white
    
    0x000080,   # black
    0xA01020,   # red
    0x40AE40,   # green
    0xB0B000,   # yellow
    
    0x0040D0,   # blue
    0x8000A0,   # magenta
    0x00A0A0,   # cyan
    0xB0A040,   # white

  );

# hash for the standard and user-defined palettes
  my %PALETTES=(
    "def" => \@DEFPAL,
    "emp" => []      ,

  );

# ---   *   ---   *   ---
# internal global storage

  my %CACHE = (  
    -SPACE    => SC_W       ,
    -FILE     => ""         ,
    -LINE     => ""         ,

    -SC_SZX   =>          68,
    -SC_SZY   =>          30,
    
    -PE_PALID => "def"      ,
    -PE_COLID => 0x04       ,

  );

# ---   *   ---   *   ---
# peso function table

my %PESO=(

  # on/off
  "peso_beg"  ,      \&pe_beg         ,
  "peso_end"  ,      \&pe_end         ,

  # formatting
  "pad"       ,      \&pex_pad        ,
  "center_beg",      \&pex_center_beg ,
  "center_end",      \&pex_center_end ,

  # coloring
  "col"       ,      \&pex_col        ,
  "pal"       ,      \&pex_pal        ,
  "pal_beg"   ,      \&pex_pal_beg    ,
  "pal_def"   ,      \&pex_pal_def    ,
  "pal_end"   ,      \&pex_pal_end    ,

);

# ---   *   ---   *   ---
# peso commands for cash

# to turn the interpreter on and off
sub pe_beg {$CACHE{-PE_RUN}=1;return pe_strip(shift);};
sub pe_end {$CACHE{-PE_RUN}=0;return PE_NIHIL;};

# ---   *   ---   *   ---
# formatting functions

# n=0..SC_M
# insert n spaces
sub pex_pad {
  my $n=shift;
  return " "x$n;

};

# body=text in any
# centers body according to screen width
sub pex_center_beg {

  my @body=split "\n",shift;
  my $result="";
  
  while(@body) {
    my $line=shift @body;
    if((index $line,PE_BEG)>-1) {
      $result=$result.pe_strip($line);
      next;

    };
    
    my $space=($CACHE{-SC_SZX}-length $line)/2;$space--;
    $space=sprintf "\e[%iG",$space;
    $result=$result.$space.$line."\e[1B";

  };return $result;

};

# dummy
# terminate centering body
sub pex_center_end {return PE_NIHIL;};

# ---   *   ---   *   ---
# coloring functions

# palid=name
# set palette to use
sub pex_pal {
  my $palid=shift;
  $CACHE{-PE_PALID}=$palid;

  return PE_NIHIL;

};

# id=00..FF;==((bg<<4)|fg)
# set color to use
sub pex_col {
  my $colid=hex shift;

  my $fg=$colid&0x0F;
  my $bg=($colid&0xF0)>>4;

  my $fg_bold=($fg>7) ? 1 : 22;
  my $bg_bold=($bg>7) ? 5 : 25;

  return sprintf "\e[$fg_bold;$bg_bold;%i;%im",30+($fg&0x7),40+($bg&0x7);
  
};

# palid=name
# start defining palette colors
sub pex_pal_beg {
  my $palid=shift;
  $CACHE{-PE_PALID}=$palid;

  if( !exists $PALETTES{$palid} ) {
    @PALETTES{$palid}=[
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      0,0,0,0,
      
    ];

  };my $sublk=shift;
  $sublk=rtrim(ltrim(trim("$sublk")));
  pex_pal_def($sublk);

  return PE_NIHIL;

};

# n=0..F,col=#RRGGBB;
# sets palette[n] to col
sub pex_pal_def {

  my @ar=split ' ',$_[0];
  
  my $palid=$CACHE{-PE_PALID};
  my @pal=@{ $PALETTES{$palid} };

  while(@ar) {
  
    my $n=shift @ar;
    my $col=hex(shift @ar);

    $pal[$n]=$col;
    
  };@{ $PALETTES{$palid} }=@pal;  
  return PE_NIHIL;

};

# dummy
# stop palette definition
sub pex_pal_end {return PE_NIHIL;};

# ---   *   ---   *   ---
# utils

# cleanse ws
sub ltrim {my $s=shift;$s=~s/^\s+//    ;return $s;};
sub rtrim {my $s=shift;$s=~s/\s+$//    ;return $s;};
sub  trim {my $s=shift;$s=~s/^\s|\s+$//;return $s;};

# get (r,g,b) from rrggbbhex
sub rrggbb_decode {
  my $lit=shift;
  
  my $b=($lit & 0x0000FF)>> 0;
  my $g=($lit & 0x00FF00)>> 8;
  my $r=($lit & 0xFF0000)>>16; 

  return ($r,$g,$b);

};

# remove ws between string and substr
sub despace {
  my $s=ltrim shift;
  my $c=shift;
  if(!$c) {return ltrim $s;};

  $s=ltrim substr $s,length $c,length $s;
  return $c.$s;

};

# ---   *   ---   *   ---

# f=function, args=arg0,arg1,..argn
# wrap peso as f,arg..argn
sub pe_scpx {
  my $argc=$#_;
  if($argc<0) {return;};

  my $f=shift;
  
  my $s=PE_BEG."$f";
  while($argc>0) {
    $s=$s." ".shift;
    $argc--;

  };return $s.PE_END;

};

# pe_com=$:(%|/)?expr;>,
# pe_sub=delimited block
# execute wrapped peso
sub pe_exec {

  my $pe_com=shift;
  my $pe_sub=shift;

  # get tag name
  $pe_com=~s/\$:([%\/]?)([\w|\d]+)//;

  # handle on and off switches/body tags
  my $md=$1;if($md) {
    $md=(ord($md)==ord('%')) ? "_beg" : "_end";

  # i love perl
  };my $ex="$2$md";
    my @ar=();

  # fetch args if any
  if(!(index $pe_com,PE_END)==0) {  
    $pe_com=~s/([\w|\d|\,]+);>//;
    @ar=split ',',$1;
    for(my $x=0;$x<$#ar;$x++) {
      $ar[$x]=ltrim $ar[$x];

    };

  # execute tag/body
  };return $PESO{$ex}->(@ar,$pe_sub);

};

# block=textual input any
# find and execute wrapped peso
sub pe_strip {
  my $block=shift;
  my $sublk="";

  # interpreter is active or is being activated
  if($CACHE{-PE_RUN} || (index $block,"\$:%peso;>")>=0) {
    $block=~ s/(\$:([%|\/]?)[\w|\d|\s|\,]+;>)\s*//;
    my $opsw=$1;

    # for [%|\/] switch ops,
    # we send enclosed text as additional input
    if($2) {

      # on $:%;>find matching $:/;> tag
      my $sw=(ord $2==ord "%") ? 1 : 0;
      if($sw) {
        my $clsw=$1;
        $clsw=~ s/%/\//;
        $clsw=~ s/\s+[\w|\d|\s|\,]+;>\s*//;
        $sublk=substr $block,0,(index $block,$clsw);

      # else simply exec up to next tag
      } else {
        $sublk=substr $block,0,(index $block,PE_BEG);

      # remove executed section
      };$block=substr $block,length $sublk,length $block;

    };return (pe_exec $opsw,$sublk).$block;

  # non-interpreter runs untested
  };$block=~ s/(\$:([%|\/]?)[\w|\d|\s|\,]+;>)\s*//;
  return $block;

};

# ---   *   ---   *   ---

# block=textual input any
# looping {read until delim, then exec}
sub pe_rdin {

  # get screen dimentions
  ($CACHE{-SC_SZX},$CACHE{-SC_SZY})=
    Term::ReadKey::GetTerminalSize();

  # replace with file later/add -f option
  my $block=shift;
  my $s="\e[2J\e[H";
  my $ni=0;

  # read block and execute
  while($block && ($ni=index $block,PE_BEG)>=0) {
  
    $block=pe_strip (
      substr $block,$ni,length $block
      
    );

    my $sub= substr $block,0,index($block,PE_BEG);
    $sub=~ s/#:NIHIL;>\s*//;

    $s=$s.$sub;

  };

  # set console defaults on end
  return "$s\e[0m\e[1G";

};

# ---   *   ---   *   ---
# rules for formatting

# \n after substr, full pad
sub ch_parens_beg {

  my $i=shift;
  my $c=shift;

  my $sub=substr $CACHE{-LINE},0,$i+length $c;
  $CACHE{-LINE}=despace(
  
    ltrim substr(
    $CACHE{-LINE},length $sub,
    length $CACHE{-LINE}
    
    ),$c

  );print "$sub\n".pe_scpx "pad",PAD;

};

# \n before substr, half pad
sub ch_parens_mid {

  my $i=shift;
  my $c=shift;
  
  my $sub=substr(
    $CACHE{-LINE},0,
    $i-length($c)+1
    
  );$CACHE{-LINE}=despace substr(
    $CACHE{-LINE},length $sub,
    length $CACHE{-LINE}

  ),$c;print "$sub\n".pe_scpx "pad",PAD-length $c;

};

# \n before substr, no pad
sub ch_parens_end {

  my $i=shift;
  my $c=shift;

  my $sub=substr $CACHE{-LINE},0,$i;
  $CACHE{-LINE}=despace substr(
    $CACHE{-LINE},$i,
    length $CACHE{-LINE}
    
  ),$c;print "$sub\n\n";

};

# ---   *   ---   *   ---

# remove substr and trim
sub ch_delim_cut {

  my $i=shift;
  my $c=shift;

  my $sub=substr $CACHE{-LINE},0,$i;
  $CACHE{-LINE}=ltrim substr(
    $CACHE{-LINE},$i+1,
    length $CACHE{-LINE}

  );print "$sub\n";

};

# ---   *   ---   *   ---

# on substring found, call associated rout
my @WRAP_RULES=(

  "{"   , \&ch_parens_beg,
  "("   , \&ch_parens_beg,
  "=~"  , \&ch_parens_beg,
  "="   , \&ch_parens_beg,
  ","   , \&ch_parens_beg,

  "};"  , \&ch_parens_end,
  "}"   , \&ch_parens_end,
  ");"  , \&ch_parens_end,
  ")"   , \&ch_parens_end,
  
  "->"  , \&ch_parens_mid,
  "&&"  , \&ch_parens_mid,
  "||"  , \&ch_parens_mid,
  
  "&"   , \&ch_parens_mid,
  "|"   , \&ch_parens_mid,
  "^"   , \&ch_parens_mid,
  "~"   , \&ch_parens_mid,
  "+"   , \&ch_parens_mid,
  "-"   , \&ch_parens_mid,
  "*"   , \&ch_parens_mid,
  "/"   , \&ch_parens_mid,

  " "   , \&ch_delim_cut ,

);

# ---   *   ---   *   ---

# format lines when they exceed SC_W chars
# returns avail-(length line), where avail is
# remaining space

sub wrapl {

  # set cur line and discount length
  $CACHE{-LINE}=ltrim $_[0];
  $CACHE{-SPACE}-=length $CACHE{-LINE};

  my $debug="\n";
  
  if($CACHE{-SPACE} < 2) {
    for(my $x=0;$x<($#WRAP_RULES)*2;$x+=2) {
      my $i=0;my $i_=0;
      my $c=$WRAP_RULES[$x];
      if(!$c) {next;};
      
      while(($i_=index substr(
        $CACHE{-LINE},$i,SC_W), $c)>0
        
      ) { $i=$i_; if(DEBUG) {$debug=$debug."#$i '$c'\n";};

      };if($i>0) {
        if(DEBUG) {$debug=$debug."#broke at $i '$c'\n";};
        $WRAP_RULES[$x+1]->($i,$c);$x=0;

      };

    };
  };

  print "$CACHE{-LINE}\n";
  if(DEBUG) {print $debug;};
  
  return $CACHE{-SPACE};

};

# ---   *   ---   *   ---
1; #ret
