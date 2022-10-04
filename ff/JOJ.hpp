#ifndef __24_JOJ_H__
#define __24_JOJ_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Bin.hpp"
  #include "kvrnel/Tab.hpp"

// ---   *   ---   *   ---
// methods

class JOJ: public Bin {

public:

  VERSION   "v2.00.1";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---
// fat boiler

  // definition of pixel for this format
  #include "ff/JOJ_PIXEL.hpp"

  // pastes in a lot of boring constants
  #include "ff/JOJ_ENCODINGS.hpp"

// ---   *   ---   *   ---
// header def for non-pixel data in file

private:

  typedef struct {

    uint8_t             mip;
    uint8_t             imcnt;

    size_t              imsz;
    size_t              imsz_sq;

    JOJ::Encoding       enc;

    size_t              palcnt;
    Tab<size_t,size_t>  pal;

  } Header;

  cxstr   m_fsig="%JOJ";
  cx64    m_header_sz=sizeof(Header);

// ---   *   ---   *   ---
// attrs

  Header  m_header;
  float*  m_pixels;

  std::unique_ptr<JOJ::Pixel> m_buff;

// ---   *   ---   *   ---
// internals

  void pal_push(size_t key);

  SubEncoding read_mode(
    int type,
    bool mode

  );

// ---   *   ---   *   ---
// interface

public:

  // file into memory
  JOJ(std::string fpath);

  // memory into file
  JOJ(

    std::string fpath,

    float* pixels,
    size_t sz,

    JOJ::Encoding enc=ENCDEF

  );

  // processes loaded buffer
  void encoder(
    int imtype,
    bool mode=Frac::ENCODE

  );

};

// ---   *   ---   *   ---
// check structs are tight

CASSERT(

  sizeof(JOJ::Pixel)==4,
  "JOJ::Pixel must be four bytes in size\n"

);

// ---   *   ---   *   ---

#endif // __24_JOJ_H__
