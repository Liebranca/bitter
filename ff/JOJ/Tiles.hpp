// ---   *   ---   *   ---
// paste-in header
// part of the JOJ class

#ifndef __24_JOJ_TILES_H__
#define __24_JOJ_TILES_H__

// ---   *   ---   *   ---
// xform/fetch helper

private:

struct Tile_Fetch {
  uint16_t src_x;
  uint16_t src_y;
  uint16_t dst_x;
  uint16_t dst_y;

};

// ---   *   ---   *   ---
// atlas table helper

struct Tile_Ref {

  uint64_t img;
  uint64_t tile;

  Tile_Ref(void) {};
  Tile_Ref(
    uint16_t img,
    uint64_t tile

  ) {

    this->img  = img;
    this->tile = tile;

  };

};

// ---   *   ---   *   ---
// comparison/reconstruction helper

public:

struct Tile_Desc {

  // fetch-from
  uint16_t x;
  uint16_t y;

  // fetch-to
  uint16_t dx;
  uint16_t dy;

  uint8_t  rotated  : 2;
  uint8_t  mirrored : 2;
  uint8_t  cleared  : 3;

  std::vector<uint64_t> users;

  void wcopy(Tile_Desc& other) {

    x        = other.x;
    y        = other.y;
    dx       = other.dx;
    dy       = other.dy;

    rotated  = other.rotated;
    mirrored = other.mirrored;
    cleared  = other.cleared;

  };

};

struct Tile_Desc_Packed {

  uint16_t x;
  uint16_t y;

  uint8_t  rotated  : 2;
  uint8_t  mirrored : 2;
  uint8_t  cleared  : 3;

};

// ---   *   ---   *   ---
// shorthands

typedef std::vector<
  std::vector<JOJ::Tile_Ref>

> Tile_Refs;

typedef std::vector<JOJ::Tile_Desc> Img_Desc;
typedef std::vector<Img_Desc> Atlas_Desc;
typedef std::vector<uint64_t> Xorkeys;

// ---   *   ---   *   ---
// another comparison helper

struct Tile_Cmp {

  JOJ::Tile_Desc& td;

  JOJ::Pixel*     a;
  JOJ::Pixel*     b;

  uint16_t        x;
  uint16_t        y;

};

// ---   *   ---   *   ---

  cx8 STD_TILE_SZ=0x10;

struct Tiles {

  uint16_t x;
  uint16_t y;

  uint16_t sz;
  uint16_t sz_i;

  uint16_t cnt;

  uint64_t sz_sq;
  uint64_t cnt_sq;

  Mem<JOJ::Pixel>  data;
  Mem<JOJ::Pixel>  copy_tile;
  Xorkeys          xorkeys;

  JOJ::Tile_Refs   users;
  JOJ::Atlas_Desc  image;
  JOJ::Img_Desc    tab;

// ---   *   ---   *   ---
// default constructor doesn't want to work

  void nit(

    uint16_t sz,
    uint64_t img_sz,
    uint64_t img_sz_sq,

    uint32_t atlas=0

  ) {

    this->x       = 0;
    this->y       = 0;

    this->sz      = sz;
    this->sz_sq   = sz*sz;
    this->sz_i    = sz-1;

    this->cnt     = img_sz/sz;
    this->cnt_sq  = this->cnt*this->cnt;

    Mem<JOJ::Pixel> _data(img_sz_sq);
    this->data.copy(_data);

    Mem<JOJ::Pixel> _copy_tile(this->sz_sq);
    this->copy_tile.copy(_copy_tile);

    this->xorkeys.resize(this->cnt_sq);
    this->tab.resize(this->cnt_sq);

    if(atlas) {

      uint16_t entry_sz  = atlas&0xFFFF;
      uint16_t entry_cnt = atlas>>16;

      this->users.resize(this->cnt_sq);
      this->image.resize(entry_cnt);

      uint64_t entry_sz_sq=entry_sz*entry_sz;

      for(auto& td_arr : this->image) {
        td_arr.resize(entry_sz_sq);

      };

    };

  };

// ---   *   ---   *   ---

  // write tiles info to a buffer
  Mem<uint8_t> to_buff(void);

  // ^inverse
  void from_buff(Mem<uint8_t>& src);

  // offsets the next table read
  void fetch_offset(

    JOJ::Tiles& atlas,

    uint16_t       img_idex,
    uint64_t       prev_tiles

  );

  enum {
    SOLID       = 0b000,
    CLEAR_NAT   = 0b001,
    CLEAR_FETCH = 0b010,
    FAKE_SOLID  = 0b100,

  };

  // discard contents of tile
  void clear(
    uint16_t x,
    uint16_t y,

    uint8_t  flag=JOJ::Tiles::CLEAR_FETCH

  );

  // ^entire image
  void clear_all(void);

  // true if tile is blank
  bool is_clear(JOJ::Pixel* a);

  // number of used tiles
  uint64_t solid_cnt(void);

  // true if tiles are identical
  bool cmp(
    JOJ::Pixel* a,
    JOJ::Pixel* b

  );

  // generate data identifier
  // for a particular tile
  uint64_t gen_xorkey(
    JOJ::Pixel* a

  );

  // ^batch, gen for whole image
  void map_xorkey(void);

