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

// ends chasm
int DLCHMNG(void);


int GTRUN(void);            // check exit condition

int FRBEG(int uptdate);     // loop top
float FBY(void);            // get 1*clock
uint UBY(void);             // get (1*clock)*base
void FREND(void);           // loop bottom
void FRSLP(void);           // get deltas and sleep

uchar* GTIBUF(void);        // get textinput buffer
void CLIBUF(void);          // clean it up

//   ---     ---     ---     ---     ---
// #:0x1;>


#ifdef __cplusplus
}
#endif

#endif                      // __CH_MANG_H
