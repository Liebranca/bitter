#ifndef __24_EVIL_H__
#define __24_EVIL_H__

// ---   *   ---   *   ---
// deps

  #include <string>
  #include <vector>

  #include "kvrnel/Style.hpp"

// ---   *   ---   *   ---
// zero if not set at compile-time

  #ifndef AR_DEBUG
  #define AR_DEBUG 0x00
  #endif

// ---   *   ---   *   ---

static std::string hexstr(
  uint64_t x,
  uint8_t  sz

);

// ---   *   ---   *   ---
// info

class Evil {

public:

  VERSION     "v2.01.0";
  AUTHOR      "IBN-3DILA";

  cx16 FLEN_TGT=64;
  cx16 CSTK_LEN=64;
  cx16 ERRS_LEN=255;

  struct Errcode {

    const uint8_t          type;

    const uint64_t         code;
    const std::string_view mess;

  };

// ---   *   ---   *   ---
// records info for calls that __can__ fail

private:

  struct Errme {

    uint64_t    depth;
    uint64_t    line;

    std::string loc;
    std::string file;
    std::string func;

    std::string info;

  };

  typedef std::vector<Evil::Errme> Errlog;

// ---   *   ---   *   ---
// attrs

  uint64_t     m_depth;

  uint64_t     m_ptr;
  uint64_t     m_state;

  Evil::Errlog m_log;

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

  // manage trace
  void printlog(bool flush);

  // gives color matching type of error
  uint8_t errtype(const Evil::Errcode& ee);

  // errthrows on the errthrower
  void on_max_depth(Evil::Errme& me);

  // get next entry in tree
  std::string get_callbranch(
    Evil::Errme& me

  );

// ---   *   ---   *   ---
// """iface"""

public:

  // ctrash
  Evil(void) {};
  ~Evil(void) {};

  // gets error location
  void geterrloc (
    const char* file,
    const char* func,

    uint64_t    line

  );

  // makes filename nicer to read
  char* shpath (const char* path);

  void set_err(const std::string& info);
  Evil::Errme& top_err(void);

  // goes back in time to save john connor
  void terminator(const Evil::Errcode& ee);

  static Evil& get(void);
  void poplog(void);

};

// ---   *   ---   *   ---
// needed for proper expansion

#define errchk \
  Evil::get().geterrloc(\
    Evil::get().shpath(__FILE__),__func__,__LINE__\
  );try

#define err(ee,info) \
  Evil::get().set_err(info);\
  throw ee

#define endchk \
  catch(Evil::Errcode ee) {\
    Evil::get().terminator(ee);\
  };Evil::get().poplog()

// ---   *   ---   *   ---

#endif // __24_EVIL_H__
