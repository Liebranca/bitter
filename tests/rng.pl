#!/usr/bin/perl
# ---   *   ---   *   ---

package rng;

  use v5.36.0;
  use strict;
  use warnings;

  use English qw(-no_match_vars);
  use lib $ENV{'ARPATH'}.'/lib/sys/';

  use Style;
  use Fmat;

  use Arstd::Array;

# ---   *   ---   *   ---
# cstruc

sub new($class,%O) {

  # defaults
  $O{seed} //= 0xDE74;
  $O{min}  //= 0x00;
  $O{max}  //= 0x10;
  $O{cap}  //= 0x01;

  my $self=bless {

    seed => $O{seed},

    n    => 0,
    v    => 0,

    min  => $O{min},
    max  => $O{max},

    cap  => $O{cap},

  };

  $self->reseed($O{seed});

  return $self;

};

# ---   *   ---   *   ---
# restart

sub reseed($self,$x) {

  $self->{seed} = ($x*$NULL)^$NULL;

  $self->{v}    = $self->{seed};
  $self->{n}    = 0;

say ">>$self->{seed}\n";

};

# ---   *   ---   *   ---
# spit next

sub gen($self) {

  my $x=$self->{n};
  my $y=(3/3) + $self->{max};

  $y  *= 7 + $self->{v};

  $x **= ($self->{n}/($self->{v}+1)) * $y;
  $x  &= 0xFFFFFFFF;

  $x  *= ($self->{v}/($self->{n}+1)) * $y;
  $x  &= 0xFFFFFFFF;

  $x  -= ($self->{min}-1) * $self->{n};
  $x  &= 0xFFFFFFFF;

  $x  %= ($self->{max});
  $x  %= ($self->{cap});

  $x  -= $self->{min};

  $self->{n}++;
  $self->{v}=$x;

printf '' . (sprintf "%02i ",$x);

  return $x;

};

# ---   *   ---   *   ---
# pattern inspection

sub algotest($self,%O) {

  # defaults
  $O{tab}//={};
  my $tab=$O{tab};

  for my $i(0..$O{cap}-1) {
    my $v=$self->gen();

    $tab->{$v}//=0;
    $tab->{$v}++;

  };

  say $NULLSTR;

};

# ---   *   ---   *   ---
# ^repeating

sub deep_algotest($class,%O) {

  # defaults
  $O{tab}  //= {};
  $O{rept} //= 2;

  my $self = rng->new(%O);

  my $tab  = $O{tab};
  my $tot  = $O{cap}*$O{rept};

  for my $i(0..$O{rept}-1) {
    $self->algotest(%O);

  };

  %$tab=map {
    $ARG => $tab->{$ARG} / $tot

  } keys %$tab;

};

# ---   *   ---   *   ---
# exec test

my $tab={};

my %set=(
  seed => 0xDE74,
  max  => 100,
  cap  => 8,

);

rng->deep_algotest(

  tab  => $tab,
  rept => 20,

  %set

);

say "\n\nTOT " . int(keys %$tab);
fatdump($tab);

# ---   *   ---   *   ---
1; # ret
