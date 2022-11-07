// ---   *   ---   *   ---
// JOJ
// Do it all over again
//
// LIBRE SOFTWARE
// Licensed under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,

// ---   *   ---   *   ---
// deps

  #include "ff/PNG_Wrap.hpp"

  #include "ff/Zwrap.hpp"
  #include "ff/JOJ.hpp"

// ---   *   ---   *   ---
// png to joj

JOJ::JOJ(
  std::string fpath,
  std::string src_path,

  const char* comp

)

:Bin() {

  this->defopen<JOJ::Header>(
    fpath,
    Bin::WRITE|Bin::TRUNC

  );

// ---   *   ---   *   ---

  m_enc            = ENCDEF;

  m_hed.img_sz     = 0;
  m_next_img       = 0;

  m_src_path       = src_path;
  m_next_type      = 0;

// ---   *   ---   *   ---
// count image components

  for(int i=0;i<4;i++) {

    if(comp[i]==0x7F) {break;};

    m_hed.img_comp[i]=comp[i];
    m_hed.img_comp_cnt++;

  };

};

// ---   *   ---   *   ---
// ^open

JOJ::JOJ(
  std::string fpath

)

:Bin() {

  this->defopen<JOJ::Header>(
    fpath,
    Bin::READ

  );

// ---   *   ---   *   ---

  m_enc=ENCDEF;

  // native format
  Mem<JOJ::Pixel> pixels(m_hed.img_sz_sq);
  m_pixels.copy(pixels);

  // ^to png/glbuff
  Mem<float> raw(m_hed.img_sz_sq<<2);
  m_raw.copy(raw);

  m_tiles.nit(
    JOJ::STD_TILE_SZ,

    m_hed.img_sz,
    m_hed.img_sz_sq

  );

  m_atlas.nit(
    JOJ::STD_TILE_SZ,

    m_hed.atlas_sz,
    m_hed.atlas_sz_sq

  );

};

// ---   *   ---   *   ---
// destroy

JOJ::~JOJ(void) {

  if(m_src_path=="NON") {
    this->rmdump();

  };

};

// ---   *   ---   *   ---
// gets generic dump id

std::string JOJ::get_dump_f(char type) {

  std::string path=

    m_fpath
  + "_dump"
  + std::string(1,type+0x41)
  ;

  return path;

};

// ---   *   ---   *   ---

void JOJ::read_img_table(JOJ::Swap_PTR& swap) {

  auto raw=swap.get();

  Mem<JOJ::Tile_Desc_Packed> m(
    m_tiles.cnt_sq

  );

  uint16_t offset=this->img_idex(raw->idex);

  uint64_t leap=
    m_tiles.cnt_sq
  * sizeof(JOJ::Tile_Desc_Packed)
  ;

  raw->table.seek(leap*offset);
  raw->table.read(&m[0],m.bytesz());

  m_tiles.from_buff(m);

};

// ---   *   ---   *   ---

void JOJ::offset_img_table(JOJ::Swap_PTR& swap) {

  auto raw=swap.get();

  raw->color.seek(0,Bin::END);
  uint64_t offset=raw->color.tell();

  m_leaps.push_back(offset);

  uint64_t prev_tiles=
    offset
  / (sizeof(JOJ::Pixel)*m_atlas.sz_sq)
  ;

  m_tiles.fetch_offset(
    prev_tiles,
    m_atlas.cnt

  );

};

// ---   *   ---   *   ---

void JOJ::read_img(JOJ::Swap_PTR& swap) {

  auto raw=swap.get();
  this->read_img_table(swap);

  uint64_t sz=
    m_tiles.solid_cnt()
  * sizeof(JOJ::Pixel)
  ;

  raw->color.seek(m_leaps[raw->idex],Bin::BEG);
  raw->color.read(m_tiles.get(0,0),sz);

};

// ---   *   ---   *   ---

void JOJ::read_atlas(JOJ::Swap_PTR& swap) {

  auto raw=swap.get();
  raw->color.read(m_atlas.get(0,0));

  Mem<JOJ::Tile_Desc_Packed> m(
    m_atlas.cnt_sq

  );

  raw->atlas.read(&m[0],m.bytesz());
  m_atlas.from_buff(m);

  this->read_img_table(swap);

};

// ---   *   ---   *   ---

void JOJ::owc_atlas(
  JOJ::Swap_PTR& swap

) {

  auto raw=swap.get();

  // overwrite descriptors
  std::string fpath_atlas=raw->atlas.get_fpath();

  raw->atlas.close();
  raw->atlas.open(fpath_atlas,Bin::NEW);

  auto m=m_atlas.to_buff();
  raw->atlas.write(&m[0],m.bytesz());

  // ^same, pixel data
  std::string fpath_color=raw->color.get_fpath();

  raw->color.close();
  raw->color.open(fpath_color,Bin::NEW);

  uint64_t sz=
    m_atlas.solid_cnt()
  * sizeof(JOJ::Pixel)
  ;

  raw->color.write(m_atlas.get(0,0),sz);

};

