#!/usr/bin/perl

  use v5.36.0;
  use strict;
  use warnings;

  use lib $ENV{'ARPATH'}.'/lib/sys/';
  use Style;
  use Shb7;

  use lib $ENV{'ARPATH'}.'/lib/';
  use Avt;

# ---   *   ---   *   ---

Avt::set_config(

  name=>'bitter',
  scan=>'-x legacy,bin',

  build=>'ar:bitter',

  libs=>[qw(stdc++ m z png)],

  utils=>{

    q[bincmp]=>[qw(bin/src/bincmp.cpp)],

    q[joj-sprite]=>[qw(bin/src/joj_sprite.cpp)],
    q[unjoj]=>[qw(bin/src/unjoj.cpp)],

    q[daf]=>[qw(bin/src/daf.cpp)],
    q[undaf]=>[qw(bin/src/undaf.cpp)],

    q[bmesh2crk]=>[qw(bin/src/bmesh2crk.cpp)],

  },

#  xprt=>[qw(TODO)],
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
