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
  + char(type+0x41)
  ;

  return path;

};

// ---   *   ---   *   ---

void JOJ::read_img_table(
  uint16_t idex

) {

  m_tiles.tab.clear();
  m_tiles.tab=m_tab[idex];

};

// ---   *   ---   *   ---

void JOJ::offset_img_table(JOJ::Swap_PTR& swap) {

  auto raw=swap.get();

  raw->color.seek(0,Bin::END);
  uint64_t offset=raw->color.tell();

  uint64_t prev_tiles=
    offset
  / (sizeof(JOJ::Pixel)*m_atlas.sz_sq)
  ;

  m_tiles.fetch_offset(

    m_atlas,

    raw->idex, // %m_hed.img_cnt
    prev_tiles

  );

};

// ---   *  ---   *   ---

void JOJ::read_atlas(JOJ::Swap_PTR& swap) {

  auto raw=swap.get();
  raw->color.read(m_atlas.get(0,0));

  Mem<uint8_t> m(raw->atlas.get_size());

  raw->atlas.read(&m[0],m.bytesz());
  m_atlas.from_buff(m);

};

// ---   *   ---   *   ---
// atlas desc to bytearray

Mem<uint8_t> JOJ::atlas_desc_to_buff(
  uint16_t idex

) {

  uint16_t limit=
    m_hed.img_comp_cnt
  * m_hed.img_cnt
  ;

  uint64_t sz=0;
  std::vector<uint16_t> hed;

  hed.resize(m_hed.img_cnt+1);

// ---   *   ---   *   ---
// get sizes for whole table

  for(

    uint16_t i=idex,j=0;

    i<limit;
    i+=m_hed.img_comp_cnt,j++

  ) {

    sz+=
      m_tab[i].size()
    * sizeof(m_tab[0][0])
    ;

    hed[j]=m_tab[i].size();

  };

  uint16_t tile_cnt=
    m_atlas.solid_cnt()
  / m_atlas.sz_sq
  ;

  uint16_t tile_sz=
    tile_cnt
  * sizeof(m_tab[0][0])
  ;

  hed[m_hed.img_cnt]=tile_cnt;

// ---   *   ---   *   ---
// write sizes to buffer

  uint64_t beg=
    hed.size()
  * sizeof(hed[0])
  ;

  Mem<uint8_t> out(beg+sz+tile_sz);
  out.write(hed.data(),beg,0);

// ---   *   ---   *   ---
// write table entries to buffer

  for(

    uint16_t i=idex;

    i<limit;
    i+=m_hed.img_comp_cnt

  ) {

    uint64_t leap=
      m_tab[i].size()
    * sizeof(m_tab[0][0])
    ;

    out.write(m_tab[i].data(),leap,beg);
    beg+=leap;

  };

// ---   *   ---   *   ---
// save atlas meta

  out.write(
    m_atlas.tab.data(),

    tile_sz,
    beg

  );

  return Mem<uint8_t>(out);

};

// ---   *   ---   *   ---
// bytearray to atlas desc

void JOJ::buff_to_atlas_desc(
  uint16_t idex

) {

  uint16_t limit=
    m_hed.img_comp_cnt
  * m_hed.img_cnt
  ;

  uint64_t hed_sz=
    (m_hed.img_cnt+1)
  * sizeof(uint16_t)
  ;

  auto m=Bin::read(hed_sz);

// ---   *   ---   *   ---
// get entries

  for(

    uint16_t j=idex,h=0;

    j<limit;
    j+=m_hed.img_comp_cnt,h+=2

  ) {

    uint16_t cnt=*((uint16_t*) &m[h]);

    m_tab[j].resize(cnt);

    Bin::read(
      (void*) m_tab[j].data(),
      cnt*sizeof(m_tab[0][0])

    );

  };

// ---   *   ---   *   ---
// get atlas meta

  uint16_t top=(m_hed.img_cnt)*2;
  uint16_t cnt=*((uint16_t*) &m[top]);
  top=limit+idex;

  m_tab[top].resize(cnt);

  uint64_t guilt_of_bjarne=
    cnt*sizeof(m_tab[0][0]);

  Mem<uint8_t> sins_of_bjarne(
    cnt*sizeof(typeof(m_tab[0][0]))

  );

  Bin::read(
    &sins_of_bjarne[0],
    guilt_of_bjarne

  );

  auto* jail_of_bjarne=
    (typeof(m_tab[0][0])*) &sins_of_bjarne[0];

  for(uint16_t i=0;i<cnt;i++) {
    m_tab[top][i].wcopy(jail_of_bjarne[i]);

  };

  m_atlas.tab.clear();
  m_atlas.tab=m_tab[top];

};

// ---   *   ---   *   ---

