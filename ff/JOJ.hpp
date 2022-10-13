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

    char sig[8];

    uint64_t pal_cnt;
    uint64_t pal_sz;

    uint64_t img_sz;
    uint64_t key_sz;
    uint64_t idex_sz;

    // for zwrap
    uint64_t body_sz;

  } Header;

// ---   *   ---   *   ---

  vicstr m_fsig(void) {
    return "%JOJ\0\0\0\0";

  };

  vic64 m_header_sz(void) {
    return sizeof(JOJ::Header);

  };

// ---   *   ---   *   ---
// runtime attrs

  typedef struct {

    uint8_t  img_cnt;

    uint64_t img_sz;
    uint64_t img_sz_sq;

    JOJ::Encoding enc;
    JOJ::SubEncoding c_enc;

    std::vector<uint64_t> pal;
    uint64_t pal_cnt;

  } Meta;

// ---   *   ---   *   ---
// attrs

  JOJ::Meta m_meta;
  float*    m_raw;

  std::unique_ptr<JOJ::Pixel> m_pixels;
  std::unique_ptr<uint64_t>   m_blocks;

// ---   *   ---   *   ---
// internals

  // get vector of sizes for
  // current sub-encoding
  std::vector<uint64_t> get_enc_sz(void);

  // replace pixel data for indices
  // on a per-componet basis
  void palette_pixels(void);

  // slices m_buff
  void pixel2x2(
    JOJ::Pixel* (&pix)[4],
    uint64_t base

  );

  // ^hashing helper for the slice
  uint64_t pixel_block(
    JOJ::Pixel* (&pix)[4]

  );

  // transforms blocks according to palette
  void xlate_blocks(
    Symtab& tab

  );

  JOJ::SubEncoding read_mode(
    int type,
    bool mode

  );

  void write_header(uint64_t body_sz);
  JOJ::Header read_header(void);

// ---   *   ---   *   ---
// interface

public:

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
