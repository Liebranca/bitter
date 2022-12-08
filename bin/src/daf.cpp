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
  #include "kvrnel/Evil.hpp"
  #include "kvrnel/Cli.hpp"

  #include "ff/DAF.hpp"

// ---   *   ---   *   ---
// info

  VERSION     "v0.00.1b";
  AUTHOR      "IBN-3DILA";

  OPTIONS {

    {"update","-u","--update",0},
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

  // data
  strvec      source;
  std::string output="./out";

// ---   *   ---   *   ---
// input handler

  errchk {

    CLI;

    // name of output file
    auto& arg_o=cli.have("output");
    if(arg_o.values.size()) {
      output=arg_o.values.back();

    };

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

    DAF daf(output+".daf",Bin::NEW);
    for(auto& fname : source) {
      Bin f(fname,Bin::READ);
      daf.push(f);

      f.unlink();

    };

  } endchk;

  return 0;

};

// ---   *   ---   *   ---
