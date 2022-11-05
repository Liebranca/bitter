// ---   *   ---   *   ---
// paste-in header
// part of the JOJ class

#ifndef __24_JOJ_TILES_H__
#define __24_JOJ_TILES_H__

// ---   *   ---   *   ---
// xform/fetch helper

private:

  cx8 STD_TILE_SZ=0x10;

typedef struct {
  uint16_t src_x;
  uint16_t src_y;
  uint16_t dst_x;
  uint16_t dst_y;

} Tile_Fetch;

// ---   *   ---   *   ---
// comparison/reconstruction helper

typedef struct {

  // fetch-from
  uint16_t x;
  uint16_t y;

  // fetch-to
  uint16_t dx;
  uint16_t dy;

  uint8_t  rotated  : 2;
  uint8_t  mirrored : 2;
  uint8_t  cleared  : 3;

  // indices of tiles sampling this one
  std::vector<uint64_t> used_by;

} Tile_Desc;

typedef struct {

  uint16_t x;
  uint16_t y;

  uint8_t  rotated  : 2;
  uint8_t  mirrored : 2;
  uint8_t  cleared  : 3;

} Tile_Desc_Packed;

// ---   *   ---   *   ---

public:

typedef struct {

  uint16_t x;
  uint16_t y;

  uint16_t sz;
  uint16_t sz_i;

  uint16_t cnt;

  uint64_t sz_sq;
  uint64_t cnt_sq;

  uint64_t solid;

  Mem<JOJ::Pixel> data;
  std::vector<JOJ::Tile_Desc> tab;

// ---   *   ---   *   ---
// default constructor doesn't want to work

  void nit(

    uint16_t sz,

    uint64_t img_sz,
    uint64_t img_sz_sq

  ) {

    this->x       = 0;
    this->y       = 0;

    this->sz      = sz;
    this->sz_sq   = sz*sz;
    this->sz_i    = sz-1;

    this->cnt     = img_sz/sz;
    this->cnt_sq  = this->cnt*this->cnt;

    this->solid   = 0;

    Mem<JOJ::Pixel> data(img_sz_sq);
    this->data.copy(data);

    this->tab.resize(this->cnt_sq);

  };

// ---   *   ---   *   ---

  // write tiles info to a buffer
  Mem<JOJ::Tile_Desc_Packed> to_buff(void);

  // ^inverse
  void from_buff(
    Mem<JOJ::Tile_Desc_Packed>& src

  );

  // discard contents of tile
  void clear(
    uint16_t x,
    uint16_t y

  );

  // true if tile is blank
  bool is_clear(JOJ::Pixel* a);

  // true if tiles are identical
  bool cmp(
    JOJ::Pixel* a,
    JOJ::Pixel* b

  );

  // attempt matching with previous tiles
  void match(
    uint16_t x,
    uint16_t y

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

  enum {
    SOLID       = 0b000,
    CLEAR_NAT   = 0b001,
    CLEAR_FETCH = 0b010,
    FAKE_SOLID  = 0b100,

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

  // send duplicate of tile to buffer
  Mem<JOJ::Pixel> copy(
    JOJ::Pixel* pixel

  );

  // replace tile with another
  void mov(
    JOJ::Pixel* dst,
    JOJ::Pixel* src

  );

  // move tile to first empty one
  void reloc(JOJ::Tile_Desc& td);

  // ^runs reloc on whole image
  void reloc_all(void);

  // builds table from image
  void pack(void);

  // build original image from table
  void unpack(void);

  // get tile number
  inline uint64_t tile_idex(
    uint16_t x,
    uint16_t y

  ) {return sq_idex(x,y,this->cnt);};

  // get index of tile in buffer
  inline uint64_t real_idex(
    uint16_t x,
    uint16_t y

  ) {

    return sq_idex(

      x*this->sz_sq,
      y*this->cnt,

      this->sz_sq

    );

  };

  // return ptr to tile at [x,y]
  inline JOJ::Pixel* get(
    uint16_t x,
    uint16_t y

  ) {

    JOJ::Pixel* out=&this->data[0];
    return out+this->real_idex(x,y);

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

} Tiles;

// ---   *   ---   *   ---

#endif // __24_JOJ_TILES_H__
