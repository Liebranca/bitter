// ---   *   ---   *   ---
// DAF
// Builds a resource file
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Arstd.hpp"
  #include "kvrnel/Evil.hpp"
  #include "kvrnel/Cli.hpp"

  #include "ff/DAF.hpp"

// ---   *   ---   *   ---
// info

  VERSION     "v0.00.2b";
  AUTHOR      "IBN-3DILA";

  OPTIONS {

    {"inspect","-i","--i",1},
    {"output","-o","--output",0}

  };

// ---   *   ---   *   ---
// ROM

  struct Error {

    CX Evil::Errcode INPUT={

      .type=AR_ERROR,

      .code=__COUNTER__,
      .mess="No files provided for archiver"

    };

  };

// ---   *   ---   *   ---

int main(int argc,char** argv) {

  strvec      source;

  std::string output  = "./out";
  bool        inspect = false;

// ---   *   ---   *   ---
// input handler

  errchk {

    CLI;

    // name of output file
    auto& arg_o=cli.have("output");
    if(arg_o.values.size()) {
      output=arg_o.values.back();

    };

    // flips archive view mode
    auto& arg_b=cli.have("inspect");
    inspect=arg_b.on;

    // input filenames
    auto& data=cli.have("DATA");
    if(!data.values.size()) {
      err(Error::INPUT,output);

    };

    source=data.values;

  } endchk;

// ---   *   ---   *   ---
// the bit

  errchk {

    if(! inspect) {

      DAF daf(output+".daf",Bin::NEW);

      for(auto& fname : source) {
        Bin f(fname,Bin::READ);
        daf.push(basef(fname),f);

        f.unlink();

      };

    } else {

      DAF daf(source.back()+".daf",Bin::READ);
      daf.prich();

    };

  } endchk;

  return 0;

};

// ---   *   ---   *   ---
