from math import *
from avtomat import mknot;

ffr_scale=5;
ffr=1/(1<<ffr_scale);

#    ---     ---     ---     ---     ---

print(

  f"{mknot()}\n"

  "#ifndef __LYM_FFR_H__\n"\
  "#define __LYM_FFR_H__\n\n"\

  "#ifdef __cplusplus\n"\
  "extern \"C\" {\n"
  "#endif\n\n"\

  "#include <stddef.h>\n\n"

)

#    ---     ---     ---     ---     ---

def coolpr(f,limit):

  f=hex(f);f=(
    f[:2]
   +('0' if len(f[2:])<2 else '')
   +(f[2:].upper())

  );

  c=',' if i<limit else '';
  b='\n  ' if not (i%8) else '';

  print(f"{b}{f}{c}",end='');

#    ---     ---     ---     ---     ---

print(

  f"\nstatic const char ffr_scale={ffr_scale};\n"
  f"\nstatic const char ffr_mul=1<<{ffr_scale};\n"
  "static const float ffr="\
  "1.0f/((float) ffr_mul);\n"

);

#    ---     ---     ---     ---     ---

print("static const char SC_TABLE[]={");

for i in range(32):
  f=int(sin(radians((i*ffr)*90))*32);
  coolpr(f,31);

print("\n\n};")
print(
  "static const size_t SC_TABLE_SZ="\
  "sizeof(SC_TABLE)/sizeof(SC_TABLE[0])-1;\n"

);

#    ---     ---     ---     ---     ---

print("static const char RD_TABLE[]={");

for i in range(64):
  f=int(radians((i*ffr)*180)*32);
  coolpr(f,63);

print("\n\n};");
print(
  "static const size_t RD_TABLE_SZ="\
  "sizeof(RD_TABLE)/sizeof(RD_TABLE[0])-1;\n"

);

#    ---     ---     ---     ---     ---

print("static const char SQ_TABLE[]={");

for i in range(128):
  f=int(sqrt(i*ffr)*32);
  coolpr(f,127);

print("\n\n};");
print(
  "static const size_t SQ_TABLE_SZ="\
  "sizeof(SQ_TABLE)/sizeof(SQ_TABLE[0])-1;\n"

);

#    ---     ---     ---     ---     ---

print(

  "#define mulc(a,b) (((a)*(b))>>ffr_scale)\n"\
  "#define divc(a,b) ((((a)<<ffr_scale)/(b)))\n"\
  "#define pow2c(x) (mulc(x,x))\n"\

  "#define sqrtc(x) SQ_TABLE[(x)&(SQ_TABLE_SZ)]\n"\

  "#define sinc(x) SC_TABLE[(x)&(SC_TABLE_SZ)]\n"\
  "#define cosc(x) SC_TABLE[(SC_TABLE_SZ)-((x)&(SC_TABLE_SZ))]\n"

);

#    ---     ---     ---     ---     ---

print(
  "#ifdef __cplusplus\n\n"\
  "}\n"
  "#endif\n"\

  "#endif // __LYM_FFR_H__\n"

);