// ---   *   ---   *   ---

void JOJ::pack_atlas(void) {

  for(

    uint16_t i=0;

    i<m_hed.img_comp_cnt;
    i++

  ) {

    auto swap  = this->swap_to(i,Bin::READ);
    auto raw   = swap.get();

    m_atlas.pack();

// ---   *   ---   *   ---

Mem<JOJ::Pixel> red(m_atlas.sz_sq);
for(uint64_t j=0;j<m_atlas.sz_sq;j++) {
  red[j].from_int(0x80FF7FFF);

};

//memcpy(
//  m_atlas.get(6,1),
//  &red[0],
//
//  m_atlas.sz_sq
//* sizeof(JOJ::Pixel)
//
//);

// ---   *   ---   *   ---

    this->owc_atlas(swap);

  };

};

// ---   *   ---   *   ---

JOJ::Swap_PTR JOJ::make_swapper(
  uint16_t idex,
  char     mode

) {

  JOJ::Swap_PTR out(new JOJ::Swap);
  JOJ::Swap*    raw=out.get();

  raw->idex=idex;
  raw->mode=mode;
  raw->type=this->get_img_type(idex);

  std::string path=this->get_dump_f(
    raw->type

  );

  raw->color.open(path,mode);
  raw->table.open(path+"_table",mode);

  if(mode!=Bin::READ) {
    mode=Bin::NEW;

  };

  raw->atlas.open(path+"_atlas",mode);

  return out;

};

// ---   *   ---   *   ---
// save/load to/from dump

JOJ::Swap_PTR JOJ::swap_to(
  uint16_t idex,
  char     mode

) {

  auto out=this->make_swapper(idex,mode);
  auto raw=out.get();

  // retrieve
  if(mode==Bin::READ) {
    this->read_atlas(out);

  // store
  } else {


    { auto m=m_atlas.to_buff();
      raw->atlas.write(&m[0],m.bytesz());

    };

    this->offset_img_table(out);

    auto m=m_tiles.to_buff();
    raw->table.write(&m[0],m.bytesz());

    uint64_t sz=
      m_tiles.solid_cnt()
    * sizeof(JOJ::Pixel)
    ;

    raw->color.write(m_tiles.get(0,0),sz);

  };

  return out;

};

// ---   *   ---   *   ---
// get NxN chunk

void JOJ::get_tile(uint64_t offset) {

  JOJ::Pixel* dst=&m_tiles.data[0]+offset;
  JOJ::Pixel* src=&m_pixels[0];

// ---   *   ---   *   ---

  for(uint16_t y=0;y<m_tiles.sz;y++) {

    uint64_t idex=sq_idex(
      m_tiles.x,
      y+m_tiles.y,

      m_hed.img_sz

    );

// ---   *   ---   *   ---

    memcpy(

      dst,
      src+idex,

      m_tiles.sz
    * sizeof(JOJ::Pixel)

    );

    dst+=m_tiles.sz;

  };

};

// ---   *   ---   *   ---
// ^whole image

void JOJ::to_tiles(void) {

  m_tiles.x=0;
  m_tiles.y=0;

  uint64_t offset=0;

  for(uint16_t y=0;y<m_tiles.cnt;y++) {
    for(uint16_t x=0;x<m_tiles.cnt;x++) {

      this->get_tile(offset);
      offset+=m_tiles.sz_sq;

      m_tiles.x+=m_tiles.sz;

    };

    m_tiles.x=0;
    m_tiles.y+=m_tiles.sz;

  };

};

// ---   *   ---   *   ---
// set NxN chunk

void JOJ::set_tile(uint64_t offset) {

  JOJ::Pixel* src=&m_tiles.data[0]+offset;
  JOJ::Pixel* dst=&m_pixels[0];

// ---   *   ---   *   ---

  for(uint16_t y=0;y<m_tiles.sz;y++) {

    uint64_t idex=sq_idex(
      m_tiles.x,
      y+m_tiles.y,

      m_hed.img_sz

    );

// ---   *   ---   *   ---

    memcpy(

      dst+idex,
      src,

      m_tiles.sz
    * sizeof(JOJ::Pixel)

    );

    src+=m_tiles.sz;

  };

};

// ---   *   ---   *   ---
// ^copies tiles array to pixel buffer

