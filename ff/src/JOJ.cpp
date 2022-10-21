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
  std::string src_path

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

  std::string path=this->get_dump_f(idex);

  Bin out(path,mode);

  void* dst=(void*) m_pixels.get();

  uint64_t sz=
    sizeof(JOJ::Pixel)
  * m_hed.img_sz_sq
  ;

  if(mode==Bin::READ) {
    out.read(dst,sz);

  } else {
    out.write(dst,sz);

  };

};

// ---   *   ---   *   ---
// tights up the buffs

void JOJ::pack(void) {

  // run encoder on list of images
  // dumps resulting buff for each
  for(int i=0;i<m_hed.img_cnt*3;i++) {

    this->read_next_img();

    this->encoder(Frac::ENCODE);
    this->swap_to(i,Bin::NEW);

  };

  // ^joins dumps into single file
  this->write();

};

// ---   *   ---   *   ---
// dump to disk

void JOJ::write(void) {

  this->write_header(&m_hed);

  for(int i=0;i<m_hed.img_cnt*3;i++) {

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
    m_hed.img_sz_sq
  * sizeof(JOJ::Pixel)
  ;

  for(int i=0;i<m_hed.img_cnt*3;i++) {

    std::string path=this->get_dump_f(i);

    Bin dst(path,Bin::NEW);
    this->transfer(&dst,sz);

  };

  m_src_path="NON";

};

// ---   *   ---   *   ---
// ^get rid of retrieved

void JOJ::rmdump(void) {

  for(int i=0;i<m_hed.img_cnt*3;i++) {

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

  char type=idex%3;
  m_c_enc=this->read_mode(type);

  // decode joj
  this->swap_to(idex,Bin::READ);
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

  m_c_enc=this->read_mode(m_next_type);

  this->from_png(
    m_img_names[m_next_img],
    (char*) IMG_TYPES[m_next_type++]

  );

  // move to next image set
  if(m_next_type==3) {
    m_next_type=0;
    m_next_img++;

  };

};

// ---   *   ---   *   ---