void JOJ::owc_atlas(
  JOJ::Swap_PTR& swap

) {

  auto raw=swap.get();

  // overwrite descriptors
  std::string fpath_atlas=
    raw->atlas.get_fpath();

  raw->atlas.close();
  raw->atlas.open(fpath_atlas,Bin::NEW);

  auto m=this->atlas_desc_to_buff(
    raw->idex

  );

  raw->atlas.write(&m[0],m.used());

  // ^same, pixel data
  std::string fpath_color=
    raw->color.get_fpath();

  raw->color.close();
  raw->color.open(fpath_color,Bin::NEW);

  uint64_t sz=
    m_atlas.solid_cnt()
  * sizeof(JOJ::Pixel)
  ;

  m_hed.atlas_solid[raw->idex]=sz;

  raw->color.write(
    m_atlas.get(0,0),sz

  );

};

// ---   *   ---   *   ---
// returns tile descriptors

void JOJ::build_img_table(
  uint16_t idex

) {

  for(auto& td : m_atlas.image[idex]) {

    if(

       td.cleared==JOJ::Tiles::CLEAR_NAT
    || td.cleared==JOJ::Tiles::FAKE_SOLID

    ) {continue;};

    m_tab[idex].push_back(td);

  };

};

// ---   *   ---   *   ---

