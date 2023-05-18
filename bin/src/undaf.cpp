// ---   *   ---   *   ---
// UNDAF
// Unpacks resource files
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

  VERSION     "v0.00.2b";
  AUTHOR      "IBN-3DILA";

  OPTIONS {

    {"fnames","-f","--fnames",0},
    {"output","-o","--output",0}

  };

// ---   *   ---   *   ---
// ROM

  struct Error {

    CX Evil::Errcode NO_SOURCE={

      .type=AR_ERROR,

      .code=__COUNTER__,
      .mess="No archive provided"

    };

  };

// ---   *   ---   *   ---

int main(int argc,char** argv) {

  strvec      fnames;

  std::string output;
  std::string source=Bin::getcwd();

// ---   *   ---   *   ---
// input handler

  errchk {

    CLI;

    // name of output files
    auto& arg_o=cli.have("output");
    if(arg_o.values.size()) {
      output=arg_o.values.back();

    };

    // names of entries to extract
    auto& arg_s=cli.have("fnames");
    if(arg_s.values.size()) {
      fnames=arg_s.values;

    };

    // input filenames
    auto& data=cli.have("DATA");
    if(!data.values.size()) {

      err(Error::NO_SOURCE,"");

    };

    source=data.values.back();

  } endchk;

// ---   *   ---   *   ---
// the bit

  errchk {

    DAF daf(source+".daf",Bin::READ);

    if(fnames.size()) {

      for(auto fname : fnames) {
        daf.extract(fname,output);

      };

    } else {
      daf.unpack(output);

    };

  } endchk;

  return 0;

};

// ---   *   ---   *   ---
