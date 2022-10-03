#ifndef __SIN_SHADER_H__
#define __SIN_SHADER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "KVRNEL/zjc_CommonTypes.h"
#include "SHADERS/sin_ShaderParams.h"

//  - --- - --- - --- - --- -

void NTSINSHD (void                     );  // kyknit shader bucket
void DLSINSHD (void                     );  // kykdel shader bucket

uint MKSHD    (const SHDP* params       );  // occupy shader slot
void DLSHD    (uint idex                );  // free shader slot

//   ---     ---     ---     ---     ---

void STPRGM   (uint idex                );  // select program to bind
void RSTSHDLOC(void                     );  // deselect program
int  CHKPRGM  (uint idex                );  // no-rebind check selected program
void BNDPRGM  (void                     );  // bind selected program

//  - --- - --- - --- - --- -

uint CMPSHD   (cchar** source           ,
               uint num_sources         ,
               uint shaderType          );  // shader compile boiler

void CHKSHDERR(uint shader, uint flag   ,
               int isProgram            ,
               const char* errorMessage );  // errcatch shader compilation

//   ---     ---     ---     ---     ---

#ifdef __cplusplus
}
#endif

#endif // __SIN_SHADER_H__
