#ifndef __SIN_GLOBALS_H__
#define __SIN_GLOBALS_H__

#include "KVRNEL/zjc_CommonTypes.h"
#include "KVRNEL/TYPES/zjc_id.h"
#include "KVRNEL/TYPES/zjc_hash.h"

#include "glad/glad.h"

#ifdef __cplusplus
extern"C" {

#endif

//   ---     ---     ---     ---     ---
// #:0x0;>


#define SIN_TEXNIDS 64
#define SIN_SHDNIDS 4

#define SIN_M3DNIDS 128
#define SIN_ANSNIDS 32
#define SIN_HASHSIZ 6

#define SIN_MSHBSZ 0x1000

#define SIN_TEXLAYS 4
#define SIN_SHDLAYS 2

#define SIN_GL_VER "#version 460\n"

extern uint SIN_EVILSTATE;
extern uint SIN_WSIZX;
extern uint SIN_WSIZY;

enum SIN_UNIFORM_MODELS {

SIN_LYTM_U,
SIN_NMAT_U,
SIN_NUM_U

};

//   ---     ---     ---     ---     ---
// #:0x1;>


void NTSINHASH(void);       // kiknit
void DLSINHASH(void);       // kikdel

int STSINHASH(ID* data);    // insert data into table

//   ---     ---     ---     ---     ---
// #:LOOKUPS;>
// if pop, removes data from hash

// look for id matching data and put it in dst
int GTSINHASH(
  ID* data,
  void** dst,
  int pop

);

// look for id matching key and put it in dst
int FNDSINHASH(
  char* key,
  void** dst,
  int pop

);

//   ---     ---     ---     ---     ---
// #:0x2;>


#ifdef __cplusplus
}
#endif

#endif // __SIN_GLOBALS_H__
