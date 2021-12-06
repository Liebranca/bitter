#ifndef __CH_MANG_H__
#define __CH_MANG_H__

#include "KVRNEL/zjc_CommonTypes.h"

#ifdef __cplusplus
extern"C" {

#endif

//   ---     ---     ---     ---     ---
// #:0x0;>

// start-kicks chasm
int NTCHMNG(char* title,int fullscreen);

int DLCHMNG(void);          // trashes it
int GTCHMNGRUN(void);       // check window  open

int FRBEGCHMNG(int uptdate);// loop top
float FBYCHMNG(void);       // get 1*clock
uint UBYCHMNG(void);        // get (1*clock)*base
void FRENDCHMNG(void);      // loop bottom
void SLEEPCHMNG(void);      // run the clock

uchar* IBUFCHMNG(void);     // get textinput buffer

//   ---     ---     ---     ---     ---
// #:0x1;>


#ifdef __cplusplus
}
#endif

#endif                      // __CH_MANG_H
