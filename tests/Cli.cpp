#include <iostream>
#include "kvrnel/Cli.hpp"

const Cli::Opt OPTIONS[]={

  {"switch","-s","--switch",1},
  {"output","-o","--output",0}

};

int main(int argc,char** argv) {

  errchk {

    CLI;

    cli.input(argc,argv);

    auto& out=cli.have("DATA");

    std::cerr << out.on << std::endl;
    for(auto& s : out.values) {
      std::cerr << s << std::endl;

    };

    cli.prich();

  } endchk;

};
