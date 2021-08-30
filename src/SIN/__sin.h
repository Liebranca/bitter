#ifndef __SIN_H__
#define __SIN_H__
#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

void NTSIN  (void                  );
void DLSIN  (void                  );

//   ---     ---     ---     ---     ---

void GTCHRGRD(uint chrsz          );        // gets dimentions of chargrid for new screen size

void GTCHRSZ (float* dst, uint mul);        // calculates percent of screen used by square
void GTSCRSZ (uint* dst,  uint mul);        // calculates how many chars fit in screen

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SIN_GLOBALS_H__