#ifndef __24_JOJ_H__
#define __24_JOJ_H__

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Style.hpp"
  #include "kvrnel/Square.hpp"
  #include "kvrnel/Bytes.hpp"
  #include "kvrnel/Seph.hpp"

  #include "kvrnel/Bin.hpp"
  #include "kvrnel/Mem.hpp"

// ---   *   ---   *   ---
// info

class JOJ: public Bin {

public:

  VERSION   "v2.01.1";
  AUTHOR    "IBN-3DILA";

  enum {

    UNPACK_IMAGE=0,
    UNPACK_ATLAS=1

  };

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

  void set_tile(
    JOJ::Tiles& tiles,
    uint64_t    offset

  );

  void to_tiles(void);
  void from_tiles(JOJ::Tiles& tiles);

// ---   *   ---   *   ---
// header def for non-pixel data in file
// used for quick read/write

  typedef struct {

    char       sig[4];

    uint16_t   img_sz;
    uint16_t   img_cnt;

    uint16_t   atlas_sz=0;

    uint64_t   img_sz_sq;
    uint64_t   atlas_sz_sq;

    uint64_t   atlas_solid[3];

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
  bool             m_atlas_mode=false;

  JOJ::Encoding    m_enc;
  JOJ::SubEncoding m_c_enc;
  JOJ::Tiles       m_tiles;
  JOJ::Tiles       m_atlas;

  JOJ::Atlas_Desc  m_tab;

  Mem<float>       m_raw;
  Mem<JOJ::Pixel>  m_pixels;

// ---   *   ---   *   ---
// internals

  inline char get_img_type(uint16_t idex) {
    return m_hed.img_comp[
      idex%m_hed.img_comp_cnt

    ];

  };

  inline uint16_t img_idex(uint16_t idex) {
    return idex/m_hed.img_comp_cnt;

  };

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

  // get descriptor for atlas entry
  void img_table(
    JOJ::Atlas_Desc& dst,
    uint16_t         idex

  );

//  void bake_frames(JOJ::Atlas_Desc& tab);

  // processes loaded buffer
  void encoder(
    bool     mode = Frac::ENCODE,
    uint64_t sz   = 0

  );

  // ^normal+curvature
  void nc_encoder(
    bool     mode = Frac::ENCODE,
    uint64_t sz   = 0

  );

  // ^gutsof
  void nc_pack(uint64_t i);
  void nc_unpack(uint64_t i);

  // sphere coords handler for nc maps
  static Seph& nseph(void) {
    static Seph s(Seph::NC,4,6,6);
    return s;

  };

  // per-pixel color conversion
  void color(float* pixel,bool mode);

  float* read_pixels(
    int  idex,
    bool unpack_tiles=false

  );

// ---   *   ---   *   ---
// dumb && repetitive boilerplate methods

  #include "ff/JOJ/Image.hpp"

// ---   *   ---   *   ---
// buff to disk stuff

  // helper
  typedef struct {

    char     mode;
    char     type;

    uint16_t idex;

    Bin      color;
    Bin      table;
    Bin      atlas;

  } Swap;

  typedef std::unique_ptr<JOJ::Swap> Swap_PTR;

  // ^nit
  JOJ::Swap_PTR make_swapper(
    uint16_t idex,
    char     mode

  );

  // makes dump id
  std::string get_dump_f(char type);

  // save/load to/from dump
  JOJ::Swap_PTR swap_to(uint16_t idex,char mode);

  // ^load just the table for an image in atlas
  void read_img_table(uint16_t idex);

  // shifts the img fetch array
  void offset_img_table(JOJ::Swap_PTR& swap);

  // ^load the whole atlas
  void read_atlas(JOJ::Swap_PTR& swap);

  // ^runs processing on atlases
  void pack_atlas(void);

  // ^rewrites saved color
  void owc_atlas(
    JOJ::Swap_PTR& swap

  );

  // populates sub-element in atlas desc
  void build_img_table(
    uint16_t idex

  );

  // atlas desc to bytearray
  Mem<uint8_t> atlas_desc_to_buff(
    uint16_t idex

  );

  // bytearray to atlas desc
  void buff_to_atlas_desc(
    uint16_t idex

  );

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

  // joins image list into single file
  void pack(void);

  // ^dumps out packed
  void unpack(void);

  // ^raw joj to float*
  Mem<float> to_buff(

    uint16_t idex,

    bool     mode = JOJ::UNPACK_IMAGE,
    float    mult = 1.0f

  );

  // ^raw joj to png
  void to_png(

    uint16_t    idex,
    std::string name,

    bool        mode=JOJ::UNPACK_IMAGE

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

  inline float get_tile_scale(void) {
    return 1.0f/(m_hed.img_sz/m_tiles.sz);

  };

  inline uint16_t get_atlas_sz(void) {
    return m_hed.atlas_sz;

  };

  inline float get_atlas_scale(void) {
    return 1.0f/(m_hed.atlas_sz/m_atlas.sz);

  };

  inline JOJ::Atlas_Desc& get_atlas_desc(void) {
    return m_tab;

  };

  inline uint16_t get_pack_cnt(void) {
    return m_hed.img_cnt*m_hed.img_comp_cnt;

  };

  inline uint16_t get_comp_cnt(void) {
    return m_hed.img_comp_cnt;

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
