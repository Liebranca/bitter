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

  // pastes in a lot of boring constants
  #include "ff/JOJ/Encoding.hpp"

  // definition of pixel for this format
  #include "ff/JOJ/Pixel.hpp"

// ---   *   ---   *   ---
// header def for non-pixel data in file
// used for quick read/write

private:

  typedef struct {

    uint64_t palcnt;
    uint64_t imsz;
    uint64_t keysz;
    uint64_t idexsz;

  } Header;

  cxstr   m_fsig="%JOJ";
  cx64    m_header_sz=sizeof(JOJ::Header);

// ---   *   ---   *   ---
// runtime attrs

  typedef struct {

    uint8_t  mip;
    uint8_t  imcnt;

    uint64_t imsz;
    uint64_t imsz_sq;

    JOJ::Encoding enc;
    JOJ::SubEncoding c_enc;

    std::vector<uint64_t> pal;
    uint64_t palcnt;

  } Meta;

// ---   *   ---   *   ---
// attrs

  JOJ::Meta m_meta;
  float*    m_raw;

  std::unique_ptr<JOJ::Pixel> m_pixels;
  std::unique_ptr<uint64_t>   m_blocks;

// ---   *   ---   *   ---
// internals

  // slices m_buff
  void pixel2x2(
    JOJ::Pixel* (&pix)[4],
    uint64_t base

  );

  // ^hashing helper for the slice
  uint64_t pixel_block(
    JOJ::Pixel_Block& blk,
    JOJ::Pixel* (&pix)[4]

  );

  // transforms blocks according to palette
  void xlate_blocks(
    std::vector<uint64_t>& keys,
    Tab<uint64_t,JOJ::Pixel_Block>& tab

  );

  // organizes blocks by frequency
  void sort_blocks(
    std::vector<uint64_t>& keys,
    Tab<uint64_t,JOJ::Pixel_Block>& tab

  );

  SubEncoding read_mode(
    int type,
    bool mode

  );

  void write_header(void);
  JOJ::Header read_header(void);

// ---   *   ---   *   ---
// interface

public:

  // compiler trash
  JOJ(std::string fpath);

  // file into memory
  JOJ(

    std::string fpath,
    float* pixels,

    JOJ::Encoding enc=ENCDEF

  );

  // memory into file
  JOJ(

    std::string fpath,

    float* pixels,
    uint64_t sz,

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

  // read image from file
  void read(void);

};

// ---   *   ---   *   ---
// check structs are tight

CASSERT(

  sizeof(JOJ::Pixel)==4,
  "JOJ::Pixel must be four bytes in size\n"

);

// ---   *   ---   *   ---

#endif // __24_JOJ_H__
