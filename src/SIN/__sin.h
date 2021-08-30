#ifndef __SIN_H__
#define __SIN_H__
#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

void NTSIN  (void                  );
void DLSIN  (void                  );

void GTCHRSZ(float* dst, uint chsiz);       // calculates percent of screen used by square
void GTSCRSZ(uint* dst, uint chsiz );       // calculates how many chars fit in screen

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SIN_GLOBALS_H__