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

    char sig[7];
    char enc[1];

    uint64_t img_sz;
    uint64_t img_sz_sq;

    uint64_t img_cnt;

    uint64_t key_sz;
    uint64_t idex_sz;

  } Header;

// ---   *   ---   *   ---

  vicstr m_fsig(void) {
    return "%JOJ\0\0\0";

  };

  vic64 m_header_sz(void) {
    return sizeof(JOJ::Header);

  };

// ---   *   ---   *   ---
// attrs

  Header    m_hed;
  float*    m_raw;

  PAL       m_pal[4];

  JOJ::Encoding    m_enc;
  JOJ::SubEncoding m_c_enc;

  std::unique_ptr<JOJ::Pixel> m_pixels;
  std::unique_ptr<uint64_t>   m_blocks;

// ---   *   ---   *   ---
// internals

  // get vector of sizes for
  // current sub-encoding
  std::vector<uint64_t> get_enc_sz(void);

  // get palette for a given bit-depth
  PAL& get_pal(uint64_t sz);

  // builds palettes from pixel components
  void palette_pixels(void);

  // swaps values of pixels for palette indices
  void xlate_pixels(void);

  // slices m_buff
  void pixel2x2(
    JOJ::Pixel* (&pix)[4],
    uint64_t base

  );

  // ^hashing helper for the slice
  uint64_t pixel_block(
    JOJ::Pixel* (&pix)[4]

  );

  // build palette by hashing blocks of pixels
  void palette_blocks(void);

  // transforms blocks according to palette
  void xlate_blocks(void);

  JOJ::SubEncoding read_mode(
    int type,
    bool mode

  );

  void write_header(void);

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
