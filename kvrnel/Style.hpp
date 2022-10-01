// ---   *   ---   *   ---
// STYLE
// Cool shorthands
//
// LIBRE SOFTWARE
// Licenced under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---

#ifndef __24_STYLE_H__
#define __24_STYLE_H__

// ---   *   ---   *   ---
// deps

  #include <cstdint>
  #include <cstddef>

  #include <string>
  #include <string_view>

// ---   *   ---   *   ---
// peso types ;>

typedef void(*nihil)(void);
typedef void(*stark)(void*);

typedef void*(*signal)(void*);

// ---   *   ---   *   ---
// constant types

#define CX static constexpr const

#define cx8   CX char
#define cx16  CX short
#define cx32  CX int
#define cx64  CX long

#define cxr32 CX float
#define cxr64 CX double

#define cxstr CX std::string_view

#define VERSION cxstr VERSION=
#define AUTHOR  cxstr AUTHOR=

cx16 DAFPAGE=0x4000;
cx16 DAFSIZE=256;

// ---   *   ---   *   ---
// syntax dummies; solely for clarity

#define xBYTES(x, type) (sizeof(type)*x)
#define xELEMS(x, type) (x/sizeof(type))

// ---   *   ---   *   ---

cxr32 FRAC8=1.0f/256.0f;
cxr32 FRAC7=1.0f/128.0f;
cxr32 FRAC6=1.0f/64.0f;
cxr32 FRAC5=1.0f/32.0f;
cxr32 FRAC4=1.0f/16.0f;
cxr32 FRAC3=1.0f/8.0f;
cxr32 FRAC2=1.0f/4.0f;
cxr32 FRAC1=1.0f/2.0f;

cxr32 FRACL_F[]={

  FRAC1,FRAC2,FRAC3,FRAC4,
  FRAC5,FRAC6,FRAC7,FRAC8

};

cx32  FRACL_I[]={2,4,8,16,32,64,128,256};
cx32  FRACL_B[]={1,2,3,4,5,6,7,8};

// ---   *   ---   *   ---
// shorthands

#define CASSERT(boo, dummy) \
  static_assert(boo, dummy)

#define arrsize(arr) \
  sizeof((arr)) / sizeof(*(arr))

#define addrof(x) (&(x))

cx8 AR_DONE  = 0x00;
cx8 AR_ERROR = 0xFE;
cx8 AR_FATAL = 0xFF;

// ---   *   ---   *   ---

#endif // __24_STYLE_H__
