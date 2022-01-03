#!/usr/bin/perl

#deps
  use strict;
  use warnings;

  use lib "./cash";
  use cash;

# ---   *   ---   *   ---

sub test {
#  my $line="const whatever=~(99999999999999999923 | 99999999999989998 && 4442222 ^ 00002222,4917897,3029183,4901423); aaaaaaaaaaa";
#  cash::wrapl $line;

my $pe='
$:%peso;>
  $:%pal user;>
  0 FF0000
  1 00FF00
  $:/pal;>
$:pal user;>
$:col 01;>
user-defined palete, color 0x01
$:pal def;>
$:col 04;>
default palete, color 0x04
$:/peso;>
';


  my $s=cash::pe_rdin($pe);
  $s=substr $s,0,(length $s);
  print "$s\n";

};test;
1;
