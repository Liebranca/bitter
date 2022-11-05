#ifndef __24_JOJ_H__
#define __24_JOJ_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Square.hpp"
  #include "kvrnel/Bytes.hpp"

  #include "kvrnel/Bin.hpp"
  #include "kvrnel/Mem.hpp"

// ---   *   ---   *   ---
// info

class JOJ: public Bin {

public:

  VERSION   "v2.00.2";
  AUTHOR    "IBN-3DILA";

// ---   *   ---   *   ---
// fat boiler

  // pastes in a lot of boring constants
  #include "ff/JOJ/Encoding.hpp"

  // definition of pixel for this format
  #include "ff/JOJ/Pixel.hpp"

  // used for sprite-packing
  #include "ff/JOJ/Tiles.hpp"

// ---   *   ---   *   ---

private:

  void get_tile(uint64_t offset);
  void set_tile(uint64_t offset);

  void to_tiles(void);
  void from_tiles(void);

// ---   *   ---   *   ---
// header def for non-pixel data in file
// used for quick read/write

  typedef struct {

    char       sig[4];

    uint16_t   img_sz;
    uint16_t   img_cnt;

    uint16_t   atlas_sz = 0;

    uint64_t   img_sz_sq;
    uint64_t   atlas_sz_sq;

    uint8_t    img_comp_cnt;
    char       img_comp[3];

  } Header;

// ---   *   ---   *   ---

  vicstr m_fsig(void) {
    return "%JOJ";

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

  std::string      m_src_path;
  strvec           m_img_names;

  uint16_t         m_next_img;
  char             m_next_type;

  JOJ::Encoding    m_enc;
  JOJ::SubEncoding m_c_enc;
  JOJ::Tiles       m_tiles;

  Mem<float>       m_raw;
  Mem<JOJ::Pixel>  m_pixels;

// ---   *   ---   *   ---
// internals

  JOJ::SubEncoding read_mode(char type);

  // buffer init/errme on bad open
  void chk_img_sz(

    std::string fpath,
    uint16_t    sz

  );

  // gets raw yauv buffer from png
  void from_png(
    std::string name,
    char* type

  );

  // get next "queued" read
  void read_next_img(void);

  // processes loaded buffer
  void encoder(bool mode=Frac::ENCODE);

  // per-pixel color conversion
  void color(float* pixel,bool mode);

  float* read_pixels(int idex);

// ---   *   ---   *   ---
// dumb && repetitive boilerplate methods

  #include "ff/JOJ/Image.hpp"

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

  // removes joj dumps
  void rmdump(void);

// ---   *   ---   *   ---
// interface

public:

  // png to joj
  JOJ(
    std::string fpath,
    std::string src_path,

    const char* comp=JOJ::DEFCOMP

  );

  // load joj
  JOJ(std::string fpath);

  // destroy
  ~JOJ(void);

  inline void atlas_sz(uint16_t x) {
    m_hed.atlas_sz    = x;
    m_hed.atlas_sz_sq = x*x;

  };

  inline uint16_t on_atlas_sz(bool& is_atlas) {
    return (is_atlas)
      ? m_hed.atlas_sz
      : m_hed.img_sz
      ;

  };

  inline uint64_t on_atlas_sz_sq(bool& is_atlas) {
    return (is_atlas)
      ? m_hed.atlas_sz_sq
      : m_hed.img_sz_sq
      ;

  };

  // joins image list into single file
  void pack(void);

  // ^dumps out packed
  void unpack(void);

  // ^raw joj to float*
  Mem<float> to_buff(
    int   idex,

    float mult  = 1.0f,
    bool  atlas = false

  );

  // ^raw joj to png
  void to_png(

    int         idex,
    std::string name,

    bool        atlas=false

  );

  // "enqueue" an image set in src path
  inline void add_img_set(std::string name) {
    m_img_names.push_back(name);
    m_hed.img_cnt++;

  };

// ---   *   ---   *   ---

  inline uint16_t get_img_sz(void) {
    return m_hed.img_sz;

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