  // ^fetch generated
  uint64_t get_xorkey(
    uint16_t ix,
    uint16_t iy

  );

  // ^compare keys of two tiles
  bool cmp_xorkey(

    // a
    uint16_t x0,
    uint16_t y0,

    // b
    uint16_t x1,
    uint16_t y1

  );

  // initializes tile descriptor
  JOJ::Tile_Desc& nit_desc(
    uint16_t x,
    uint16_t y

  );

  // get handle to compared tile
  JOJ::Pixel* match_get_ref(
    uint16_t x,
    uint16_t y

  );

  // matches two tiles
  bool match_cmp(JOJ::Tile_Cmp& mat);

  enum {

    XFORM_APPLY,
    XFORM_SKIP

  };

  // attempt matching with previous tiles
  void match(
    uint16_t _x,
    uint16_t _y

  );

  // initialize a tile for processing
  void pre_proc(
    uint16_t _x,
    uint16_t _y

  );

  enum {

    MIRROR_NONE,

    MIRROR_X,
    MIRROR_Y,
    MIRROR_XY

  };

  enum {

    ROT_0,

    ROT_90,
    ROT_180,
    ROT_240

  };

  // ^attempts mirroring
  bool match_mirror(
    JOJ::Tile_Desc& td

  );

  // ^attemps rotation
  bool match_rotate(
    JOJ::Tile_Desc& td

  );

  // walks back mirrors in descriptor
  void undo_mirror(
    JOJ::Tile_Desc& td

  );

  // walks back rotations in descriptor
  void undo_rotation(
    JOJ::Tile_Desc& td

  );

  // ^undoes failed attempts
  inline void match_undo(
    JOJ::Tile_Desc& td

  ) {

    this->undo_rotation(td);
    this->undo_mirror(td);

    td.rotated  = ROT_0;
    td.mirrored = MIRROR_NONE;

  };

  // rotates from descriptor
  void apply_rotation(
    JOJ::Tile_Desc& td

  );

  // mirrors from descriptor
  void apply_mirror(
    JOJ::Tile_Desc& td

  );

  // send duplicate of tile to tile_copy buffer
  void copy(JOJ::Pixel* pixel);

  // replace tile with another
  void mov(
    JOJ::Pixel* dst,
    JOJ::Pixel* src

  );

  // move tile to first empty one
  void reloc(JOJ::Tile_Desc& td);

  // move local fetch refs
  void reloc_users(

    uint64_t idex,

    uint16_t x,
    uint16_t y

  );

  inline void clear_users(void) {
    this->users.clear();
    this->users.resize(this->cnt_sq);

  };

  // ^runs reloc on whole image
  void reloc_all(void);

  // builds table from image
  void pack(bool skip_match=false);

  // build original image from table
  void unpack(
    JOJ::Tiles& atlas

  );

  // get tile number
  inline uint64_t tile_idex(
    uint16_t _x,
    uint16_t _y

  ) {return sq_idex(_x,_y,this->cnt);};

  // get index of tile in buffer
  inline uint64_t real_idex(
    uint16_t _x,
    uint16_t _y

  ) {

    return sq_idex(

      _x*this->sz_sq,
      _y*this->cnt,

      this->sz_sq

    );

  };

  // return ptr to tile at [x,y]
  inline JOJ::Pixel* get(
    uint16_t _x,
    uint16_t _y

  ) {

    return

      &this->data[0]
    + this->real_idex(_x,_y)
    ;

  };

  // return ptr to tile N
  inline JOJ::Pixel* get(uint64_t i) {

    return

      &this->data[0]
    + (i*this->sz_sq)
    ;

  };

// ---   *   ---   *   ---
// under the hood

private:

  enum {

    XFORM_ROR,
    XFORM_ROL,

    XFORM_XMIR,
    XFORM_YMIR

  };

  // generic
  void xform(
    uint16_t off_x,
    uint16_t off_y,

    uint32_t fn_idex

  );

  // 90 degrees, clockwise rotation
  void xform_ror(Tile_Fetch& co) {
    co.dst_x=this->sz_i-co.src_y;
    co.dst_y=co.src_x;

  };

  // ^anti-clockwise
  void xform_rol(Tile_Fetch& co) {
    co.dst_x=co.src_y;
    co.dst_y=this->sz_i-co.src_x;

  };

  // invert on X
  void xform_xmir(Tile_Fetch& co) {
    co.dst_x=this->sz_i-co.src_x;
    co.dst_y=co.src_y;

  };

  // ^on Y
  void xform_ymir(Tile_Fetch& co) {
    co.dst_x=co.src_x;
    co.dst_y=this->sz_i-co.src_y;

  };

// ---   *   ---   *   ---
// ^entry points

public:

  inline void ror(uint16_t x,uint16_t y) {
    this->xform(x,y,XFORM_ROR);

  };

  inline void rol(uint16_t x,uint16_t y) {
    this->xform(x,y,XFORM_ROL);

  };

  inline void xmir(uint16_t x,uint16_t y) {
    this->xform(x,y,XFORM_XMIR);

  };

  inline void ymir(uint16_t x,uint16_t y) {
    this->xform(x,y,XFORM_YMIR);

  };

// ---   *   ---   *   ---

};

// ---   *   ---   *   ---

#endif // __24_JOJ_TILES_H__
