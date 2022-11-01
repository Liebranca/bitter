// ---   *   ---   *   ---
// boring details that'd make the header
// too long for the casual reader
// meant to be pasted in!

#ifndef __24_JOJ_TILES_H__
#define __24_JOJ_TILES_H__

// ---   *   ---   *   ---
// xform/fetch helper

private:

typedef struct {
  uint64_t src_x;
  uint64_t src_y;
  uint64_t dst_x;
  uint64_t dst_y;

} Tile_Fetch;

// ---   *   ---   *   ---

public:

typedef struct {

  uint64_t x;
  uint64_t y;

  uint64_t sz;
  uint64_t sz_sq;
  uint64_t sz_i;

  uint64_t cnt;
  uint64_t cnt_sq;

  std::unique_ptr<JOJ::Pixel> data;

// ---   *   ---   *   ---
// true if chunks are identical

  bool cmp(
    uint64_t idex_a,
    uint64_t idex_b

  );

// ---   *   ---   *   ---
// return ptr to tile at [x,y]

  inline JOJ::Pixel* get(
    uint64_t x,
    uint64_t y

  ) {

    JOJ::Pixel* out=this->data.get();
    uint64_t offset=sq_idex(

      x*this->sz_sq,
      y*this->cnt,

      this->sz_sq

    );

    return out+offset;

  };

// ---   *   ---   *   ---
// send duplicate of tile to buffer

  std::unique_ptr<JOJ::Pixel> copy(
    JOJ::Pixel* pixel

  );

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
    uint64_t off_x,
    uint64_t off_y,

    uint64_t fn_idex

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

  inline void ror(uint64_t x,uint64_t y) {
    this->xform(x,y,JOJ::Tiles::XFORM_ROR);

  };

  inline void rol(uint64_t x,uint64_t y) {
    this->xform(x,y,JOJ::Tiles::XFORM_ROL);

  };

  inline void xmir(uint64_t x,uint64_t y) {
    this->xform(x,y,JOJ::Tiles::XFORM_XMIR);

  };

  inline void ymir(uint64_t x,uint64_t y) {
    this->xform(x,y,JOJ::Tiles::XFORM_YMIR);

  };

// ---   *   ---   *   ---

} Tiles;

// ---   *   ---   *   ---

#endif // __24_JOJ_TILES_H__
