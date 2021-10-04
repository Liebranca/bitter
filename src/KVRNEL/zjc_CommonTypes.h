/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/
/*    ZJC_COMMONTYPES                       *
*                                           *
*     -only header worthy of this notice    *
*                                           */
// *   ---     ---     ---     ---     ---  *
/*    LIBRE SOFTWARE                        *
*                                           *
*     Licenced under GNU GPL3               *
*     be a bro and inherit                  *
*                                           */
// *   ---     ---     ---     ---     ---  *
/* CONTRIBUTORS                             *
*     lyeb,                                 *
*                                           */
/*/*//*//*//*//*//*//*//*//*//*//*//*//*//*/*/

#ifndef __ZJC_COMMONTYPES_H__
#define __ZJC_COMMONTYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#define ZJC_DAFPAGE             0x4000
#define ZJC_DAFSIZE                256

//  - --- - --- - --- - --- -               // syntax dummies; solely for clarity

#define xBYTES(x, type) (sizeof(type)*x)
#define xELEMS(x, type) (x/sizeof(type))

//  - --- - --- - --- - --- -

#define EXPORT __declspec(dllexport)        // find a better place for this

//  - --- - --- - --- - --- -

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

#define FRAC8 1.0f / 256.0f
#define FRAC7 1.0f / 128.0f
#define FRAC6 1.0f /  64.0f
#define FRAC5 1.0f /  32.0f
#define FRAC4 1.0f /  16.0f
#define FRAC3 1.0f /   8.0f
#define FRAC2 1.0f /   4.0f
#define FRAC1 1.0f /   2.0f

static const float FRACL_F[] =              { FRAC1, FRAC2, FRAC3, FRAC4,
                                              FRAC5, FRAC6, FRAC7, FRAC8    };

static const uint  FRACL_I[] =              {    2,      4,      8,    16,
                                                32,     64,    128,   256   };

//  - --- - --- - --- - --- -

long hexstr_tolong (char* v);
long chexstr_tolong(cchar* v);

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_COMMONTYPES_H__
