#ifndef __24_CLI_H__
#define __24_CLI_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"

// ---   *   ---   *   ---
// info

class Cli {

public:

  VERSION     "v0.00.1b";
  AUTHOR      "IBN-3DILA";

// ---   *   ---   *   ---
// helper

  struct Opt {

    std::string name;
    uint64_t    idex;

    std::vector<std::string> values;

  };

// ---   *   ---   *   ---
// attrs

private:

  uint64_t                 m_opt_sz;

  std::vector<std::string> m_opt;
  std::vector<Cli::Opt>    m_have;

// ---   *   ---   *   ---
// guts

  cxi32 NO_MATCH=-1;

  // get idex of option
  // NO_MATCH on fail
  int is_opt(std::string& s);

  // 0: option is -OPT [value,value]
  // 1: option is -OPT[value,value]
  bool data_in_opt(
    Cli::Opt&    dst,
    std::string& arg,
    std::string& opt

  );

  // breaks value,value
  void comma_split(
    Cli::Opt&    dst,
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
    const char** opt,
    uint64_t     opt_sz

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
