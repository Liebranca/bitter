#!/usr/bin/perl

#***************************
# FFR                      *
# mini-float lookups       *
#                          *
# LIBRE SOFTWARE           *
# Licensed under GNU GPL3  *
# be a bro and inherit     *
#                          *
# CONTRIBUTORS             *
# lyeb,                    *
#***************************

use Math::Trig;
use Math::Complex;
require './avtomat.pl';

#    ---     ---     ---     ---     ---

$ffr_scale=5;
$ffr_mul=1<<$ffr_scale,
$ffr=1/$ffr_mul;

#    ---     ---     ---     ---     ---

$notice=&AVTO::mknot;
open FH,'+>','../kvrnel/lymath/ffr.h' or die $!;

print FH <<EOF
$notice

#ifndef __LYM_FFR_H__
#define __LYM_FFR_H__

#ifdef __cplusplus
extern \"C\" {
#endif

#include <stddef.h>
EOF
;

#    ---     ---     ---     ---     ---

sub roundf {
  return int sprintf('%.0f', @_[0]);

};

sub byte {
  return '0x' . uc sprintf '%.2x',@_[0];

};

#    ---     ---     ---     ---     ---

sub coolpr {

  my $f=byte @_[0];
  my $limit=@_[1];

  my $c=($i<$limit) ? ',' : '';
  my $b=(!($i%8)) ? "\n  " : '';

  print FH join '',$b,$f,$c;

};

#    ---     ---     ---     ---     ---

print FH <<EOF

static const char ffr_scale=$ffr_scale;
static const char ffr_mul=1<<$ffr_scale;
static const float ffr=1.0f/$ffr_mul.0f;

EOF
;

#    ---     ---     ---     ---     ---

print FH "static const char SC_TABLE[]={\n";

@range=(0..31);
for $i (@range) {
  my $f=roundf(sin(deg2rad(($i*$ffr)*90))*$ffr_mul);
  coolpr $f,31;

};print (FH

  "\n\n};".

  "static const size_t SC_TABLE_SZ=\n".
  "  sizeof(SC_TABLE)/sizeof(SC_TABLE[0])-1;\n\n"

);

#    ---     ---     ---     ---     ---

print FH "static const char RD_TABLE[]={\n";

@range=(0..63);
for $i (@range) {
  my $f=roundf(deg2rad(($i*$ffr)*180)*$ffr_mul);
  coolpr $f,63;

};print(FH
  "\n\n};".
  "static const size_t RD_TABLE_SZ=\n".
  "  sizeof(RD_TABLE)/sizeof(RD_TABLE[0])-1;\n\n"

);

#    ---     ---     ---     ---     ---

print FH "static const char SQ_TABLE[]={\n";

@range=(0..127);
for $i (@range) {
  my $f=roundf(sqrt($i*$ffr)*$ffr_mul);
  coolpr($f,127);

};print(FH
  "\n\n};".

  "static const size_t SQ_TABLE_SZ=\n".
  "  sizeof(SQ_TABLE)/sizeof(SQ_TABLE[0])-1;\n"

);

#    ---     ---     ---     ---     ---

print FH <<EOF

#define mulc(a,b) (((a)*(b))>>ffr_scale)
#define divc(a,b) ((((a)<<ffr_scale)/(b)))
#define pow2c(x) (mulc(x,x))

char wr_sqrtc(char x);
#define sqrtc(x) wr_sqrtc(x)

char wr_sinc(char x);
#define sinc(x) wr_sinc(x)

char wr_cosc(char x);
#define cosc(x) wr_cosc(x)

#ifdef __cplusplus
}
#endif

#endif // __LYM_FFR_H__
EOF
;

close FH;

#    ---     ---     ---     ---     ---

open FH,'+>','../KVRNEL/LYMATH/ffr.c' or die $!;

print FH <<EOF
$notice

#include "ffr.h"
#include <stdlib.h>

#define abs_c(x) char sig=(x<0) ? -1 : 1;x=abs(x)

char wr_sqrtc(char x) { abs_c(x);
  return (1*sig)*SQ_TABLE[(x)&(SQ_TABLE_SZ)];

};

char wr_sinc(char x) { abs_c(x);
  return (1*sig)*SC_TABLE[(x)&(SC_TABLE_SZ)];

};

char wr_cosc(char x) { abs_c(x);
  return (1*sig)*SC_TABLE[(SC_TABLE_SZ)-((x)&(SC_TABLE_SZ))];

};

EOF
;

close FH;

#    ---     ---     ---     ---     ---

$trsh='../../trashcan/';

$dep_s=&AVTO::mkdep(
  "ffr",$trsh."KVRNEL/LYMATH/"

);open FH,'+>',$trsh.'AVTOMAT/ffr.pmk' or die $!;
print FH $dep_s;
close FH;

#    ---     ---     ---     ---     ---
