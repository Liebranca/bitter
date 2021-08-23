#ifndef __CH_MANG_H__
#define __CH_MANG_H__

#include "KVRNEL/zjc_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//   ---     ---     ---     ---     ---

int NTCHMNG     (char* title,  \
                 int fullscreen);           // start-kicks chasm
int DLCHMNG     (void          );           // trashes it
int GTCHMNGRUN  (void          );           // checks if window is open

void  FRBEGCHMNG(void          );           // loop top
float FBYCHMNG  (void          );           // get 1*clock
uint  UBYCHMNG  (void          );           // get (1*clock)*base
void  FRENDCHMNG(void          );           // loop bottom
void  SLEEPCHMNG(void          );           // run the clock

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __CH_MANG_H__