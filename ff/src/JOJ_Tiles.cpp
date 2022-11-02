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
// instas

template class Mem<JOJ::Tile_Desc>;

// ---   *   ---   *   ---
// write tiles info to a buffer

Mem<JOJ::Tile_Desc_Packed>
JOJ::Tiles::to_buff(void) {

  Mem<JOJ::Tile_Desc_Packed> out(this->cnt_sq);

  uint64_t i=0;
  for(JOJ::Tile_Desc& td : this->tab) {

    out[i].rotated  = td.rotated;
    out[i].mirrored = td.mirrored;

    out[i].x        = td.x;
    out[i].y        = td.y;
    out[i].dx       = td.dx;
    out[i].dy       = td.dy;

    i++;

  };

  return Mem<JOJ::Tile_Desc_Packed>(out);

};

// ---   *   ---   *   ---
// ^get tiles from mem

void JOJ::Tiles::from_buff(
  Mem<JOJ::Tile_Desc_Packed>& src

) {

  uint64_t i=0;
  for(JOJ::Tile_Desc& td : this->tab) {

    td.rotated  = src[i].rotated;
    td.mirrored = src[i].mirrored;

    td.x        = src[i].x;
    td.y        = src[i].y;
    td.dx       = src[i].dx;
    td.dy       = src[i].dy;

    i++;

  };

};

// ---   *   ---   *   ---
// discard contents of tile

void JOJ::Tiles::clear(
  uint16_t x,
  uint16_t y

) {

  JOJ::Pixel* tile = this->get(x,y);
  for(uint16_t i=0;i<this->sz_sq;i++) {
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

  for(uint16_t i=0;i<this->sz_sq;i++) {

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
// ^reverse

void JOJ::Tiles::apply_mirror(
  JOJ::Tile_Desc& td

) {

  switch(td.mirrored) {

  case MIRROR_X:
    this->xmir(td.dx,td.dy);
    break;

  case MIRROR_Y:
    this->ymir(td.dx,td.dy);
    break;

  case MIRROR_XY:
    this->xmir(td.dx,td.dy);
    this->ymir(td.dx,td.dy);

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
// ^reverse

void JOJ::Tiles::apply_rotation(
  JOJ::Tile_Desc& td

) {

  switch(td.rotated) {

  case ROT_90:
    this->ror(td.dx,td.dy);
    break;

  case ROT_180:
    this->ror(td.dx,td.dy);
    this->ror(td.dx,td.dy);
    break;

  case ROT_240:
    this->rol(td.dx,td.dy);
    break;

  };

};

// ---   *   ---   *   ---
// match tile against the rest of the image

void JOJ::Tiles::match(
  uint16_t x,
  uint16_t y

) {

  uint64_t        td_idex = this->tile_idex(x,y);
  JOJ::Tile_Desc& td      = this->tab[td_idex];

  td.rotated  = 0;
  td.mirrored = 0;

  td.x        = x;
  td.y        = y;

  td.dx       = x;
  td.dy       = y;

// ---   *   ---   *   ---

  // get current
  JOJ::Pixel* a=this->get(x,y);

  // walk tiles
  for(uint16_t iy=0;iy<this->cnt;iy++) {
  for(uint16_t ix=0;ix<this->cnt;ix++) {

    // stop at self
    if(ix==x && iy==y) {
      return;

    // skip blank tiles
    } else if(
      this->cleared[this->tile_idex(ix,iy)]

    ) {continue;};

    // get previous
    JOJ::Pixel* b=this->get(ix,iy);

// ---   *   ---   *   ---
// compare against previous

CMP:

    bool same=this->cmp(a,b);

    // no match
    if(!same) {

      // transforms pending, retry
      if(this->match_rotate(td)) {
        goto CMP;

      // transforms exhausted, undo
      } else {
        this->match_undo(td);

      };

// ---   *   ---   *   ---

    // match, end here
    } else if(same) {

      uint64_t ref_idex=this->tile_idex(ix,iy);
      this->tab[ref_idex].used_by.push_back(
        td_idex

      );

      td.x=ix;
      td.y=iy;

      this->clear(x,y);

      return;

    };

  }};

};

// ---   *   ---   *   ---
// move tile to first empty one

void JOJ::Tiles::reloc(
  JOJ::Tile_Desc& td

) {

  uint64_t src_idex=this->tile_idex(td.dx,td.dy);

  if(this->cleared[src_idex]) {
    return;

  };

// ---   *   ---   *   ---

  for(uint16_t y=0;y<this->cnt;y++) {
  for(uint16_t x=0;x<this->cnt;x++) {

    uint64_t dst_idex=this->tile_idex(x,y);

    if(dst_idex==src_idex) {
      return;

    } else if(this->cleared[dst_idex]) {

      this->mov(
        this->get(x,y),
        this->get(td.dx,td.dy)

      );

      for(

        uint64_t ref_idex
      : this->tab[src_idex].used_by

      ) {

        this->tab[ref_idex].x=x;
        this->tab[ref_idex].y=y;

      };

      this->clear(td.dx,td.dy);
      this->cleared[dst_idex]=0;

      td.x=x;
      td.y=y;

      return;

    };

  }};

};

// ---   *   ---   *   ---
// ^whole image

void JOJ::Tiles::reloc_all(void) {

  for(JOJ::Tile_Desc& td : this->tab) {
    this->reloc(td);

  };

};

// ---   *   ---   *   ---
// build table

void JOJ::Tiles::pack(void) {

  for(uint16_t y=0;y<this->cnt;y++) {
  for(uint16_t x=0;x<this->cnt;x++) {
    this->match(x,y);

  }};

  this->reloc_all();

};

// ---   *   ---   *   ---
// rebuilds original image from descriptor table

void JOJ::Tiles::unpack(void) {

  uint64_t img_sz_sq=this->sz*this->cnt;
  img_sz_sq*=img_sz_sq;

  // make copy of image
  std::unique_ptr<JOJ::Pixel> src(
    new JOJ::Pixel[img_sz_sq]

  );

  // go raw
  JOJ::Pixel* src_p=src.get();

  // transfer
  memcpy(
    src_p,this->data.get(),
    img_sz_sq*sizeof(JOJ::Pixel)

  );

  // walk the descriptor table
  for(JOJ::Tile_Desc td : this->tab) {

    JOJ::Pixel* dst=this->get(td.dx,td.dy);
    uint64_t offset=this->real_idex(td.x,td.y);

    this->mov(dst,src_p+offset);

    this->apply_rotation(td);
    this->apply_mirror(td);

  };

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
  uint16_t off_x,
  uint16_t off_y,

  uint32_t fn_idex

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
