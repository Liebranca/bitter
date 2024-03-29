#ifndef __MA_INS_H__
#define __MA_INS_H__

#include "KVRNEL/zjc_CommonTypes.h"
#include "ma_boxes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

void ldins (uint loc     );                 // load instruction and run it
void lmpush(uint loc     );                 // queue instruction
void lmpop (void         );                 // exec next instruction in queue

void lmcpy (void         );                 // a=b
void lmmov (void         );                 // a=b; b^=b
void lmwap (void         );                 // c=a; a=b; b=c
void lmwed (void         );                 // set typing rules

void lmjmp (void         );                 // jump to label
void lmjif (void         );                 // jump if
void lmeif (void         );                 // jump if not
void lmexit(void         );                 // exit program

void lmadd (void         );                 // a+=b
void lmsub (void         );                 // a-=b
void lminc (void         );                 // a++
void lmdec (void         );                 // a--
void lmmul (void         );                 // a*=b
void lmdiv (void         );                 // a/=b
void lmmod (void         );                 // a%=b

void lmand (void         );                 // a&=b
void lmor  (void         );                 // a|=b
void lmxor (void         );                 // a^=b
void lmnor (void         );                 // a=!(a|b)
void lmnand(void         );                 // a=!(a&b)
void lmxnor(void         );                 // a=!(a^b)

void lmtil (void         );                 // a=~a
void lmcl  (void         );                 // empty a; a^=a
void lmclm (void         );                 // a&=~b
void lmnot (void         );                 // a=!a; flags &=~a==0

void lmshr (void         );                 // a>>b
void lmshl (void         );                 // a<<b

void lmlis (void         );                 // local vars && aliases

void lmbuf (void         );                 // set current stream/buffer
void lmptr (void         );                 // get stream/buff cur
void lmmal (void         );                 // open stream/allocate
void lmral (void         );                 // rename stream/reallocate
void lmfre (void         );                 // close stream/deallocate
void lmsow (void         );                 // write a to current stream/buffer
void lmreap(void         );                 // do a on current stream/buffer
void lmkin (void         );                 // keyboard input

void lmtty (void         );                 // set tty flags

void lmcall(void         );                 // jump to label (and go back on ret)
void lmret (void         );                 // go back to last call

void lmasl (uint* udr    );                 // arithmetics solver

//   ---     ---     ---     ---     ---

void swcpy (void         );                 // flip switches for instruction sw***
void swmov (void         );
void swwap (void         );
void swwed (void         );

void swjmp (void         );
void swjif (void         );
void sweif (void         );
void swexit(void         );

void swadd (void         );
void swsub (void         );
void swinc (void         );
void swdec (void         );

void swmul (void         );
void swdiv (void         );
void swmod (void         );
void swand (void         );

void swor  (void         );
void swxor (void         );
void swnor (void         );
void swnand(void         );
void swxnor(void         );

void swtil (void         );
void swcl  (void         );
void swclm (void         );
void swnot (void         );

void swshr (void         );
void swshl (void         );

void swlis (void         );

void swbuf (void         );
void swptr (void         );
void swmal (void         );
void swral (void         );
void swfre (void         );
void swsow (void         );
void swreap(void         );
void swkin (void         );

void swtty (void         );

void swcall(void         );
void swret (void         );

//   ---     ---     ---     ---     ---

void stentry(void        );                 // set program entry point

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __MA_INS_H__