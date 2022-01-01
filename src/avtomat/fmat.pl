#!/usr/bin/perl

#deps
  use lib "./cash";
  use cash;

# ---   *   ---   *   ---

$line="const whatever=~(99999999999999999923 | 99999999999989998 && 4442222 ^ 00002222,4917897,3029183,4901423); aaaaaaaaaaa";
cash::wrapl $line;
