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
    PE_BEG   =>  '$:'       ,
    PE_END   =>  ';>'       ,
    PE_NIHIL =>  "#:NIHIL;>\n",

  };

# ---   *   ---   *   ---
# standard and user defined color schemes

# default palette for $:%col;>
  my @DEFPAL=(
    0x000020,   # defbg 0
    0x000040,   # curbg 1
    0x800000,   # selbg 2
    0x400040,   # hglbg 3

    0x40AE40,   # deffg 4
    0x00A0A0,   # strfg 5
    0xB0B000,   # carfg 6
    0xD09820,   # numfg 7
    
    0xB0A040,   # opsfg 8
    0x8020A0,   # keyfg 9
    0x0080B0,   # insfg A
    0xA01020,   # dirfg B

    0x202060,   # shblk C
    0x404080,   # shdrk D
    0x6060A0,   # shlgt E
    0x8080C0,   # shwht F

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
    
    -PE_PALID => "def"      ,
    -PE_COLID => 0x04       ,

  );

# ---   *   ---   *   ---
# peso function table

my %PESO=(

  # on/off
  "peso_beg",      \&pe_beg      ,
  "peso_end",      \&pe_end      ,

  # formatting
  "pad"     ,      \&pex_pad     ,

  # coloring
  "col"     ,      \&pex_col     ,
  "pal"     ,      \&pex_pal     ,
  "pal_beg" ,      \&pex_pal_beg ,
  "pal_def" ,      \&pex_pal_def ,
  "pal_end" ,      \&pex_pal_end ,

);

# ---   *   ---   *   ---
# peso commands for cash

# to turn the interpreter on and off
sub pe_beg {$CACHE{-PE_RUN}=1;return PE_NIHIL;};
sub pe_end {$CACHE{-PE_RUN}=0;return PE_NIHIL;};

# ---   *   ---   *   ---
# formatting functions

# n=0..SC_M
# insert n spaces
sub pex_pad {
  my $n=shift;
  return " "x$n;

};

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
  
  my $palid=$CACHE{-PE_PALID};
  
  my @pal=@{ $PALETTES{$palid} };
  my @fg=rrggbb_decode($pal[$colid & 0x0F]);

  
  my @bg=rrggbb_decode($pal[($colid & 0xF0)>>4]);

  return "\e[38;2;$fg[0];$fg[1];$fg[2]m".
         "\e[48;2;$bg[0];$bg[1];$bg[2]m";

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

  $pe_com=~s/\$:([%\/]?)([\w|\d]+)//;

  my $md=$1;

  if($md) {
    $md=(ord($md)==ord('%')) ? "_beg" : "_end";

  # i love perl
  };my $ex="$2$md";
    my @ar=();
  
  if(!(index $pe_com,PE_END)==0) {  
    $pe_com=~s/([\w|\d|\,]+);>//;
    @ar=split ',',$1;
    for(my $x=0;$x<$#ar;$x++) {
      $ar[$x]=ltrim $ar[$x];

    };

  };return $PESO{$ex}->(@ar,$pe_sub);

};

# block=textual input any
# find wrapped peso
sub pe_strip {
  my $block=shift;
  my $sublk="";

  if($CACHE{-PE_RUN} || (index $block,"\$:%peso;>")>=0) {
    $block=~ s/(\$:([%|\/]?)[\w|\d|\s|\,]+;>)\s*//;

    # for [%|\/] switch ops,
    # we send enclosed text as additional input
    if($2) {
      $sublk=substr $block,0,(index $block,PE_BEG);
      $block=~ s/${sublk}\s*//;

    };return (pe_exec $1,$sublk).$block;

  };$block=~ s/(\$:([%|\/]?)[\w|\d|\s|\,]+;>)\s*//;
  return $block;

};

# ---   *   ---   *   ---

# block=textual input any
# looping {read until delim, then exec}
sub pe_rdin {

  # replace with file later/add -f option
  my $block=shift;
  my $s="";
  my $ni=0;

  while($block && ($ni=index $block,PE_BEG)>=0) {
  
    $block=pe_strip (
      substr $block,$ni,length $block
      
    );

    my $sub= substr $block,0,index($block,PE_BEG);
    $sub=~ s/#:NIHIL;>\s*//;

    $s=$s.$sub;

  };

  return "$s\e[0m";

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
