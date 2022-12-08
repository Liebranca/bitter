// ---   *   ---   *   ---
// BINCMP
// Compares a series of
// files byte-by-byte
//
// Also generates *.crk
// polydump for sprite drawing
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

  #include "kvrnel/Bin.hpp"
  #include "kvrnel/Cli.hpp"

// ---   *   ---   *   ---
// info

  VERSION     "v0.00.1b";
  AUTHOR      "IBN-3DILA";

  OPTIONS {

    {"non","-n","--non",0}

  };

// ---   *   ---   *   ---
// ROM

  struct Error {

    CX Evil::Errcode NO_INPUT={

      .type=AR_ERROR,

      .code=__COUNTER__,
      .mess="No files provided"

    };

  };

// ---   *   ---   *   ---

int main(int argc,char** argv) {

  // data
  strvec fnames;

  // used for result-print
  bool        out   = true;
  std::string flist = "";

  errchk {

    CLI;

    // filenames
    auto& data=cli.have("DATA");
    if(data.values.size()<2) {
      err(Error::NO_INPUT,"");

    };

    fnames=data.values;

  } endchk;

// ---   *   ---   *   ---
// the bit

  errchk {

    // get first file
    auto b0=Bin::orc(fnames[0]);

    // compare subsequent files
    // against first
    for(int i=1;i<fnames.size();i++) {
      auto b1=Bin::orc(fnames[i]);
      if(!b0.cmp(b1)) {
        out=false;
        break;

      };

    };

  } endchk;

// ---   *   ---   *   ---
// make string for list of file names

  flist+='(';

  for(auto& s : fnames) {

    flist+=s;
    if(&s != &fnames.back()) {
      flist+=',';

    };

  };

  flist+=')';

// ---   *   ---   *   ---
// spit notice

  std::cerr << "Files in " << flist << " are ";

  if(out) {
    std::cerr << "all equal" << std::endl;

  } else {
    std::cerr << "not equal" << std::endl;

  };

  return out;

};

// ---   *   ---   *   ---
