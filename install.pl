#!/usr/bin/perl

  use v5.36.0;
  use strict;
  use warnings;

  use lib $ENV{'ARPATH'}.'/lib/sys/';
  use Shb7;

  use lib $ENV{'ARPATH'}.'/lib/';
  use Avt;

# ---   *   ---   *   ---

Avt::set_config(

  name=>'bitter',
  scan=>'-x legacy',

  build=>'ar:bitter',

#  xprt=>[qw(TODO)],
  libs=>[qw(stdc++ m)],

#  post_build=>q(
#
#    use Emit::Std;
#    use Emit::Python;
#
#    Emit::Std::outf(
#
#      'Python','lib/JOJ.py',
#
#      author=>'IBN-3DILA',
#      include=>[['Avt.cwrap','*']],
#
#      body=>\&Emit::Python::shwlbind,
#      args=>['JOJ',['bitter']],
#
#    );
#
#  ),

);

Avt::scan();
Avt::config();
Avt::make();

# ---   *   ---   *   ---
1; # ret