void JOJ::from_tiles(void) {

  m_tiles.x=0;
  m_tiles.y=0;

  uint64_t offset=0;

  for(uint16_t y=0;y<m_tiles.cnt;y++) {
    for(uint16_t x=0;x<m_tiles.cnt;x++) {

      this->set_tile(offset);
      offset+=m_tiles.sz_sq;

      m_tiles.x+=m_tiles.sz;

    };

    m_tiles.x=0;
    m_tiles.y+=m_tiles.sz;

  };

};

// ---   *   ---   *   ---
// tights up the buffs

void JOJ::pack(void) {

  // run encoder on list of images
  // dumps resulting buff for each
  for(

    uint16_t i=0;

    i<m_hed.img_cnt*m_hed.img_comp_cnt;
    i++

  ) {

    this->read_next_img();
    this->encoder(Frac::ENCODE);

    this->to_tiles();
    m_tiles.pack();

    // always truncate on first write
    char mode=(i<m_hed.img_comp_cnt)
      ? Bin::NEW
      : Bin::WRITE|Bin::APPEND
      ;

    this->swap_to(i,mode);

  };

  this->pack_atlas();

  // ^joins dumps into single file
  this->write();

};

// ---   *   ---   *   ---

void JOJ::write_leaps(void) {

  m_leaps.insert(
    m_leaps.begin(),
    m_leaps.size()

  );

  Bin::write(
    m_leaps.data(),

    m_leaps.size()
  * sizeof(uint64_t)

  );

};

// ---   *   ---   *   ---

void JOJ::read_leaps(void) {

  uint64_t sz=0;
  Bin::read(&sz,sizeof(uint64_t));

  m_leaps.clear();
  m_leaps.resize(sz);

  Bin::read(m_leaps.data(),sz*sizeof(uint64_t));

};

// ---   *   ---   *   ---
// dump to disk

void JOJ::write(void) {

  this->write_header(&m_hed);
  this->write_leaps();

  for(

    uint16_t i=0;

    i<m_hed.img_comp_cnt;
    i++

  ) {

    auto swap = this->make_swapper(i,Bin::READ);
    auto raw  = swap.get();

    raw->atlas.transfer(this);
    raw->table.transfer(this);
    raw->color.transfer(this);

    raw->atlas.nuke();
    raw->table.nuke();
    raw->color.nuke();

  };

};

// ---   *   ---   *   ---
// ^retrieve

void JOJ::unpack(void) {

  uint64_t atlas_sz=
    m_atlas.cnt_sq
  * sizeof(JOJ::Tile_Desc_Packed)
  ;

  uint64_t table_sz=

    m_tiles.cnt_sq
  * sizeof(JOJ::Tile_Desc_Packed)
  * m_hed.img_cnt
  ;

  this->read_leaps();

// ---   *   ---   *   ---

  for(

    uint16_t i=0;

    i<m_hed.img_comp_cnt;
    i++

  ) {

    auto swap = this->make_swapper(i,Bin::NEW);
    auto raw  = swap.get();

    this->transfer(&raw->atlas,atlas_sz);
    this->transfer(&raw->table,table_sz);
    this->transfer(&raw->color);

  };

  m_src_path="NON";

};

// ---   *   ---   *   ---
// ^get rid of retrieved

void JOJ::rmdump(void) {

  for(

    uint16_t i=0;

    i<m_hed.img_comp_cnt;
    i++

  ) {

    auto swap = this->make_swapper(i,Bin::READ);
    auto raw  = swap.get();

    raw->atlas.nuke();
    raw->table.nuke();
    raw->color.nuke();

  };

};

// ---   *   ---   *   ---
// to avoid having the switch pasted everywhere

JOJ::SubEncoding JOJ::read_mode(char type) {

  JOJ::SubEncoding out;

  switch(type) {

  case JOJ::NVEC:
    out=m_enc.normal;
    Frac::Rounding_Mode=Frac::NVEC;

    break;

  case JOJ::YAUV:
    out=m_enc.color;
    Frac::Rounding_Mode=Frac::CORD;

    break;

  case JOJ::ORME:
    out=m_enc.shade;
    Frac::Rounding_Mode=Frac::CORD;

    break;

  // TODO: throw unrecognized type
  default:
    ;

  };

  return out;

};

// ---   *   ---   *   ---
// does/undoes frac'ing of floats

void JOJ::encoder(bool mode) {

  JOJ::Pixel* buff=&m_pixels[0];

// ---   *   ---   *   ---
// transform to and from compressed floats

  struct Frac::Bat<char> batch={

    .m_bytes  = buff[0].as_ptr(),
    .m_floats = &m_raw[0],
    .m_sz     = m_hed.img_sz_sq*4,

    .m_enc    = (char*) m_c_enc.values,
    .m_cnt    = (int*) m_c_enc.cnt,

    .m_mode   = mode,

  };

  batch.encoder();

};

