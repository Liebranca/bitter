// ---   *   ---   *   ---
// CLI
// Command-line interface
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include <iostream>
  #include "kvrnel/Cli.hpp"

// ---   *   ---   *   ---
// nit

Cli::Cli(
  const Cli::Opt* opt,
  const uint64_t  opt_sz

) {

  m_opt_sz=opt_sz;

  m_opt.resize(m_opt_sz);
  m_have.resize(m_opt_sz);

  for(uint64_t i=0;i<m_opt_sz;i++) {

    m_opt[i]=opt[i];
    m_opt[i].long_form+='=';

    m_have[i].name=m_opt[i].id;
    m_have[i].idex=i;

  };

};

// ---   *   ---   *   ---
// breaks value,value

void Cli::comma_split(
  Cli::Arg&    dst,
  std::string& s

) {

  uint64_t pos=NPOS;

  while((pos=s.find(","))!=NPOS) {

    dst.values.push_back(
      s.substr(0,pos)

    );

    s=s.substr(pos+1);
    s=(s.length()) ? s : "";

  };

  if(s.length()) {
    dst.values.push_back(s);

  };

};

// ---   *   ---   *   ---
// 0: option is -OPT [value,value]
// 1: option is -OPT[value,value]

bool Cli::data_in_opt(

  Cli::Arg&    dst,
  Cli::Opt&    opt,

  std::string& arg

) {

  bool out   = false;
  auto a_len = arg.length();

  if(a_len) {

    if(opt.is_switch) {
      err(Cli::Error::SWITCH,opt.id);

    };

    this->comma_split(dst,arg);
    out=true;

  };

  return out;

};

// ---   *   ---   *   ---
// string is an option

int Cli::is_opt(std::string& s) {

  int      out = NO_MATCH;
  uint64_t pos = NPOS;

  for(int i=0;i<m_opt_sz;i++) {
    for(int j=0;j<2;j++) {
      auto form=(j)
        ? m_opt[i].short_form
        : m_opt[i].long_form
        ;

      pos=s.rfind(form,0);

      if(pos==0) {
        m_have[i].on=true;
        out=i;

        s=s.substr(form.length());

        break;

      } else {
        pos=NPOS;

      };

    };

    if(pos!=NPOS) {
      break;

    };

  };

  return out;

};

// ---   *   ---   *   ---
// bunch of switches

int Cli::parse_opt(
  int          idex,
  std::string& arg

) {

  int out=NO_MATCH;

  // this entry is data
  if(idex==NO_MATCH) {
    this->comma_split(m_have[idex],arg);

  // next entry is data
  } else if(!this->data_in_opt(
    m_have[idex],
    m_opt[idex],

    arg

  )) {out=idex;};

  return out;

};

// ---   *   ---   *   ---
// fill out m_have from argv

void Cli::input(int argc,char** argv) {

  int next=NO_MATCH;

  for(int i=1;i<argc;i++) {
    std::string arg(argv[i]);

    // arg is data
    if(next!=NO_MATCH) {
      this->comma_split(m_have[next],arg);
      next=NO_MATCH;

    // check arg is option
    } else {
      int idex = this->is_opt(arg);
      next     = this->parse_opt(idex,arg);

    };

  };

};

// ---   *   ---   *   ---
// debug

void Cli::prich(int errout) {

  auto& out=(errout) ? std::cerr : std::cout;

  for(uint64_t i=0;i<m_opt_sz;i++) {

    out
    << m_have[i].idex << ": "
    << m_have[i].name << " "
    << m_have[i].on   << " "
    ;

    for(auto& s : m_have[i].values) {

      out << s;
      if(s!=m_have[i].values.back()) {
        out << '|';

      };

    };

    out << std::endl;

  };

};

// ---   *   ---   *   ---
