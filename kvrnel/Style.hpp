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

  #include <vector>
  #include <new>

// ---   *   ---   *   ---
// peso types ;>

typedef void(*nihil)(void);
typedef void(*stark)(void*);

typedef void*(*signal)(void*);

// ---   *   ---   *   ---
// also known as 'do nothing'

static void noop(void) {};

// ---   *   ---   *   ---
// constant types

#define CX static constexpr const

#define cx8   CX uint8_t
#define cx16  CX uint16_t
#define cx32  CX uint32_t
#define cx64  CX uint64_t

#define cxi8  CX int8_t
#define cxi16 CX int16_t
#define cxi32 CX int32_t
#define cxi64 CX int64_t

#define cxr32 CX float
#define cxr64 CX double

#define cxstr CX std::string_view

#define cxfptr const auto

#define VERSION cxstr _VERSION=
#define AUTHOR  cxstr _AUTHOR=

// ---   *   ---   *   ---
// can't override inherited constants?
//
// what kind of mediocre programming cookbook
// does one have to smoke the pages of in
// order to think that is smart in any way,
// shape or form... ?
//
// oh right! this is C++...

#define VIC virtual inline const

#define vic8  VIC uint8_t
#define vic16 VIC uint16_t
#define vic32 VIC uint32_t
#define vic64 VIC uint64_t

#define vicr32 VIC float
#define vicr64 VIC double

#define vicstr VIC std::string_view

cx16 DAFPAGE=0x4000;
cx16 DAFSIZE=256;

// ---   *   ---   *   ---
// syntax dummies; solely for clarity

#define xBYTES(x, type) (sizeof(type)*x)
#define xELEMS(x, type) (x/sizeof(type))

// ---   *   ---   *   ---
// shorthands

#define CASSERT(boo, dummy) \
  static_assert(boo, dummy)

#define arrsize(arr) \
  (sizeof((arr)) / sizeof(*(arr)))

#define reipret(type,value) \
  *((type*) (&value))

#define addrof(x) (&(x))

cx8 AR_DONE  = 0x00;
cx8 AR_ERROR = 0xFE;
cx8 AR_FATAL = 0xFF;

// ---   *   ---   *   ---
// shortening horrible C++ names

typedef std::vector<std::string> strvec;

#define SINGLETON(name) \
  /* this is what idiocy looks like */  \
  static inline name & ice(void) {\
    static name ice; \
    return ice; \
  }; \
  name (name const&)          = delete; \
  void operator=(name const&) = delete

// jesus christ
#define CLINE_SZ \
  std::hardware_destructive_interference_size

#define cline_align alignas(CLINE_SZ)
#define cline_offset(type) \
  CLINE_SZ/sizeof(type)

#define CLINE_ALIGN_CHK(type) \
  cx16 type##_##V_OFFSET=(cline_offset(type)) \
    ? cline_offset(type) \
    : ! (sizeof(type) % CLINE_SZ) \
      ? 1 \
      : 0 \
    ; \
  CASSERT(  \
    type##_##V_OFFSET, \
    "\n\e[37;1m<" \
    "\e[31;21mAR" \
    "\e[37;1m>" \
    "\e[32;1m" #type "\e[0m" \
    " must be cache line aligned\n" \
    "\e[37;1m::\e[0m" \
    "rewrite as \e[35;1mstruct cline_align \e[0m" \
    "" #type " {...};\n" \
    "\e[37;1m::\e[0m" \
    "else adjust struct elements\n" \
  )

// ---   *   ---   *   ---

#endif // __24_STYLE_H__
