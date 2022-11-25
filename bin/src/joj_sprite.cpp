// ---   *   ---   *   ---
// JOJ SPRITE
// Turns a series of images
// into a *.joj file
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

  #include "kvrnel/Cli.hpp"

  #include "ff/JOJ.hpp"
  #include "ff/CRK.hpp"

// ---   *   ---   *   ---
// info

  VERSION     "v0.00.2b";
  AUTHOR      "IBN-3DILA";

  OPTIONS {

    {"sorce_dir","-sd","--source_dir",0},
    {"output","-o","--output",0},
    {"type","-t","--type",0},
    {"atlas_sz","-as","--atlas_sz",0}

  };

// ---   *   ---   *   ---
// ROM

  const char COMP_YAUV[]={
    JOJ::YAUV,0x7F

  };

  struct Error {

    CX Evil::Errcode NO_IMG={

      .type=AR_ERROR,

      .code=__COUNTER__,

      .mess=
        "No images provided for *.joj"

    };

  };

// ---   *   ---   *   ---

int main(int argc,char** argv) {

  // data
  strvec      images;

  std::string srcdir   = "./";
  std::string output   = "./out";

  const char* img_type = COMP_YAUV;

  uint16_t    atlas_sz = 128;

// ---   *   ---   *   ---
// input handler

  errchk {

    CLI;

    // name of output file
    auto& arg_o=cli.have("output");
    if(arg_o.values.size()) {
      output=arg_o.values.back();

    };

    // images to pack together
    auto& data=cli.have("DATA");
    if(!data.values.size()) {
      err(Error::NO_IMG,output);

    };

    images=data.values;

    // name of srcdir dir
    auto& arg_sd=cli.have("sorce_dir");
    if(arg_sd.values.size()) {
      srcdir=arg_sd.values.back();

    };

    // size of final packed image
    auto& arg_as=cli.have("atlas_sz");
    if(arg_as.values.size()) {
      atlas_sz=std::stoi(arg_as.values.back());

    };

// TODO: add switches to this ;>
//
// for now we only use YAUV encoding,
// so it's very much low-priority

    // encoding selection
    auto& arg_t=cli.have("type");
    if(arg_t.on) {

      auto key=arg_t.values.back();

    };

  } endchk;

// ---   *   ---   *   ---
// the bit

  errchk {

    // create atlas
    JOJ joj(output+".joj",srcdir,img_type);
    for(auto& name : images) {
      joj.add_img_set(name);

    };

    joj.atlas_sz(atlas_sz);
    joj.pack();

    // create polydump
    CRK crk(output+".crk",joj);
    crk.pack();

  } endchk;

// ---   *   ---   *   ---
// debug

  errchk {

    JOJ joj(output+".joj");

    joj.unpack();

    joj.to_png(0,"atlas",JOJ::UNPACK_ATLAS);

    uint64_t idex=0;
    for(auto& name : images) {
      joj.to_png(idex++,name,JOJ::UNPACK_IMAGE);

    };

  } endchk;

  return 0;

};

// ---   *   ---   *   ---
