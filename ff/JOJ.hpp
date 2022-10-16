#ifndef __24_JOJ_H__
#define __24_JOJ_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Bytes.hpp"

  #include "kvrnel/Bin.hpp"
  #include "kvrnel/Tab.hpp"

  #include "ff/PAL.hpp"

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

    uint64_t img_sz;
    uint64_t img_sz_sq;

    uint64_t img_cnt;

  } Header;

// ---   *   ---   *   ---

  vicstr m_fsig(void) {
    return "%JOJ\0\0\0\0";

  };

  vic64 m_header_sz(void) {
    return sizeof(JOJ::Header);

  };

  VIC void* get_header(void) {
    return &m_hed;

  };

// ---   *   ---   *   ---
// attrs

  Header m_hed;
  PAL    m_pal[3];

  std::string      m_src_path;
  strvec           m_img_names;

  uint64_t         m_next_img;
  char             m_next_type;

  JOJ::Encoding    m_enc;
  JOJ::SubEncoding m_c_enc;

  std::unique_ptr<float>      m_raw;
  std::unique_ptr<JOJ::Pixel> m_pixels;

// ---   *   ---   *   ---
// internals

  JOJ::SubEncoding read_mode(char type);

  // buffer init/errme on bad open
  void chk_img_sz(

    std::string fpath,

    uint64_t width,
    uint64_t height

  );

  // gets raw yauv buffer from png
  void from_png(
    std::string name,
    char* type

  );

  // get next "queued" read
  void read_next_img(void);

  // processes loaded buffer
  void encode(bool mode=Frac::ENCODE);

// ---   *   ---   *   ---
// buff to disk stuff

  // makes dump id
  std::string get_dump_f(int idex);

  // save/load to/from dump
  void swap_to(int idex,char mode);

  // write packed
  void write(void);

  // read unpacked
  void read(void);

// ---   *   ---   *   ---
// interface

public:

  // png to joj
  JOJ(
    std::string fpath,
    std::string src_path

  );

  // load joj
  JOJ(std::string fpath);

  // joins image list into single file
  void pack(void);

  // "enqueue" an image set in src path
  inline void add_img_set(std::string name) {
    m_img_names.push_back(name);
    m_hed.img_cnt++;

  };

};

// ---   *   ---   *   ---
// check structs are tight

CASSERT(

  sizeof(JOJ::Pixel)==4,
  "JOJ::Pixel must be four bytes in size\n"

);

// ---   *   ---   *   ---

#endif // __24_JOJ_H__
