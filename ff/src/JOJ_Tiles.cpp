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

Mem<uint8_t> JOJ::Tiles::to_buff(void) {

  uint64_t _sz=
    this->cnt_sq
  * sizeof(uint64_t)
  ;

  for(uint64_t i=0;i<this->cnt_sq;i++) {

    _sz+=
      this->users[i].size()
    * sizeof(JOJ::Tile_Ref)
    ;

  };

// ---   *   ---   *   ---

  Mem<uint8_t> out(_sz);

  uint64_t i=0;
  for(uint64_t i=0,j=0;i<this->cnt_sq;i++) {

    auto src      = this->users[i];
    uint64_t _cnt = src.size();

    out.write(&_cnt,sizeof(uint64_t),j);
    j+=sizeof(uint64_t);

    uint64_t leap=
      _cnt
    * sizeof(JOJ::Tile_Ref)
    ;

    if(leap) {

      out.write(src.data(),leap,j);
      j+=leap;

    };

  };

  return Mem<uint8_t>(out);

};

// ---   *   ---   *   ---
// ^get tiles from mem

void JOJ::Tiles::from_buff(
  Mem<uint8_t>& src

) {

  for(uint64_t i=0,j=0;i<this->cnt_sq;i++) {

    uint64_t _cnt=*((uint64_t*) &src[j]);
    j+=sizeof(uint64_t);

    uint64_t leap=
      _cnt
    * sizeof(JOJ::Tile_Ref)
    ;

    if(leap) {

      this->users[i].resize(_cnt);

      memcpy(

        this->users[i].data(),
        &src[j],

        leap

      );

      j+=leap;

    };

  };

};

// ---   *   ---   *   ---
// offsets the next table read

void JOJ::Tiles::fetch_offset(

  JOJ::Tiles& atlas,

  uint16_t       img_idex,
  uint64_t       prev_tiles

) {

  // get origin
  uint32_t xy=rsq_idex(
    prev_tiles,atlas.cnt

  );

  // ^unpack
  uint16_t ox=xy&0xFFFF;
  uint16_t oy=xy>>16;

// ---   *   ---   *   ---
// walk the table and adjust fetches

  for(auto& td : this->tab) {

    if(

       td.cleared==CLEAR_NAT
    || td.cleared==FAKE_SOLID

    ) {continue;};

    while(ox>=atlas.cnt) {
      ox-=atlas.cnt;
      oy++;

    };

    td.x=ox++;
    td.y=oy;

    uint64_t td_idex=atlas.tile_idex(td.x,td.y);

    auto ref=JOJ::Tile_Ref(
      img_idex,
      this->tile_idex(td.dx,td.dy)

    );

    if(td_idex>=atlas.cnt_sq) {
      fprintf(stderr,"ATLAS SIZE TOO SMALL");
      exit(1);

    };

    atlas.users[td_idex].push_back(ref);

  };

  memcpy(

    atlas.image[img_idex].data(),
    this->tab.data(),

    this->cnt_sq
  * sizeof(JOJ::Tile_Desc)

  );

};

// ---   *   ---   *   ---
// discard contents of tile

void JOJ::Tiles::clear(
  uint16_t _x,
  uint16_t _y,

  uint8_t  flag

) {

  JOJ::Pixel* tile = this->get(_x,_y);
  for(uint16_t i=0;i<this->sz_sq;i++) {
    tile->clear();
    tile++;

  };

  this->tab[this->tile_idex(_x,_y)].cleared=flag;

};

void JOJ::Tiles::clear_all(void) {

  for(auto& td : this->tab) {
    this->clear(td.dx,td.dy,td.cleared);

  };

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

  for(auto& td : this->tab) {

    if(

       td.cleared==CLEAR_NAT
    || td.cleared==CLEAR_FETCH

    ) {break;};

    out++;

  };

  return out*this->sz_sq;

};

// ---   *   ---   *   ---
// true if tiles are identical

bool JOJ::Tiles::cmp(
  JOJ::Pixel* a,
  JOJ::Pixel* b

) {

  return ! memcmp(

    a,b,

    this->sz_sq
  * sizeof(*a)

  );

};

// ---   *   ---   *   ---
// generate data identifier
// for a particular tile
//
// used to detect viable
// candidates for xform

uint64_t JOJ::Tiles::gen_xorkey(
  JOJ::Pixel* a

) {

  uint64_t out=0;
  for(uint64_t i=0;i < this->sz_sq;i+=2) {

    JOJ::Pixel& p0=a[i+0];
    JOJ::Pixel& p1=a[i+1];

    out^=
      (uint64_t(p0.as_int()) <<  0)
    | (uint64_t(p1.as_int()) << 32)
    ;

  };

  return out;

};