// ---   *   ---   *   ---
// minimal sanity check

void JOJ::chk_img_sz(

  std::string fpath,
  uint16_t    sz

) {

  // get mem
  if(!m_hed.img_sz) {

    m_hed.img_sz    = sz;
    m_hed.img_sz_sq = sz*sz;

    if(!m_hed.atlas_sz) {
      this->atlas_sz(sz);

    };

    // for extracting image files
    Mem<float> raw(m_hed.img_sz_sq<<2);
    m_raw.copy(raw);

    // ^converts into integer format
    Mem<JOJ::Pixel> pixels(m_hed.img_sz_sq);
    m_pixels.copy(pixels);

    m_tiles.nit(
      JOJ::STD_TILE_SZ,

      m_hed.img_sz,
      m_hed.img_sz_sq

    );

    m_atlas.nit(
      JOJ::STD_TILE_SZ,

      m_hed.atlas_sz,
      m_hed.atlas_sz_sq

    );

// ---   *   ---   *   ---
// throw bad size

  } else if(sz*sz!=m_hed.img_sz_sq) {

    printf(
      "Image <%s> has size different "
      "from previous in queue; *.joj "
      "requires same-size images\n",

      fpath.c_str()

    );

    exit(1);

  };

};

// ---   *   ---   *   ---
// handles color conversions

void JOJ::color(float* pixel,bool mode) {

  if(mode==Frac::ENCODE) {

    if(m_c_enc==m_enc.color) {
      rgba2yauv(pixel);

    };

  } else {

    if(m_c_enc==m_enc.color) {
      yauv2rgba(pixel);

    } else if(m_c_enc==m_enc.normal) {
      fnorm(pixel);

    };

  };

};

// ---   *   ---   *   ---
// gets raw yauv buffer from png

void JOJ::from_png(
  std::string name,
  char* type

) {

  std::string fpath=
    m_src_path

  + "/"
  + name

  + std::string(type)
  + ".png"

  ;

  // open image
  PNG im(fpath);

  // run checks
  this->chk_img_sz(fpath,im.get_sz());

  auto buff=im.read();

  // convert
  this->img_cpy_rgba2yauv(
    &m_raw[0],
    &buff[0],

    m_hed.img_sz,
    255.0f

  );

};

// ---   *   ---   *   ---
// fill out internal buffer
// give back ptr to buff

float* JOJ::read_pixels(
  int  idex,
  bool unpack_tiles

) {

  m_c_enc=this->read_mode(
    this->get_img_type(idex)

  );

  // get data dumps
  auto swap = this->swap_to(idex,Bin::READ);
  auto raw  = swap.get();

  m_atlas.unpack();

  if(unpack_tiles) {
    m_tiles.unpack(m_atlas,true);

  } else {
    memcpy(
      m_tiles.get(0,0),
      m_atlas.get(0,0),

      m_hed.img_sz_sq
    * sizeof(JOJ::Pixel)

    );

  };

  this->from_tiles();
  this->encoder(Frac::DECODE);

  return &m_raw[0];

};

// ---   *   ---   *   ---
// from joj format to float

Mem<float> JOJ::to_buff(

  uint16_t idex,
  float    mult

) {

  Mem<float> out(m_hed.img_sz_sq<<2);
  float* pixels = this->read_pixels(idex);

  this->img_cpy_yauv2rgba(
    &out[0],
    pixels,

    m_hed.img_sz,
    mult

  );

  return Mem<float>(out);

};

// ---   *   ---   *   ---
// from joj format to png

void JOJ::to_png(

  uint16_t    idex,
  std::string name

) {

  // make image
  PNG im(name,m_hed.img_sz);

  float* pixels=this->read_pixels(idex,true);
  Mem<uint8_t> buff(m_hed.img_sz_sq<<2);

  this->img_cpy_yauv2rgba(
    &buff[0],
    pixels,

    m_hed.img_sz,
    255.0f

  );

  // dump it out
  im.write(buff);

};

// ---   *   ---   *   ---
// get next "queued" read

void JOJ::read_next_img(void) {

  uint8_t comp_idex=
    m_hed.img_comp[m_next_type++];

  m_c_enc=this->read_mode(comp_idex);

  this->from_png(
    m_img_names[m_next_img],
    (char*) IMG_TYPES[comp_idex]

  );

  // move to next image set
  if(m_next_type==m_hed.img_comp_cnt) {
    m_next_type=0;
    m_next_img++;

  };

};

// ---   *   ---   *   ---
