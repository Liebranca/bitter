#ifndef __24_EVIL_H__
#define __24_EVIL_H__

#include "Style.hpp"

// ---   *   ---   *   ---
// zero if not set at compile-time

#ifndef AR_DEBUG
#define AR_DEBUG 0x00

#endif

// ---   *   ---   *   ---
// errcode list: kills process

namespace Fatal {

  enum {

    OOM=0,
    ACV,

    END_TIMES,
    ILLEGAL,

    ZLIB,
    ARCHIVE_FULL,

    ERRCODE_MAX

  };

};

// ---   *   ---   *   ---
// errcode list: warn rather than kill

namespace Error {

  enum {

    OPEN_FAIL=Fatal::ERRCODE_MAX,

    CLOSE_FAIL,
    WRITE_FAIL,
    READ_FAIL,

    BAD_SIG,
    UNLINK_FAIL,

    FULL_STACK,
    EMPTY_STACK,

    HASH_INSERT,
    HASH_FETCH,

    ERRCODE_MAX

  };

};

// ---   *   ---   *   ---
// records info for calls that __can__ fail

typedef struct {

  char loc[256]; // line that made the call

  int  depth;    // keep it low
  int  state;    // 0 on 'all OK'

} Dang;

// ---   *   ---   *   ---
// info

namespace Evil {

  VERSION     "v2.00.1";
  AUTHOR      "IBN-3DILA";

// ---   *   ---   *   ---
// debug master

  static int m_depth;   // call depth counter

  static int m_ptr;     // current idex
  static int m_state;   // global errstate

  static Dang m_log[64]; // call dump

  cx32   FLEN_TGT=64;    // file-shorten target

// ---   *   ---   *   ---
// pretty colors ;>

  cxstr PALETTE[]={

    "\e[36;1;40;25m",
    "\e[32;1;40;25m",
    "\e[33;1;40;25m",
    "\e[33;22;40;25m",

    "\e[37;22;40;25m",
    "\e[37;1;40;25m",

    "\e[31;1;40;25m",
    "\e[35;1;40;25m",

  };

// ---   *   ---   *   ---

  // makes filename nicer to read
  char* shpath (const char* path);

  // manage trace
  void printlog(int flush);
  void poplog(void);

  // goes back in time to save john connor
  void terminator(
    int errcode,
    std::string info

  );

// ---   *   ---   *   ---

  // gets error location
  Dang* geterrloc (
    const char* p,
    const char* f,

    int l

  );

  // entry point, so to speak
  void errchk(
    Dang* call,

    int*  retx,

    const short errcode,
    std::string info

  );

};

// ---   *   ---   *   ---
// needed for proper expansion

#define arcatch(func,retx,errcode,info) {\
  Dang* e_call=Evil::geterrloc(\
    __FILE__,__func__,__LINE__\
  );e_call->state=func;\
  Evil::errchk(e_call,&retx,errcode,info);\
}

// ---   *   ---   *   ---

#endif // __24_EVIL_H__
