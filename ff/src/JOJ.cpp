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

  #include <png++/png.hpp>

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
  m_pixels=std::unique_ptr<JOJ::Pixel>(
    new JOJ::Pixel[m_hed.img_sz_sq]

  );

  // ^to png/glbuff
  m_raw=std::unique_ptr<float>(
    new float[m_hed.img_sz_sq*4]

  );

  m_tiles.nit(
    JOJ::STD_TILE_SZ,

    m_hed.img_sz,
    m_hed.img_sz_sq

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

std::string JOJ::get_dump_f(int idex) {

  std::string path=

    m_fpath
  + "_dump"
  + std::to_string(idex)
  ;

  return path;

};

// ---   *   ---   *   ---
// save/load to/from dump

void JOJ::swap_to(int idex,char mode) {

  std::string path = this->get_dump_f(idex);
  void*       src  = (void*) m_tiles.get(0,0);

  Bin out(path,mode);

  uint64_t sz=
    sizeof(JOJ::Pixel)
  * m_hed.img_sz_sq
  ;

  if(mode==Bin::READ) {

    Mem<JOJ::Tile_Desc_Packed> m(
      m_tiles.cnt_sq

    );

    out.read(&m[0],m.bytesz());
    m_tiles.from_buff(m);

    out.read(src,sz);

  } else {

    Mem<JOJ::Tile_Desc_Packed> m=
      m_tiles.to_buff();

    out.write(&m[0],m.bytesz());
    out.write(src,sz);

  };

};

// ---   *   ---   *   ---
// get NxN chunk

void JOJ::get_tile(uint64_t offset) {

  JOJ::Pixel* dst=m_tiles.data.get()+offset;
  JOJ::Pixel* src=m_pixels.get();

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

  JOJ::Pixel* src=m_tiles.data.get()+offset;
  JOJ::Pixel* dst=m_pixels.get();

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

    int i=0;

    i<m_hed.img_cnt*m_hed.img_comp_cnt;
    i++

  ) {

    this->read_next_img();

    this->encoder(Frac::ENCODE);

    this->to_tiles();
    m_tiles.pack();

    this->swap_to(i,Bin::NEW);

  };

  // ^joins dumps into single file
  this->write();

};

// ---   *   ---   *   ---
// dump to disk

void JOJ::write(void) {

  this->write_header(&m_hed);

  for(

    int i=0;

    i<m_hed.img_cnt*m_hed.img_comp_cnt;
    i++

  ) {

    std::string path=this->get_dump_f(i);

    Bin src(path,Bin::READ);

    src.transfer(this);
    src.nuke();

  };

};

// ---   *   ---   *   ---
// ^retrieve

void JOJ::unpack(void) {

  uint64_t sz=

    m_tiles.cnt_sq
  * sizeof(JOJ::Tile_Desc)

  + m_hed.img_sz_sq
  * sizeof(JOJ::Pixel)

  ;

  for(

    int i=0;

    i<m_hed.img_cnt*m_hed.img_comp_cnt;
    i++

  ) {

    std::string path=this->get_dump_f(i);

    Bin dst(path,Bin::NEW);
    this->transfer(&dst,sz);

  };

  m_src_path="NON";

};

// ---   *   ---   *   ---
// ^get rid of retrieved

