#!/usr/bin/perl
# ---   *   ---   *   ---
# transforms on square arrays

# ---   *   ---   *   ---
# deps

  use v5.36.0;
  use strict;
  use warnings;

  use English qw(-no_match_vars);

# ---   *   ---   *   ---

my $SZ    = 8;
my $SZ_I  = $SZ-1;
my $SZ_H  = ($SZ/2)-1;
my $SZ_SQ = $SZ*$SZ;

my $AR    = [0..$SZ_SQ-1];

# ---   *   ---   *   ---
# index into 2D rect

sub sq_idex($x,$y) {
  return $x+($y*$SZ);

};

# ---   *   ---   *   ---
# ^2D coords from index

sub rsq_idex($i) {

  my $y=int($i/$SZ);
  my $x=$i-($y*$SZ);

  return ($x,$y);

};

# ---   *   ---   *   ---

sub ror() {

  my $cpy=[@{$AR}];

  for my $y(0..$SZ_I) {
    for my $x(0..$SZ_I) {
      $AR->[sq_idex($SZ_I-$y,$x)]=
        $cpy->[sq_idex($x,$y)];

    };

  };

};

# ---   *   ---   *   ---

sub rol() {

  my $cpy=[@{$AR}];

  for my $y(0..$SZ_I) {
    for my $x(0..$SZ_I) {
      $AR->[sq_idex($y,$SZ_I-$x)]=
        $cpy->[sq_idex($x,$y)];

    };

  };

};

# ---   *   ---   *   ---

sub draw() {

  for my $y(0..$SZ_I) {
    for my $x(0..$SZ_I) {
      my $idex=sq_idex($x,$y);

      # test reversion works
      ($x,$y)=rsq_idex($idex);
      $idex=sq_idex($x,$y);

      print '|'.(chr(0x21+$AR->[$idex]));

    };

    print "|\n";

  };

};

# ---   *   ---   *   ---

draw();

# ---   *   ---   *   ---
