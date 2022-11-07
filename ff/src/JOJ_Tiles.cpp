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
    out[i].cleared  = td.cleared;

    out[i].x        = td.x;
    out[i].y        = td.y;

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
    td.cleared  = src[i].cleared;

    td.x        = src[i].x;
    td.y        = src[i].y;

    i++;

  };

};

// ---   *   ---   *   ---
// offsets the next table read

void JOJ::Tiles::fetch_offset(
  uint64_t prev_tiles,
  uint16_t atlas_cnt

) {

  // get origin
  uint32_t xy=rsq_idex(
    prev_tiles,atlas_cnt

  );

  // ^unpack
  uint16_t ox=xy&0xFFFF;
  uint16_t oy=xy>>16;

// ---   *   ---   *   ---
// walk the table and adjust fetches

  for(JOJ::Tile_Desc& td : this->tab) {

    if(td.cleared==CLEAR_NAT) {continue;};

    td.x+=ox;

    while(td.x>=atlas_cnt) {
      td.x-=atlas_cnt;
      td.y++;

    };

    td.y+=oy;

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

  this->tab[this->tile_idex(x,y)].cleared=
    CLEAR_FETCH;

};

// ---   *   ---   *   ---
// true if tile is blank

bool JOJ::Tiles::is_clear(
  JOJ::Pixel* t

) {

  bool out=true;

  for(uint16_t i=0;i<this->sz_sq;i++) {

    if(!t->is_clear()) {
      out=false;
      break;

    };

    t++;

  };

  return out;

};

// ---   *   ---   *   ---
// counts non-transparent tiles

uint64_t JOJ::Tiles::solid_cnt(void) {

  uint64_t out=0;

  for(uint64_t i=0;i<this->cnt_sq;i++) {

    JOJ::Tile_Desc& td=this->tab[i];

    if(

       td.cleared==CLEAR_NAT
    || td.cleared==CLEAR_FETCH

    ) {

      out=i;
      break;

    };

  };

  return out*this->sz_sq;

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
// initializes tile descriptor

JOJ::Tile_Desc& JOJ::Tiles::nit_desc(
  uint16_t x,
  uint16_t y

) {

  uint64_t        td_idex = this->tile_idex(x,y);
  JOJ::Tile_Desc& td      = this->tab[td_idex];

  td.rotated  = ROT_0;
  td.mirrored = MIRROR_NONE;
  td.cleared  = SOLID;

  td.x        = x;
  td.y        = y;

  td.dx       = x;
  td.dy       = y;

  td.used_by.clear();

  return td;

};

// ---   *   ---   *   ---
// get handle to compared tile

JOJ::Pixel* JOJ::Tiles::match_get_ref(
  uint16_t x,
  uint16_t y

) {

  // get tile
  JOJ::Pixel* out  = this->get(x,y);
  uint64_t    idex = this->tile_idex(x,y);

  // skip if blank
  if(this->tab[idex].cleared) {out=NULL;};

  return out;

};

// ---   *   ---   *   ---
// matches two tiles

bool JOJ::Tiles::match_cmp(
  JOJ::Tile_Cmp& mat

) {

  bool same=false;

  while(!same) {

    same=this->cmp(mat.a,mat.b);

    // no match
    if(!same) {

      // transforms pending, retry
      if(this->match_rotate(mat.td)) {
        continue;

      // transforms exhausted, undo and end
      } else {
        this->match_undo(mat.td);
        break;

      };

// ---   *   ---   *   ---
// match, end here

    } else {

      this->clear(mat.td.dx,mat.td.dy);

      mat.td.x=mat.x;
      mat.td.y=mat.y;

      this->tab[
        this->tile_idex(mat.x,mat.y)

      ].used_by.push_back(
        this->tile_idex(mat.td.dx,mat.td.dy)

      );

      break;

    };

  };

  return same;

};

// ---   *   ---   *   ---
// match tile against the rest of the image

void JOJ::Tiles::match(
  uint16_t x,
  uint16_t y

) {

  // get current
  JOJ::Tile_Desc& td  = this->nit_desc(x,y);
  JOJ::Pixel*     a   = this->get(x,y);

  JOJ::Tile_Cmp   mat = {

    .td = td,

    .a  = a,
    .b  = NULL,
    .x  = x,
    .y  = y

  };

  // skip tile if blank
  if(this->is_clear(a)) {
    td.cleared=CLEAR_NAT;
    goto TAIL;

  };

  // walk tiles
  for(uint16_t iy=0;iy<this->cnt;iy++) {
  for(uint16_t ix=0;ix<this->cnt;ix++) {

    JOJ::Pixel* b=this->match_get_ref(ix,iy);

    // stop at self
    if(ix==x && iy==y) {
      goto TAIL;

    // skip blanks
    } else if(b==NULL) {
      continue;

    };

    // compare
    mat.x=ix;
    mat.y=iy;
    mat.b=b;

    // stop if there's a match
    if(this->match_cmp(mat)) {
      goto TAIL;

    };

  }};

TAIL:
  return;

};

// ---   *   ---   *   ---
// move tile to first empty one

void JOJ::Tiles::reloc(
  JOJ::Tile_Desc& td

) {

  uint64_t src_idex=this->tile_idex(td.dx,td.dy);

  if(this->tab[src_idex].cleared) {
    return;

  };

// ---   *   ---   *   ---

  for(uint16_t y=0;y<this->cnt;y++) {
  for(uint16_t x=0;x<this->cnt;x++) {

    uint64_t dst_idex = this->tile_idex(x,y);
    uint8_t  cleared  = this->tab[dst_idex].cleared;

    if(x==td.dx && y==td.dy) {
      return;

    } else if(cleared && cleared!=FAKE_SOLID) {

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

      cleared=(cleared==CLEAR_NAT)
        ? FAKE_SOLID
        : SOLID
        ;

      this->tab[dst_idex].cleared=cleared;

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

void JOJ::Tiles::unpack(
  JOJ::FwdTiles& atlas,
  bool           clear_nat

) {

  JOJ::Pixel* src=atlas.get(0,0);

  // walk the descriptor table
  std::vector<uint64_t> needs_clear;

  for(uint16_t y=0;y<this->cnt;y++) {
  for(uint16_t x=0;x<this->cnt;x++) {

    // fetch tile
    uint64_t        td_idex = this->tile_idex(x,y);
    JOJ::Tile_Desc& td      = this->tab[td_idex];


    // remember transparent
    if(

       td.cleared==FAKE_SOLID
    || td.cleared==CLEAR_NAT

    ) {

      needs_clear.push_back(td_idex);
      if(td.cleared==CLEAR_NAT) {continue;};

    };

    // original position
    td.dx=x;
    td.dy=y;

    // fetch-from
    uint64_t offset=atlas.real_idex(td.x,td.y);

    // fetch-to
    JOJ::Pixel* dst=this->get(x,y);

    // fetch and transform
    this->mov(dst,&src[offset]);

    this->apply_rotation(td);
    this->apply_mirror(td);

  }};

  // blank out transparent tiles
  if(clear_nat) {
  for(uint64_t td_idex : needs_clear) {
    JOJ::Tile_Desc& td=this->tab[td_idex];
    this->clear(td.dx,td.dy);

  }};

};

// ---   *   ---   *   ---
// ^for unpacking the atlas itself

void JOJ::Tiles::unpack(bool clear_nat) {

  JOJ::FwdTiles cpy;

  uint64_t img_sz    = this->cnt*this->sz;
  uint64_t img_sz_sq = img_sz*img_sz;

  cpy.nit(
    this->sz,
    img_sz,
    img_sz_sq

  );

  memcpy(
    cpy.get(0,0),
    this->get(0,0),

    img_sz_sq
  * sizeof(JOJ::Pixel)

  );

  this->unpack(cpy,clear_nat);

};

// ---   *   ---   *   ---
// send duplicate of tile to buffer

Mem<JOJ::Pixel> JOJ::Tiles::copy(
  JOJ::Pixel* pixel

) {

  Mem<JOJ::Pixel> out(this->sz_sq);
  JOJ::Pixel* out_p=&out[0];

  memcpy(
    out_p,pixel,

    this->sz_sq
  * sizeof(JOJ::Pixel)

  );

  return Mem<JOJ::Pixel>(out);

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

  JOJ::Pixel* buff_p=&buff[0];

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

};

// ---   *   ---   *   ---
