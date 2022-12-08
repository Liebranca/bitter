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

  VERSION     "v0.00.1b";
  AUTHOR      "IBN-3DILA";

  OPTIONS {

    {"idex","-i","--idex",0},
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

  // data
  std::vector<uint64_t> indices;

  strvec      output;
  std::string source="./out";

// ---   *   ---   *   ---
// input handler

  errchk {

    CLI;

    // name of output files
    auto& arg_o=cli.have("output");
    if(arg_o.values.size()) {
      output=arg_o.values;

    };

    // indices of entries to extract
    auto& arg_i=cli.have("idex");
    if(arg_i.values.size()) {

      for(auto& s : arg_i.values) {
        indices.push_back(std::stoull(s));

      };

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
    daf.unpack(output.back());

  } endchk;

  return 0;

};

// ---   *   ---   *   ---
