// ---   *   ---   *   ---
// JOJ TILES
// Boards of pixels
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include "ff/JOJ.hpp"

// ---   *   ---   *   ---
// discard contents of tile

void JOJ::Tiles::clear(
  uint64_t x,
  uint64_t y

) {

  JOJ::Pixel* tile = this->get(x,y);
  for(uint64_t i=0;i<this->sz_sq;i++) {
    tile->clear();
    tile++;

  };

  this->cleared[this->tile_idex(x,y)]=1;

};

// ---   *   ---   *   ---
// true if tiles are identical

bool JOJ::Tiles::cmp(
  JOJ::Pixel* a,
  JOJ::Pixel* b

) {

  bool out=true;

  for(uint64_t i=0;i<this->sz_sq;i++) {

    if(*a++!=*b++) {
      out=false;
      break;

    };

  };

  return out;

};

// ---   *   ---   *   ---
// tries out mirroring combinations

bool JOJ::Tiles::match_mirror(
  JOJ::Tile_Desc& td

) {

  bool out=true;

// ---   *   ---   *   ---
// mirror tile to try another match

  switch(td.mirrored) {

  case MIRROR_NONE:
    this->xmir(td.x,td.y);
    td.mirrored=MIRROR_X;

    break;

  case MIRROR_X:
    this->xmir(td.x,td.y);
    this->ymir(td.x,td.y);
    td.mirrored=MIRROR_Y;

    break;

// ---   *   ---   *   ---
// try last combination

  case MIRROR_Y:
    this->xmir(td.x,td.y);
    td.mirrored=MIRROR_XY;

    break;

// ---   *   ---   *   ---
// undo and break loop

  case MIRROR_XY:
    this->xmir(td.x,td.y);
    this->ymir(td.x,td.y);
    td.mirrored=MIRROR_NONE;

    out=false;

  };

  return out;

};

// ---   *   ---   *   ---
// tries out rotation and mirror

bool JOJ::Tiles::match_rotate(
  JOJ::Tile_Desc& td

) {

  bool out=true;

  // rotate
  this->ror(td.x,td.y);
  td.rotated++;

  // attempt mirroring every 4 rotations
  if(td.rotated==ROT_0) {
    out*=this->match_mirror(td);

  };

  return out;

};

// ---   *   ---   *   ---
// selfex

void JOJ::Tiles::undo_mirror(
  JOJ::Tile_Desc& td

) {

  switch(td.mirrored) {

  case MIRROR_X:
    this->xmir(td.x,td.y);
    break;

  case MIRROR_Y:
    this->ymir(td.x,td.y);
    break;

  case MIRROR_XY:
    this->xmir(td.x,td.y);
    this->ymir(td.x,td.y);

    break;

  };

};

// ---   *   ---   *   ---

void JOJ::Tiles::undo_rotation(
  JOJ::Tile_Desc& td

) {

  switch(td.rotated) {

  case ROT_90:
    this->rol(td.x,td.y);
    break;

  case ROT_180:
    this->ror(td.x,td.y);
    this->ror(td.x,td.y);
    break;

  case ROT_240:
    this->ror(td.x,td.y);
    break;

  };

};

// ---   *   ---   *   ---
// match tile against the rest of the image

JOJ::Tile_Desc JOJ::Tiles::match(
  uint64_t x,
  uint64_t y

) {

  JOJ::Tile_Desc out={
    .rotated  = 0,
    .mirrored = 0,

    .x        = x,
    .y        = y

  };

  // get current
  JOJ::Pixel* a=this->get(x,y);

  // walk tiles
  for(uint64_t iy=0;iy<x;iy++) {
  for(uint64_t ix=0;ix<y;ix++) {

    // get previous
    JOJ::Pixel* b=this->get(ix,iy);

    // skip blank tiles
    if(this->cleared[this->tile_idex(ix,iy)]) {
      continue;

    };

// ---   *   ---   *   ---
// compare against previous

CMP:

    bool same=this->cmp(a,b);

    // no match
    if(!same) {

      // transforms pending, retry
      if(this->match_rotate(out)) {
        goto CMP;

      // transforms exhausted, undo
      } else {
        this->match_undo(out);

      };

    // match, end here
    } else if(same) {
      out.x=ix;
      out.y=iy;

      this->clear(x,y);

      return out;

    };

// ---   *   ---   *   ---
// give back descriptor

  }};

  return out;

};

// ---   *   ---   *   ---
// move tile to first empty one

void JOJ::Tiles::reloc(
  JOJ::Tile_Desc& td

) {

  uint64_t src_idex=this->tile_idex(td.x,td.y);

  if(this->cleared[src_idex]) {
    return;

  };

// ---   *   ---   *   ---

  for(uint64_t y=0;y<this->cnt;y++) {
  for(uint64_t x=0;x<this->cnt;x++) {

    uint64_t dst_idex=this->tile_idex(x,y);

    if(dst_idex==src_idex) {
      return;

    } else if(this->cleared[dst_idex]) {

      this->mov(
        this->get(x,y),
        this->get(td.x,td.y)

      );

      this->clear(td.x,td.y);
      this->cleared[dst_idex]=0;

      td.x=x;
      td.y=y;

      return;

    };

  }};

};

// ---   *   ---   *   ---
// send duplicate of tile to buffer

std::unique_ptr<JOJ::Pixel> JOJ::Tiles::copy(
  JOJ::Pixel* pixel

) {

  std::unique_ptr<JOJ::Pixel> out(
    new JOJ::Pixel[this->sz_sq]

  );

  JOJ::Pixel* out_p=out.get();

  memcpy(
    out_p,pixel,

    this->sz_sq
  * sizeof(JOJ::Pixel)

  );

  return out;

};

// ---   *   ---   *   ---
// replace tile with another

void JOJ::Tiles::mov(
  JOJ::Pixel* dst,
  JOJ::Pixel* src

) {

  memcpy(
    dst,src,

    this->sz_sq
  * sizeof(JOJ::Pixel)

  );

};

// ---   *   ---   *   ---
// generic iterative transform

void JOJ::Tiles::xform(
  uint64_t off_x,
  uint64_t off_y,

  uint64_t fn_idex

) {

  auto pixel = this->get(off_x,off_y);
  auto buff  = this->copy(pixel);

  JOJ::Pixel* buff_p=buff.get();

// ---   *   ---   *   ---
// get function pointer from idex

  void (JOJ::Tiles::*get_next)
  (JOJ::Tile_Fetch&);

  switch(fn_idex) {

  case XFORM_ROR:
    get_next=&JOJ::Tiles::xform_ror;
    break;

  case XFORM_ROL:
    get_next=&JOJ::Tiles::xform_rol;
    break;

  case XFORM_XMIR:
    get_next=&JOJ::Tiles::xform_xmir;
    break;

  case XFORM_YMIR:
    get_next=&JOJ::Tiles::xform_ymir;
    break;

  };

// ---   *   ---   *   ---
// walk the tile

  Tile_Fetch co={0};

  for(co.src_y=0;co.src_y<this->sz;co.src_y++) {
  for(co.src_x=0;co.src_x<this->sz;co.src_x++) {

    // transform coords
    (this->*get_next)(co);

    // ^map to square
    uint64_t dst_idex=sq_idex(
      co.dst_x,co.dst_y,this->sz

    );

    uint64_t src_idex=sq_idex(
      co.src_x,co.src_y,this->sz

    );

    // fetch
    pixel[dst_idex]=buff_p[src_idex];

  }};

  buff.reset();

};

// ---   *   ---   *   ---