// ---   *   ---   *   ---
// ^batch, gen for whole image

void JOJ::Tiles::map_xorkey(void) {

  // walk tiles
  for(uint16_t iy=0;iy<this->cnt;iy++) {
  for(uint16_t ix=0;ix<this->cnt;ix++) {

    // get tile descriptor
    auto td_idex=this->tile_idex(ix,iy);

    // get data
    JOJ::Tile_Desc& td = this->tab[td_idex];
    JOJ::Pixel*     a  = this->get(ix,iy);

    uint64_t key=this->gen_xorkey(a);
    xorkeys[ix + (iy*this->cnt)]=key;

  }};

};

// ---   *   ---   *   ---
// ^fetch generated

uint64_t JOJ::Tiles::get_xorkey(
  uint16_t ix,
  uint16_t iy

) {

  return xorkeys[ix + (iy*this->cnt)];

};

// ---   *   ---   *   ---
// ^compare keys of two tiles

bool JOJ::Tiles::cmp_xorkey(

  // a
  uint16_t x0,
  uint16_t y0,

  // b
  uint16_t x1,
  uint16_t y1

) {

  return
     this->get_xorkey(x0,y0)
  == this->get_xorkey(x1,y1)
  ;

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
    this->xmir(td.dx,td.dy);
    td.mirrored=MIRROR_X;

    break;

  case MIRROR_X:
    this->xmir(td.dx,td.dy);
    this->ymir(td.dx,td.dy);
    td.mirrored=MIRROR_Y;

    break;

// ---   *   ---   *   ---
// try last combination

  case MIRROR_Y:
    this->xmir(td.dx,td.dy);
    td.mirrored=MIRROR_XY;

    break;

// ---   *   ---   *   ---
// undo and break loop

  case MIRROR_XY:
    this->xmir(td.dx,td.dy);
    this->ymir(td.dx,td.dy);
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
  this->ror(td.dx,td.dy);
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
    this->rol(td.dx,td.dy);
    break;

  case ROT_180:
    this->ror(td.dx,td.dy);
    this->ror(td.dx,td.dy);
    break;

  case ROT_240:
    this->ror(td.dx,td.dy);
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
  uint16_t _x,
  uint16_t _y

) {

  uint64_t        td_idex = this->tile_idex(_x,_y);
  JOJ::Tile_Desc& td      = this->tab[td_idex];

  td.rotated  = ROT_0;
  td.mirrored = MIRROR_NONE;
  td.cleared  = SOLID;

  td.x        = _x;
  td.y        = _y;

  td.dx       = _x;
  td.dy       = _y;

  td.users.clear();

  return td;

};

// ---   *   ---   *   ---
// get handle to compared tile

JOJ::Pixel* JOJ::Tiles::match_get_ref(
  uint16_t _x,
  uint16_t _y

) {

  // get tile
  JOJ::Pixel* out  = this->get(_x,_y);
  uint64_t    idex = this->tile_idex(_x,_y);

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

  while(! same) {

    same=this->cmp(mat.a,mat.b);

    // no match
    if(! same) {

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

      uint64_t idex=this->tile_idex(
        mat.td.dx,mat.td.dy

      );

      // add this tile to target's users
      this->tab[
        this->tile_idex(mat.x,mat.y)

      ].users.push_back(idex);

      // notify users of this tile
      this->reloc_users(
        idex,mat.x,mat.y

      );

      break;

    };

  };

  return same;

};

// ---   *   ---   *   ---
// match tile against the rest of the image

