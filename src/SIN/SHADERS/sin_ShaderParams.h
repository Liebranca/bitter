#ifndef __SIN_SHADER__PARAMS_H__
#define __SIN_SHADER__PARAMS_H__

#include "KVRNEL/zjc_CommonTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SIN_SBFTRANSFORMA 0x01
#define SIN_SBFTRANSFORMB 0x02
#define SIN_SBFAMBIENT    0x04
#define SIN_SBFLIGHTING   0x08

typedef struct SIN_SHADER_PARAMS {

    cchar** source_v;
    cchar** source_f;

    cchar*  attribs     [ 8];
    cchar*  uniforms    [16];
    cchar*  ubos        [ 4];
    cchar*  samplers    [ 4];

    uchar   num_vsources;
    uchar   num_fsources;
    uchar   num_attribs;
    uchar   num_uniforms;
    uchar   num_ubos;
    uchar   num_samplers;
    ushort  flags;

} SHDP;

#ifdef __cplusplus
}
#endif

#endif // __SIN_SHADER__PARAMS_H__
