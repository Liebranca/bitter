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
    TAB    =>  2,
    DEBUG  =>  0,

  };

# ---   *   ---   *   ---
# dirty stuff, don't touch

  use constant {
    PE_BEG => "\$:",
    PE_END => ";>",

  };

# ---   *   ---   *   ---
# cache

  my %CACHE = (  
    -SPACE => SC_W,
    -LINE  => ""  ,

  );

# ---   *   ---   *   ---
# utils

sub ltrim {my $s=shift;$s=~s/^\s+//    ;return $s;};
sub rtrim {my $s=shift;$s=~s/\s+$//    ;return $s;};
sub  trim {my $s=shift;$s=~s/^\s|\s+$//;return $s;};

# remove ws between string and substr
sub despace {
  my $s=ltrim shift;
  my $c=shift;
  if(!$c) {return ltrim $s;};

  $s=ltrim substr $s,length $c,length $s;
  return $c.$s;

};

# $:wrap;> f,arg..argn
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

# ---   *   ---   *   ---
# rules

# \n after substr, full tab
sub ch_parens_beg {

  my $i=shift;
  my $c=shift;

  my $sub=substr $CACHE{-LINE},0,$i+length $c;
  $CACHE{-LINE}=despace(
  
    ltrim substr(
    $CACHE{-LINE},length $sub,
    length $CACHE{-LINE}
    
    ),$c

  );print "$sub\n".pe_scpx "tab",TAB;

};

# \n before substr, half tab
sub ch_parens_mid {

  my $i=shift;
  my $c=shift;
  
  my $sub=substr(
    $CACHE{-LINE},0,
    $i-length($c)+1
    
  );$CACHE{-LINE}=despace substr(
    $CACHE{-LINE},length $sub,
    length $CACHE{-LINE}

  ),$c;print "$sub\n".pe_scpx "tab",TAB-length $c;

};

# \n before substr, no tab
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
@cash::wrap_rules=(

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

  # comment out
  my $debug="\n";
  
  if($CACHE{-SPACE} < 2) {
    for(my $x=0;$x<($#cash::wrap_rules)*2;$x+=2) {
      my $i=0;my $i_=0;
      my $c=$cash::wrap_rules[$x];
      if(!$c) {next;};
      
      while(($i_=index substr(
        $CACHE{-LINE},$i,SC_W), $c)>0
        
      ) { $i=$i_; if(DEBUG) {$debug=$debug."#$i '$c'\n";};

      };if($i>0) {
        if(DEBUG) {$debug=$debug."#broke at $i '$c'\n";};
        $cash::wrap_rules[$x+1]->($i,$c);$x=0;

      };

    };
  };

  print "$CACHE{-LINE}\n";
  if(DEBUG) {print $debug;};
  
  return $CACHE{-SPACE};

};

# ---   *   ---   *   ---
1; #ret
