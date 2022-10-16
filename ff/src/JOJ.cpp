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

  m_enc=ENCDEF;

};

// ---   *   ---   *   ---

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
// ^read body

void JOJ::read(void) {

//  uint64_t blkcnt = m_meta.img_sz_sq>>2;
//  auto     buff   = Bin::read(hed.idex_sz*blkcnt);
//
//  // get mem
//  m_pixels=std::unique_ptr<JOJ::Pixel>(
//    new JOJ::Pixel[m_meta.img_sz_sq]
//
//  );
//
//// ---   *   ---   *   ---
//// translate palette to color
//
//  JOJ::Pixel* pix_p = m_pixels.get();
//  uint64_t*   data  = (uint64_t*) buff.get();
//
//  uint64_t    bits  = (hed.key_sz<<3)>>2;
//
//// ---   *   ---   *   ---
//// get key && idex size values
//
//  uint64_t mask=(1LL<<bits)-1;
//  mask-=1*!mask;
//
//  uint64_t idex_sz=hed.idex_sz<<3;
//  uint64_t idex_mask=(1LL<<idex_sz)-1;
//  idex_mask-=1*!idex_mask;
//
//  uint64_t bcnt=0;
//
//// ---   *   ---   *   ---
//// walk the buff
//
//  for(
//
//    uint64_t i=0,base=0;
//
//    i<blkcnt;
//    i++,base+=2
//
//  ) {
//
//    // idex of current block
//    uint64_t idex=*data;
//    idex>>=bcnt;
//    idex&=idex_mask;
//
//    // ^fetch from palette
//    uint64_t key=m_meta.pal[idex];
//
//    // idex is variable-sized
//    // adjust buffer accto it
//    bcnt+=idex_sz;
//    if(bcnt==64) {bcnt=0;data++;};
//
//    // skip uneven rows
//    base+=m_meta.img_sz*(
//      base && !(base%m_meta.img_sz)
//
//    );
//
//// ---   *   ---   *   ---
//// decode pixels from block
//
//    JOJ::Pixel* pix[4];
//    this->pixel2x2(pix,base);
//
//    for(int j=0;j<4;j++) {
//
//      pix[j]->from_key(
//        key&mask,
//        m_meta.enc.color
//
//      );
//
//      key>>=bits;
//
//    };
//
//// ---   *   ---   *   ---
//
//  };

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

      rgba2yauv(pixels+orig);

    };

  };

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