void JOJ::rmdump(void) {

  for(

    int i=0;

    i<m_hed.img_cnt*m_hed.img_comp_cnt;
    i++

  ) {

    std::string path=this->get_dump_f(i);

    Bin dst(path,Bin::READ);
    dst.nuke();

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

  JOJ::Pixel* buff=m_pixels.get();

// ---   *   ---   *   ---
// transform to and from compressed floats

  struct Frac::Bat<char> batch={

    .m_bytes  = buff[0].as_ptr(),
    .m_floats = m_raw.get(),
    .m_sz     = m_hed.img_sz_sq*4,

    .m_enc    = (char*) m_c_enc.values,
    .m_cnt    = (int*) m_c_enc.cnt,

    .m_mode   = mode,

  };

  batch.encoder();

};

// ---   *   ---   *   ---
// ensures you fools don't pass non-square
// images through a square image format

void JOJ::chk_img_sz(

  std::string fpath,

  uint64_t width,
  uint64_t height

) {

  // errchk
  if(width!=height) {

    printf(
      "Image <%s> must be square "
      "for *.joj format conversion\n",

      fpath.c_str()

    );

    exit(1);

  };

// ---   *   ---   *   ---
// get mem

  if(!m_hed.img_sz) {

    m_hed.img_sz    = width;
    m_hed.img_sz_sq = width*width;

    // for extracting image files
    m_raw=std::unique_ptr<float>(
      new float[m_hed.img_sz_sq*4]

    );

    // ^converts into integer format
    m_pixels=std::unique_ptr<JOJ::Pixel>(
      new JOJ::Pixel[m_hed.img_sz_sq]

    );

    m_tiles.nit(
      JOJ::STD_TILE_SZ,

      m_hed.img_sz,
      m_hed.img_sz_sq

    );

// ---   *   ---   *   ---
// throw bad size

  } else if(width*height!=m_hed.img_sz_sq) {

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
// because overloading operator ==
// on a struct was too much of a hassle

bool JOJ::enc_compare(
  JOJ::SubEncoding& x

) {

  return

      m_c_enc.values == x.values
  &&  m_c_enc.cnt    == x.cnt

  ;

};

// ---   *   ---   *   ---
// handles color conversions

void JOJ::color(float* pixel,bool mode) {

  if(mode==Frac::ENCODE) {

    if(this->enc_compare(m_enc.color)) {
      rgba2yauv(pixel);

    };

  } else {

    if(this->enc_compare(m_enc.color)) {
      yauv2rgba(pixel);

    } else if(this->enc_compare(m_enc.normal)) {
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
  png::image<png::rgba_pixel> im(fpath);

  // get dims
  uint64_t width  = im.get_width();
  uint64_t height = im.get_height();

  this->chk_img_sz(fpath,width,height);

// ---   *   ---   *   ---
// translate buffer

  float*   pixels = m_raw.get();
  uint64_t i      = 0;

  for(uint64_t y=0;y<height;y++) {

    uint64_t row=y*width;

    for(uint64_t x=0;x<width;x++) {

      png::rgba_pixel px=im.get_pixel(x,y);

      uint64_t orig=i;
      pixels[i++]=px.red/255.0f;
      pixels[i++]=px.green/255.0f;
      pixels[i++]=px.blue/255.0f;
      pixels[i++]=px.alpha/255.0f;

      this->color(pixels+orig,Frac::ENCODE);

    };

  };

};

// ---   *   ---   *   ---
// fill out internal buffer
// give back ptr to buff

float* JOJ::read_pixels(int idex) {

  char type=m_hed.img_comp[
    idex%m_hed.img_comp_cnt

  ];

  m_c_enc=this->read_mode(type);

  // decode joj
  this->swap_to(idex,Bin::READ);

  m_tiles.unpack();
  this->from_tiles();

  this->encoder(Frac::DECODE);

  return m_raw.get();

};

// ---   *   ---   *   ---
// from joj format to float

std::unique_ptr<float>
JOJ::to_buff(int idex,float mult) {

  std::unique_ptr<float> out(
    new float[m_hed.img_sz_sq<<2]

  );

  float* pixels = this->read_pixels(idex);
  float* out_p  = out.get();

// ---   *   ---   *   ---

  uint64_t i=0;

  for(uint64_t y=0;y<m_hed.img_sz;y++) {
    for(uint64_t x=0;x<m_hed.img_sz;x++) {

      this->color(pixels+i,Frac::DECODE);

      out_p[i+0] = pixels[i+0]*mult;
      out_p[i+1] = pixels[i+1]*mult;
      out_p[i+2] = pixels[i+2]*mult;
      out_p[i+3] = pixels[i+3]*mult;

      i+=4;

    };

  };

  return out;

};

// ---   *   ---   *   ---
// from joj format to png

void JOJ::to_png(
  int idex,
  std::string name

) {

  // make image
  png::image<png::rgba_pixel> im(
    m_hed.img_sz,m_hed.img_sz

  );

  float* pixels=this->read_pixels(idex);

// ---   *   ---   *   ---
// transform color

  uint64_t i=0;

  for(uint64_t y=0;y<m_hed.img_sz;y++) {
    for(uint64_t x=0;x<m_hed.img_sz;x++) {

      png::rgba_pixel px;

      this->color(pixels+i,Frac::DECODE);

      px.red   = pixels[i++]*255.0f;
      px.green = pixels[i++]*255.0f;
      px.blue  = pixels[i++]*255.0f;
      px.alpha = pixels[i++]*255.0f;

      im[y][x]=px;

    };

  };

  // dump it out
  im.write(name);

};

// ---   *   ---   *   ---

void JOJ::to_png(
  float* pixels,
  std::string name

) {

  // make image
  png::image<png::rgba_pixel> im(
    m_hed.img_sz,m_hed.img_sz

  );

// ---   *   ---   *   ---
// transform color

  uint64_t i=0;

  for(uint64_t y=0;y<m_hed.img_sz;y++) {
    for(uint64_t x=0;x<m_hed.img_sz;x++) {

      png::rgba_pixel px;

      px.red   = pixels[i++];
      px.green = pixels[i++];
      px.blue  = pixels[i++];
      px.alpha = pixels[i++];

      im[y][x]=px;

    };

  };

  // dump it out
  im.write(name);

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
