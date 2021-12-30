#!/usr/bin/perl

#***************************
# CALOUT                   *
# generates debug printer  *
#                          *
# LIBRE SOFTWARE           *
# Licensed under GNU GPL3  *
# be a bro and inherit     *
#                          *
# CONTRIBUTORS             *
# lyeb,                    *
#***************************

require './avtomat.pl';

#    ---     ---     ---     ---     ---

open FH,'+>','../KVRNEL/calout.h' or die $!;

$notice=&AVTO::mknot;

print FH <<EOF
$notice

#ifndef __CALOUT_H__
#define __CALOUT_H__

#ifdef __cplusplus
extern "C" {
#endif
EOF
;

#    ---     ---     ---     ---     ---

@fams=split '','MFKERHPS';
$base_form=<<EOF;

#if KVR_DEBUG & KVR_CALO!
  #ifndef CALOUT_!
  #define CALOUT_!(calcall) calcall
  #endif

#else
  #ifndef CALOUT_!
  #define CALOUT_!(calcall) asm("nop")
  #endif

#endif
EOF
;

for my $c (@fams) {

  my $s=$base_form;$s=~s/!/$c/ig;
  print FH "$s\n";

};print FH <<EOF

#ifndef STRINGIFY
#define STRINGIFY(x) #x
#endif

#ifndef CHARIFY
#define CHARIFY(x) (char) CHARIFY(x)[0]
#endif

#define CALOUT(fam, format, ...) \
  CALOUT_##fam(__calout(CHARIFY(fam),format,__VA_ARGS__))

#ifdef __cplusplus
}
#endif

#endif // __CALOUT_H__
EOF
;

close FH;

#    ---     ---     ---     ---     ---
