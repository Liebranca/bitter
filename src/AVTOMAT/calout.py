"""*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    MKCALOUT                              *
*                                           *
*     -macro-generator script               *
*                                           */
// *   ---     ---     ---     ---     ---  *
/*    LIBRE SOFTWARE                        *
*                                           *
*     Licenced under GNU GPL3               *
*     be a bro and inherit                  *
*                                           */
// *   ---     ---     ---     ---     ---  *
/* CONTRIBUTORS                             *
*     lyeb,                                 *
*                                           */
/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/"""

from avtomat import mknot;

#   ---     ---     ---     ---     ---

fams="MFKERHPS";
base_form=(
"""#if KVR_DEBUG & KVR_CALO!
#define CALOUT_!(format, ...) __calout('!', format, ##__VA_ARGS__)
#else
#define CALOUT_!(format, ...) asm("nop")
#endif
"""
);

#   ---     ---     ---     ---     ---

fstr=mknot();

for fam in fams:
  fstr=fstr+base_form.replace('!', fam);

fstr=fstr+"#define CALOUT(fam, format, ...) ";
fstr=fstr+"CALOUT_##fam(format, ##__VA_ARGS__)\n\n";

print(fstr);

#   ---     ---     ---     ---     ---