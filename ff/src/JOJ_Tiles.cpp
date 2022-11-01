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

bool JOJ::Tiles::cmp(
  uint64_t idex_a,
  uint64_t idex_b

) {

  bool out=true;

  idex_a*=this->sz_sq;
  idex_b*=this->sz_sq;

  JOJ::Pixel* pixel=this->data.get();

  for(uint64_t i=0;i<this->sz_sq;i++) {

    if(pixel[idex_a++]!=pixel[idex_b++]) {
      out=false;
      break;

    };

  };

  return out;

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

  case JOJ::Tiles::XFORM_ROR:
    get_next=&JOJ::Tiles::xform_ror;
    break;

  case JOJ::Tiles::XFORM_ROL:
    get_next=&JOJ::Tiles::xform_rol;
    break;

  case JOJ::Tiles::XFORM_XMIR:
    get_next=&JOJ::Tiles::xform_xmir;
    break;

  case JOJ::Tiles::XFORM_YMIR:
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
