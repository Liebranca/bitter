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


package cash;

# ---   *   ---   *   ---
# settings

  use constant {    
    SC_W => 56,
    SC_M => 28,
    SC_Q => 14,
    TAB  =>  2,

  };

# ---   *   ---   *   ---
# cache

  my %CACHE = {  
    -SPACE => SC_W,
    -LINE  => ""  ,

  };

# ---   *   ---   *   ---
# utils

sub ltrim {my $s=shift;$s=~s/^\s+//    ;return $s;};
sub rtrim {my $s=shift;$s=~s/\s+$//    ;return $s;};
sub  trim {my $s=shift;$s=~s/^\s|\s+$//;return $s;};

sub despace {
  my $s=ltrim shift;
  my $c=shift;

  my $s=ltrim substr $s,length $c,length $s;
  return $c.$s;

};

# ---   *   ---   *   ---
# rules

sub ch_parens_beg {

  my $i=shift;
  my $c=shift;

  my $sub=substr $CACHE{-LINE},0,$i+length $c;
  $CACHE{-LINE}=despace(
  
    ltrim substr(
    $CACHE{-LINE},length $sub,
    length $CACHE{-LINE}
    
    ),$c

  );print "$sub\n\\:".TAB.";>";

};

sub ch_parens_mid {

  my $i=shift;
  my $c=shift;
  
  my $sub=substr(
    $CACHE{-LINE},0,
    $i-length($c)+1
    
  );$CACHE{-LINE}=despace substr(
    $CACHE{-LINE},length $sub,
    length $CACHE{-LINE}

  ),$c;print "$sub\n\\:".(TAB-length @_[1]).";>";

};

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

@wrap_rules=(

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

sub wrapl {

  $CACHE{-LINE}=ltrim @_[0];
  $CACHE{-SPACE}-=length $CACHE{-LINE};

  my $debug="\n";
  
  if($CACHE{-SPACE} < 2) {
    for(my $x=0;$x<@wrap_rules*2;$x+=2) {
      my $i=0;my $i_=0;my $c=@wrap_rules[$x];
      
      while(($i_=index substr(
        $CACHE{-LINE},$i,SC_W), $c)>0
        
      ) {$i=$i_;$debug=$debug."#$i '$c'\n";};if($i>0) {
        $debug=$debug."#broke at $i '$c'\n";
        @wrap_rules[$x+1]->($i,$c);$x=0;

      };

    };
  };

  print "$CACHE{-LINE}\n";
  #print $debug;
  
  return $SPACE;

};

# ---   *   ---   *   ---
1; #ret
