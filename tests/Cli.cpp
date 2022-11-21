#include "kvrnel/Cli.hpp"

const Cli::Opt OPTIONS[]={

  {"switch","-s","--switch",1},
  {"output","-o","--output",0}

};

int main(int argc,char** argv) {

  errchk {

    CLI;

    cli.input(argc,argv);
    cli.prich();

  } endchk;

};
