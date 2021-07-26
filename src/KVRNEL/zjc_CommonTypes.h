/*   ---     ---     ---     ---     ---    *
 *  ZJC_COMMONTYPES.H                       *
 *                                          *
 *  -typedefs for fixed-sized types         *
 *  -definition of FRAC constants           *
 *  -some type conversion                   *
 *                                          *
 *   ---     ---     ---     ---     ---    */

#ifndef __ZJC_COMMONTYPES_H__
#define __ZJC_COMMONTYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ZJC_DAFPAGE           0x4000
#define ZJC_DAFSIZE              256

#define EXPORT __declspec(dllexport)        /* find a better place for this */

typedef                 int8_t   schar;     // signed  8-bit
typedef                 int16_t  sshort;    // signed 16-bit
typedef                 int32_t  sint;      // signed 32-bit
typedef                 int64_t  slong;     // signed 64-bit

typedef                 uint8_t  uchar;     // unsigned  8-bit
typedef                 uint16_t ushort;    // unsigned 16-bit
typedef                 uint32_t uint;      // unsigned 32-bit
typedef                 uint64_t ulong;     // unsigned 64-bit

typedef const           int8_t   cchar;     // const signed  8-bit
typedef const           int16_t  cshort;    // const signed 16-bit
typedef const           int32_t  cint;      // const signed 32-bit
typedef const           int64_t  clong;     // const signed 64-bit

typedef const           uint8_t  cuchar;    // const unsigned  8-bit
typedef const           uint16_t cushort;   // const unsigned 16-bit
typedef const           uint32_t cuint;     // const unsigned 32-bit
typedef const           uint64_t culong;    // const unsigned 64-bit

//  - --- - --- - --- - --- -

typedef struct ZJC_FLOAT_PAIR   { float x; float y; } fpair;
typedef struct ZJC_INT_PAIR     { int x;   int   y; } ipair;

//  - --- - --- - --- - --- -

static const float FRAC8 = 1.0f / 256;
static const float FRAC7 = 1.0f / 128;
static const float FRAC6 = 1.0f /  64;
static const float FRAC5 = 1.0f /  32;
static const float FRAC4 = 1.0f /  16;

//  - --- - --- - --- - --- -

long hexstr_tolong (char* v);
long chexstr_tolong(cchar* v);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_COMMONTYPES_H__
