#!/usr/bin/perl

#deps
  use strict;
  use warnings;

  use lib "./cash";
  use cash;

# ---   *   ---   *   ---

# this is for later use somewhere else
# please ignore it
#
#ReadMode 0;
#ReadMode 3;
#
#my $c=undef;while(1) {
#  while(!( $c=ReadLine -1 )){;}
#
#  $c=~ s/(.)/sprintf '%.02x',ord $1/seg;
#  $c=lc $c;
#
#  print "$c\n";
#  if(ord $c==ord "q") {
#    print "Yeah\n";
#
#  };last;
#  
#};
#
#ReadMode 0;

# run test file
my $pe=`cat ./cash/test.px`;
my $s=cash::pe_rdin($pe);
print $s;

# ---   *   ---   *   ---
1; # ret
