#ifndef __24_CLI_H__
#define __24_CLI_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Evil.hpp"

// ---   *   ---   *   ---
// info

class Cli {

public:

  VERSION     "v0.00.1b";
  AUTHOR      "IBN-3DILA";

// ---   *   ---   *   ---
// helper

  struct Opt {

    std::string id;

    std::string short_form;
    std::string long_form;

    bool        is_switch;

  };

// ---   *   ---   *   ---

  struct Error {

    CX Evil::Errcode SWITCH={

      .type=AR_ERROR,

      .code=__COUNTER__,
      .mess="Option takes no arguments"

    };

  };

// ---   *   ---   *   ---
// internal helper

private:

  struct Arg {

    std::string name;
    uint64_t    idex;

    bool        on;

    std::vector<std::string> values;

  };

// ---   *   ---   *   ---
// attrs

  uint64_t              m_opt_sz;

  std::vector<Cli::Opt> m_opt;
  std::vector<Cli::Arg> m_have;

// ---   *   ---   *   ---
// guts

  cxi32 NO_MATCH = -1;
  cx64  NPOS     = std::string::npos;

  // get idex of option
  // NO_MATCH on fail
  int is_opt(std::string& s);

  // 0: option is -OPT [value,value]
  // 1: option is -OPT[value,value]
  bool data_in_opt(
    Cli::Arg&    dst,
    Cli::Opt&    opt,

    std::string& arg

  );

  // breaks value,value
  void comma_split(
    Cli::Arg&    dst,
    std::string& s

  );

  // bunch of switches
  int parse_opt(
    int          idex,
    std::string& arg

  );

// ---   *   ---   *   ---
// iface

public:

  Cli(
    const Cli::Opt* opt,
    const uint64_t  opt_sz

  );

  // fill out m_have from argv
  void input(int argc,char** argv);

  // debug
  void prich(int errout=0);

};

// ---   *   ---   *   ---
// sugar

#define CLI \
  Cli cli( \
    OPTIONS, \
    arrsize(OPTIONS) \
  )

// ---   *   ---   *   ---

#endif // __24_CLI_H__
