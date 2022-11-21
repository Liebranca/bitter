#include "kvrnel/Cli.hpp"

const char* OPTIONS[]={

  "-l","-o"

};

int main(int argc,char** argv) {

  CLI;

  cli.input(argc,argv);
  cli.prich();

};