void JOJ::Tiles::match(

  uint16_t _x,
  uint16_t _y

) {

  // get tile descriptor
  auto td_idex=this->tile_idex(_x,_y);

  // get data
  JOJ::Tile_Desc& td = this->tab[td_idex];
  JOJ::Pixel*     a  = this->get(_x,_y);

  if(td.cleared) {
    return;

  };

  // make comparison helper
  JOJ::Tile_Cmp mat={

    .td = td,

    .a  = a,
    .b  = NULL,
    .x  = _x,
    .y  = _y

  };

  // walk tiles
  for(uint16_t iy=0;iy<this->cnt;iy++) {
  for(uint16_t ix=0;ix<this->cnt;ix++) {

    JOJ::Pixel* b=this->match_get_ref(ix,iy);

    // stop at self
    if(ix==_x && iy==_y) {
      goto TAIL;

    // skip blanks
    } else if(b==NULL) {
      continue;

    // only attempt matches on
    // tiles with the same bytes
    } else if(! this->cmp_xorkey(_x,_y,ix,iy)) {
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

void JOJ::Tiles::pre_proc(

  uint16_t _x,
  uint16_t _y

) {

  // get current
  JOJ::Tile_Desc& td  = this->nit_desc(_x,_y);
  JOJ::Pixel*     a   = this->get(_x,_y);

  // mark blank tile
  if(this->is_clear(a)) {
    td.cleared=CLEAR_NAT;

  };

};

// ---   *   ---   *   ---

void JOJ::Tiles::reloc_users(

  uint64_t idex,

  uint16_t _x,
  uint16_t _y

) {

  auto& td=this->tab[idex];

  for(uint64_t ref_idex : td.users) {

    auto& dst=this->tab[ref_idex];

    dst.x=_x;
    dst.y=_y;

    dst.rotated  = td.rotated;
    dst.mirrored = td.mirrored;

  };

  if(this->users.size()) {

    std::vector<JOJ::Tile_Ref>& src=
      this->users[idex];

    for(JOJ::Tile_Ref& ref : src) {

      auto& dst=
        this->image[ref.img][ref.tile];

      dst.x=_x;
      dst.y=_y;

      dst.rotated  = td.rotated;
      dst.mirrored = td.mirrored;

    };

  };

};

// ---   *   ---   *   ---
// move tile to first empty one

void JOJ::Tiles::reloc(
  JOJ::Tile_Desc& td

) {

  uint64_t src_idex=
    this->tile_idex(td.dx,td.dy);

  if(this->tab[src_idex].cleared) {
    return;

  };

// ---   *   ---   *   ---

  uint16_t x_avail=0;
  uint16_t y_avail=0;

  for(auto& dst : this->tab) {

    uint64_t dst_idex=this->tile_idex(
      x_avail,y_avail

    );

    uint8_t cleared=
      this->tab[dst_idex].cleared;

// ---   *   ---   *   ---

    if(dst_idex==src_idex) {
      break;

    } else if(cleared && cleared!=FAKE_SOLID) {

      this->mov(
        this->get(x_avail,y_avail),
        this->get(td.dx,td.dy)

      );

// ---   *   ---   *   ---

      if(cleared==CLEAR_FETCH) {

        this->reloc_users(

          dst_idex,

          this->tab[dst_idex].x,
          this->tab[dst_idex].y

        );

      };

      this->reloc_users(

        src_idex,

        x_avail,
        y_avail

      );

      this->clear(td.dx,td.dy);

      cleared=(cleared==CLEAR_NAT)
        ? FAKE_SOLID
        : SOLID
        ;

      this->tab[dst_idex].cleared=cleared;

      td.x=x_avail;
      td.y=y_avail;

      break;

    };

// ---   *   ---   *   ---

    x_avail++;

    if(x_avail==this->cnt) {
      x_avail=0;
      y_avail++;

    };

  };

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

void JOJ::Tiles::pack(
  bool skip_match

) {

  if(! skip_match) {
    this->map_xorkey();

  };

  for(uint16_t _y=0;_y<this->cnt;_y++) {
  for(uint16_t _x=0;_x<this->cnt;_x++) {

    this->pre_proc(_x,_y);

    if(! skip_match) {
      this->match(_x,_y);

    };

    auto& td=this->tab[this->tile_idex(_x,_y)];
    this->reloc(td);

  }};

};

// ---   *   ---   *   ---
// rebuilds original image from descriptor table

void JOJ::Tiles::unpack(
  JOJ::Tiles& atlas

) {

  JOJ::Pixel* src=atlas.get(0,0);

  // walk the descriptor table
  for(auto& td : this->tab) {

    // fetch-from
    uint64_t offset=atlas.real_idex(td.x,td.y);

    // fetch-to
    JOJ::Pixel* dst=this->get(td.dx,td.dy);

    // fetch and transform
    this->mov(dst,&src[offset]);

    this->undo_rotation(td);
    this->undo_mirror(td);

  };

};

// ---   *   ---   *   ---
// send duplicate of tile to buffer

void JOJ::Tiles::copy(
  JOJ::Pixel* pixel

) {

  memcpy(
    &copy_tile[0],pixel,

    this->sz_sq
  * sizeof(JOJ::Pixel)

  );

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

  auto pixel=this->get(off_x,off_y);
  this->copy(pixel);

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
    pixel[dst_idex]=copy_tile[src_idex];

  }};

};

// ---   *   ---   *   ---
