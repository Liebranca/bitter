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

#  xprt=>[qw(TODO)],
  libs=>[qw(stdc++ m z png)],

  post_build=>q(

    chdir(Shb7::dir('bitter'));
    my $debug=($cli->{debug}!=$NULL)
      ? q[-g] : $NULLSTR;

    my @call=(qw(

      ../avtomat/bin/olink

      -lbitter
      -o../bin/joj-sprite

    ),$debug,

      q[bin/src/joj_sprite.cpp]

    );

    system {$call[0]} @call;

    @call=(qw(

      ../avtomat/bin/olink

      -lbitter
      -o../bin/dafpack

    ),$debug,

      q[bin/src/dafpack.cpp]

    );

    system {$call[0]} @call;

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

  ),

);

Avt::scan();
Avt::config();
Avt::make();

# ---   *   ---   *   ---
1; # ret