void JOJ::pack_atlas(void) {

  uint16_t limit=
    m_hed.img_comp_cnt
  * m_hed.img_cnt
  ;

  m_tab.resize(limit+m_hed.img_comp_cnt);

  for(

    uint16_t i=0;

    i<m_hed.img_comp_cnt;
    i++

  ) {


    m_atlas.clear_users();

    auto swap = this->swap_to(i,Bin::READ);
    auto raw  = swap.get();

    m_atlas.pack(JOJ::Tiles::XFORM_APPLY);

    for(

      uint16_t j=i;

      j<limit;
      j+=m_hed.img_comp_cnt

    ) {

      this->build_img_table(j);

    };

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

    this->offset_img_table(out);

    auto m=m_atlas.to_buff();
    raw->atlas.write(&m[0],m.used());

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

void JOJ::set_tile(
  JOJ::Tiles& tiles,
  uint64_t    offset

) {

  JOJ::Pixel* src=&tiles.data[0]+offset;
  JOJ::Pixel* dst=&m_pixels[0];

// ---   *   ---   *   ---

  for(uint16_t y=0;y<tiles.sz;y++) {

    uint64_t idex=sq_idex(
      tiles.x,
      y+tiles.y,

      tiles.cnt*tiles.sz

    );

// ---   *   ---   *   ---

    memcpy(

      dst+idex,
      src,

      tiles.sz
    * sizeof(JOJ::Pixel)

    );

    src+=tiles.sz;

  };

};

// ---   *   ---   *   ---
// ^copies tiles array to pixel buffer

void JOJ::from_tiles(JOJ::Tiles& tiles) {

  tiles.x=0;
  tiles.y=0;

  uint64_t offset=0;

  for(uint16_t y=0;y<tiles.cnt;y++) {
    for(uint16_t x=0;x<tiles.cnt;x++) {

      this->set_tile(tiles,offset);
      offset+=tiles.sz_sq;

      tiles.x+=tiles.sz;

    };

    tiles.x=0;
    tiles.y+=tiles.sz;

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

    if(m_c_enc==m_enc.normal) {
      this->nc_encoder(Frac::ENCODE);

    } else {
      this->encoder(Frac::ENCODE);

    };

    this->to_tiles();
    m_tiles.pack(JOJ::Tiles::XFORM_SKIP);

    // always truncate on first write
    char mode=(i<m_hed.img_comp_cnt)
      ? Bin::NEW
      : Bin::WRITE|Bin::APPEND
      ;

    this->swap_to(i,mode);

  };

  this->pack_atlas();
  this->write();

};

// ---   *   ---   *   ---
// dump to disk

void JOJ::write(void) {

  this->write_header(&m_hed);

  for(

    uint16_t i=0;

    i<m_hed.img_comp_cnt;
    i++

  ) {

    auto swap = this->make_swapper(i,Bin::READ);
    auto raw  = swap.get();

    raw->atlas.transfer(*this);
    raw->color.transfer(*this);

  };

  this->rmdump();

};

// ---   *   ---   *   ---
// ^retrieve

void JOJ::unpack(void) {

  uint16_t limit=
    m_hed.img_comp_cnt
  * m_hed.img_cnt
  ;

  m_tab.resize(limit+m_hed.img_comp_cnt);

  for(

    uint16_t i=0;

    i<m_hed.img_comp_cnt;
    i++

  ) {

    auto swap = this->make_swapper(i,Bin::NEW);
    auto raw  = swap.get();

    this->buff_to_atlas_desc(i);
    this->transfer(
      raw->color,
      m_hed.atlas_solid[i]

    );

  };

  m_src_path="NON";;

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

    raw->atlas.unlink();
    raw->color.unlink();

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

void JOJ::encoder(
  bool     mode,
  uint64_t sz

) {

  JOJ::Pixel* buff=&m_pixels[0];
  sz+=m_hed.img_sz_sq * (! sz);

// ---   *   ---   *   ---
// transform to and from compressed floats

  struct Frac::Bat<char> batch={

    .m_bytes  = buff[0].as_ptr(),
    .m_floats = &m_raw[0],
    .m_sz     = sz*4,

    .m_enc    = (char*) m_c_enc.values,
    .m_cnt    = (int*) m_c_enc.cnt,

    .m_mode   = mode,

  };

  batch.encoder();

};

// ---   *   ---   *   ---
// handles conversion of nc maps
// (normal+curvature)

void JOJ::nc_encoder(
  bool     mode,
  uint64_t sz

) {

  sz+=m_hed.img_sz_sq * (! sz);

  auto branch_to=(mode==Frac::DECODE)
    ? &JOJ::nc_unpack
    : &JOJ::nc_pack
    ;

  for(uint64_t i=0;i<sz;i++) {
    (this->*branch_to)(i);

  };

};

// ---   *   ---   *   ---
// ^gutsof

void JOJ::nc_pack(uint64_t i) {
  uint32_t v=JOJ::nseph().nc_pack(&m_raw[i*4]);
  m_pixels[i].from_int(v&0xFFFFFFFF);

};

void JOJ::nc_unpack(uint64_t i) {

  JOJ::nseph().nc_unpack(
    &m_raw[i*4],
    uint64_t(m_pixels[i].as_int())

  );

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

    uint32_t entry_cnt=
      m_hed.img_cnt
    * m_hed.img_comp_cnt
    ;

    m_atlas.nit(
      JOJ::STD_TILE_SZ,

      m_hed.atlas_sz,
      m_hed.atlas_sz_sq,

      m_tiles.cnt | (entry_cnt<<16)

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

    } else if(m_c_enc==m_enc.normal) {
      nc_discard_chk(pixel);

    } else {
      orme_discard_chk(pixel);

    };

  } else {

    if(m_c_enc==m_enc.color) {
      yauv2rgba(pixel);

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
  this->img_cpy_rgba2joj(
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

  char type = this->get_img_type(idex);
  m_c_enc   = this->read_mode(type);

  std::string path=
    this->get_dump_f(type);

  Bin color(path,Bin::READ);
  color.read(m_atlas.get(0,0));

  // get single image
  if(unpack_tiles) {

    if(m_atlas_mode) {

      Mem<JOJ::Pixel> pixels(m_hed.img_sz_sq);
      m_pixels.copy(pixels);

      Mem<float> raw(m_hed.img_sz_sq<<2);
      m_raw.copy(raw);

      m_atlas_mode=false;

    };

    this->read_img_table(idex);
    m_tiles.unpack(m_atlas);

    this->from_tiles(m_tiles);

    if(m_c_enc==m_enc.normal) {
      this->nc_encoder(Frac::DECODE);

    } else {
      this->encoder(Frac::DECODE);

    };

    m_tiles.clear_all();

  // get the entire atlas
  } else {

    if(! m_atlas_mode) {

      Mem<JOJ::Pixel> pixels(m_hed.atlas_sz_sq);
      m_pixels.copy(pixels);

      Mem<float> raw(m_hed.atlas_sz_sq<<2);
      m_raw.copy(raw);

      m_atlas_mode=true;

    };

    this->from_tiles(m_atlas);

    if(m_c_enc==m_enc.normal) {
      nc_encoder(
        Frac::DECODE,
        m_hed.atlas_sz_sq

      );

    } else {
      this->encoder(
        Frac::DECODE,
        m_hed.atlas_sz_sq

      );

    };

  };

  return &m_raw[0];

};

// ---   *   ---   *   ---
// from joj format to float

Mem<float> JOJ::to_buff(

  uint16_t idex,

  bool     mode,
  float    mult

) {

  uint64_t sz=(mode==JOJ::UNPACK_ATLAS)
    ? m_hed.atlas_sz
    : m_hed.img_sz
    ;

  uint64_t sz_sq=sz*sz;

  Mem<float> out(sz_sq<<2);
  float* pixels=this->read_pixels(
    idex,mode==JOJ::UNPACK_IMAGE

  );

  this->img_cpy_joj2rgba(
    &out[0],
    pixels,

    sz,
    mult

  );

  return Mem<float>(out);

};

// ---   *   ---   *   ---
// from joj format to png

void JOJ::to_png(

  uint16_t    idex,
  std::string name,

  bool        mode

) {

  uint64_t sz=(mode==JOJ::UNPACK_ATLAS)
    ? m_hed.atlas_sz
    : m_hed.img_sz
    ;

  uint64_t sz_sq=sz*sz;

  // make image
  PNG im(name+".png",sz);

  float* pixels=this->read_pixels(
    idex,mode==JOJ::UNPACK_IMAGE

  );

  Mem<uint8_t> buff(sz_sq<<2);

  this->img_cpy_joj2rgba(
    &buff[0],
    pixels,

    sz,
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
