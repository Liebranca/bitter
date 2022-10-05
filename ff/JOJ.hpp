#ifndef __24_JOJ_H__
#define __24_JOJ_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Bin.hpp"
  #include "kvrnel/Tab.hpp"

// ---   *   ---   *   ---
// info

class JOJ: public Bin {

public:

  VERSION   "v2.00.1";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---
// fat boiler

  // definition of pixel for this format
  #include "ff/JOJ/Pixel.hpp"

  // pastes in a lot of boring constants
  #include "ff/JOJ/Encoding.hpp"

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

    Tab<

      std::string,
      JOJ::Pixel_Block

    > blkpal;

  } Header;

  cxstr   m_fsig="%JOJ";
  cx64    m_header_sz=sizeof(Header);

// ---   *   ---   *   ---
// attrs

  Header  m_header;
  float*  m_raw;

  std::unique_ptr<JOJ::Pixel> m_pixels;
  std::unique_ptr<size_t>     m_blocks;

// ---   *   ---   *   ---
// internals

  // slices m_buff
  void pixel2x2(
    JOJ::Pixel* (&pix)[4],
    size_t base

  );

  // ^hashing helper for the slice
  std::string pixel_block(
    JOJ::Pixel_Block& blk,
    JOJ::Pixel* (&pix)[4]

  );

  // registers colors
  size_t push_palette(JOJ::Pixel* b);

  // transforms blocks according to palette
  void xlate_blocks(
    std::vector<std::string>& keys

  );

  // organizes blocks by frequency
  void sort_blocks(
    std::vector<std::string>& keys

  );

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

  // tightens the bits in an
  // already encoded image
  void compress(void);

  // write image contents
  void write(void);

};

// ---   *   ---   *   ---
// check structs are tight

CASSERT(

  sizeof(JOJ::Pixel)==4,
  "JOJ::Pixel must be four bytes in size\n"

);

// ---   *   ---   *   ---

#endif // __24_JOJ_H__
