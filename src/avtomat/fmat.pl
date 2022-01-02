#!/usr/bin/perl

#deps
  use strict;
  use warnings;

  use lib "./cash";
  use cash;

# ---   *   ---   *   ---

sub test {
  my $line="const whatever=~(99999999999999999923 | 99999999999989998 && 4442222 ^ 00002222,4917897,3029183,4901423); aaaaaaaaaaa";
  cash::wrapl $line;

};test;
