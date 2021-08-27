#ifndef __SIN_H__
#define __SIN_H__
#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

void NTSIN  (void                  );
void DLSIN  (void                  );

void GTCHRSZ(float* dst, uint chsiz);       // calculates percent of screen used by square

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SIN_GLOBALS_H__