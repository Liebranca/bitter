#ifndef __24_CLI_H__
#define __24_CLI_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Evil.hpp"
  #include "kvrnel/Tab.hpp"

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

  struct Arg {

    bool   on;
    strvec values;

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
// attrs

private:

  uint64_t m_opt_sz;

  std::vector<Cli::Opt> m_opt;
  std::vector<Cli::Arg> m_have;

  Tab<std::string,uint64_t> m_tab;

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

  // get value of option
  Cli::Arg& have(std::string key);

  // debug
  void prich(int errout=0);

};

// ---   *   ---   *   ---
// sugar

#define OPTIONS \
  const Cli::Opt _OPTIONS[]=

#define CLI \
  Cli cli( \
    _OPTIONS, \
    arrsize(_OPTIONS) \
  );\
  cli.input(argc,argv)

// ---   *   ---   *   ---

#endif // __24_CLI_H__
