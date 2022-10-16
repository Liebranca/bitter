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

  #include "kvrnel/Bytes.hpp"

  #include "ff/Zwrap.hpp"
  #include "ff/JOJ.hpp"

// ---   *   ---   *   ---
// float buffer to *.joj

JOJ::JOJ(

  std::string fpath,

  float* pixels,
  uint64_t sz,

  JOJ::Encoding enc

)

:Bin() {

  this->defopen<JOJ::Header>(
    fpath,
    Bin::WRITE|Bin::TRUNC

  );

// ---   *   ---   *   ---

  m_raw            = pixels;
  m_enc            = enc;

  m_hed.img_sz     = fast_sqrt2(sz);

  // remember size squared
  m_hed.img_sz_sq  = sz;

  // main data buffer
  m_pixels=std::unique_ptr<JOJ::Pixel>(
    new JOJ::Pixel[sz]

  );

  // ^compressed version
  m_blocks=std::unique_ptr<uint64_t>(
    new uint64_t[sz>>2]

  );

  // for pixel components
  m_pal[0]=PAL("./pal_2bit",Bin::NEW,32);
  m_pal[1]=PAL("./pal_3bit",Bin::NEW,32);
  m_pal[2]=PAL("./pal_4bit",Bin::NEW,32);

  // for pixel blocks
  m_pal[3]=PAL(
    "./pal_16bit",
    Bin::NEW,

    m_hed.img_sz<<4

  );

};

// ---   *   ---   *   ---
// ^from file to *.joj

JOJ::JOJ(

  std::string fpath,
  float* pixels,

  JOJ::Encoding enc

)

:Bin() {

  this->defopen<JOJ::Header>(
    fpath,
    Bin::READ

  );

  m_raw=pixels;
  m_enc=enc;

  m_pal[0]=PAL("./pal_2bit",Bin::READ);
  m_pal[1]=PAL("./pal_3bit",Bin::READ);
  m_pal[2]=PAL("./pal_4bit",Bin::READ);

  m_pal[3]=PAL("./pal_16bit",Bin::READ);

};

// ---   *   ---   *   ---
// calculate 2x2 from leftmost coordinate

void JOJ::pixel2x2(
  JOJ::Pixel* (&pix)[4],
  uint64_t base

) {

  // get source
  JOJ::Pixel* buff=m_pixels.get();

  // fetch a slice
  pix[0]=&buff[base];
  pix[1]=&buff[base+1];
  pix[2]=&buff[base+m_hed.img_sz];
  pix[3]=&buff[base+m_hed.img_sz+1];

};

// ---   *   ---   *   ---
// create ID to represent four pixels

uint64_t JOJ::pixel_block(
  JOJ::Pixel* (&pix)[4]

) {

  uint64_t key  = 0x00;
  uint64_t j    = 0x00;

  int      bits = near_pow2(

  enc_bitsize<char>(
    (char*) m_c_enc.values,
    (int*) m_c_enc.cnt

  ));

// ---   *   ---   *   ---
// build block key from pixel data

  for(int i=0;i<4;i++) {

    uint64_t color=pix[i]->as_key(
      m_c_enc

    );

    key|=color<<=j;
    j+=bits;

  };

  return key;

};

// ---   *   ---   *   ---
// get vector of sizes for current sub-encoding

std::vector<uint64_t> JOJ::get_enc_sz(void) {

  // get mem
  std::vector<uint64_t> out;
  out.reserve(4);

  // break encoding
  char* sizes = (char*) m_c_enc.values;
  int*  cnt   = (int*) m_c_enc.cnt;

  // fill vector
  while(*cnt) {

    for(int i=0;i<*cnt;i++) {
      out.push_back(uint64_t(
        Frac::BITS[sizes[1]]+1

      ));

    };

    sizes+=3;
    cnt++;

  };

  return out;

};

// ---   *   ---   *   ---
// get palette for a given bit-depth

PAL& JOJ::get_pal(uint64_t sz) {

  switch(sz) {
  case 2:
    return m_pal[0];

  case 3:
    return m_pal[1];

  case 4:
    return m_pal[2];

  };

};

// ---   *   ---   *   ---
// builds palettes from pixel components

void JOJ::palette_pixels(void) {

  JOJ::Pixel* buff=m_pixels.get();
  std::vector<uint64_t> sizes=this->get_enc_sz();

// ---   *   ---   *   ---
// walk the image

  for(

    uint64_t i=0;

    i<m_hed.img_sz_sq;
    i++

  ) {

// ---   *   ---   *   ---
// get pixel

    JOJ::Pixel* pix=&buff[i];
    for(int j=0;j<4;j++) {

      // get component value && size
      uint8_t  key = (*pix)[j];
      uint64_t sz  = sizes[j];

      // ^pass through palette
      PAL& pal     = this->get_pal(sz);
      (*pix)[j]    = pal.cpush(key)->idex;

    };

  };

};

// ---   *   ---   *   ---
// swaps values of pixels for palette indices

void JOJ::xlate_pixels(void) {

  JOJ::Pixel* buff=m_pixels.get();
  std::vector<uint64_t> sizes=this->get_enc_sz();

// ---   *   ---   *   ---
// walk the image

  for(

    uint64_t i=0;

    i<m_hed.img_sz_sq;
    i++

  ) {

// ---   *   ---   *   ---
// get pixel

    JOJ::Pixel* pix=&buff[i];
    for(int j=0;j<4;j++) {

      // get component value && size
      uint8_t  marker = (*pix)[j];
      uint64_t sz     = sizes[j];

      // ^swap value for idex
      PAL& pal        = this->get_pal(sz);
      (*pix)[j]       = pal->iget(marker).idex;

    };

  };

};

// ---   *   ---   *   ---
// tight up the buff

void JOJ::palette_blocks(void) {

  uint64_t  limit  = m_hed.img_sz_sq>>1;
  uint64_t* blocks = m_blocks.get();

  PAL&      pal    = m_pal[3];

// ---   *   ---   *   ---
// walk by 2x2 blocks

  for(

    uint64_t base=0,i=0;

    base<m_hed.img_sz_sq-m_hed.img_sz;
    base+=2,i++

  ) {

    // skip uneven row
    base+=m_hed.img_sz*(
      base && !(base%m_hed.img_sz)

    );

// ---   *   ---   *   ---
// construct pixel block

    // get hashable 2x2 block
    JOJ::Pixel* pix[4];
    this->pixel2x2(pix,base);

    // generate key
    uint64_t key=this->pixel_block(pix);

    // insert new blocks in table
    blocks[i]=pal.cpush(key)->idex;

  };

};

// ---   *   ---   *   ---
// transforms blocks according to palette

void JOJ::xlate_blocks(void) {

  uint64_t* blocks = m_blocks.get();
  PAL&      pal    = m_pal[3];

  // walk the image and replace key index
  // for sorted value of block
  uint64_t limit=m_hed.img_sz_sq>>2;
  for(uint64_t i=0;i<limit;i++) {

    uint64_t marker=blocks[i];
    blocks[i]=pal.iget(marker).idex;

  };

};

// ---   *   ---   *   ---
// tights up the buffs

void JOJ::compress(void) {

  // build palettes from pixel
  // components of all images
  for(int i=0;i<m_hed.img_cnt;i++) {
    this->swap_to(i);
    this->palette_pixels();

  };

  // sort values by frequency
  m_pal[0].sort();
  m_pal[1].sort();
  m_pal[2].sort();

  // ^replace component value for
  // sorted palette index
  for(int i=0;i<m_hed.img_cnt;i++) {
    this->swap_to(i);
    this->xlate_pixels();

  };

// ---   *   ---   *   ---
// ^same process for 2x2 blocks

  for(int i=0;i<m_hed.img_cnt;i++) {
    this->swap_to(i);
    this->palette_blocks();

  };

  m_pal[3].sort();

  for(int i=0;i<m_hed.img_cnt;i++) {
    this->swap_to(i);
    this->xlate_blocks();

  };

};

// ---   *   ---   *   ---
// dump to disk

void JOJ::write_header(void) {

  Bin::write_header(&m_hed);

  for(int i=0;i<4;i++) {
    m_pal[i].write();

  };

};

// ---   *   ---   *   ---
// ^file body

void JOJ::write(void) {

  this->write_header();

  uint64_t* buff  = m_blocks.get();
  uint8_t*  out_p = out.get();

  Bin::write(
    buff,

    (m_hed.img_sz_sq >> 2)
  * sizeof(buff[0])

  );

};

// ---   *   ---   *   ---
// ^read body

void JOJ::read(void) {

  uint64_t blkcnt = m_meta.img_sz_sq>>2;
  auto     buff   = Bin::read(hed.idex_sz*blkcnt);

  // get mem
  m_pixels=std::unique_ptr<JOJ::Pixel>(
    new JOJ::Pixel[m_meta.img_sz_sq]

  );

// ---   *   ---   *   ---
// translate palette to color

  JOJ::Pixel* pix_p = m_pixels.get();
  uint64_t*   data  = (uint64_t*) buff.get();

  uint64_t    bits  = (hed.key_sz<<3)>>2;

// ---   *   ---   *   ---
// get key && idex size values

  uint64_t mask=(1LL<<bits)-1;
  mask-=1*!mask;

  uint64_t idex_sz=hed.idex_sz<<3;
  uint64_t idex_mask=(1LL<<idex_sz)-1;
  idex_mask-=1*!idex_mask;

  uint64_t bcnt=0;

// ---   *   ---   *   ---
// walk the buff

  for(

    uint64_t i=0,base=0;

    i<blkcnt;
    i++,base+=2

  ) {

    // idex of current block
    uint64_t idex=*data;
    idex>>=bcnt;
    idex&=idex_mask;

    // ^fetch from palette
    uint64_t key=m_meta.pal[idex];

    // idex is variable-sized
    // adjust buffer accto it
    bcnt+=idex_sz;
    if(bcnt==64) {bcnt=0;data++;};

    // skip uneven rows
    base+=m_meta.img_sz*(
      base && !(base%m_meta.img_sz)

    );

// ---   *   ---   *   ---
// decode pixels from block

    JOJ::Pixel* pix[4];
    this->pixel2x2(pix,base);

    for(int j=0;j<4;j++) {

      pix[j]->from_key(
        key&mask,
        m_meta.enc.color

      );

      key>>=bits;

    };

// ---   *   ---   *   ---

  };

};

// ---   *   ---   *   ---
// to avoid having the switch pasted everywhere

JOJ::SubEncoding JOJ::read_mode(
  int type,
  bool mode

) {

  JOJ::SubEncoding out;

  switch(type) {

  case JOJ::NVEC:
    out=m_enc.normal;
    m_hed.enc=0;

    Frac::Rounding_Mode=Frac::NVEC;

    break;

  case JOJ::YAUV:
    out=m_enc.color;
    m_hed.enc=1;

    Frac::Rounding_Mode=Frac::CORD;

    break;

  case JOJ::ORME:
    out=m_enc.shade;
    m_hed.enc=2;

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
  int imtype,
  bool mode

) {

  JOJ::Pixel* buff=m_pixels.get();

  JOJ::SubEncoding enc=
    this->read_mode(imtype,mode);

  m_c_enc=enc;

// ---   *   ---   *   ---
// transform to and from compressed floats

  struct Frac::Bat<char> batch={

    .m_bytes  = buff[0].as_ptr(),
    .m_floats = m_raw,
    .m_sz     = m_hed.img_sz_sq*4,

    .m_enc    = (char*) enc.values,
    .m_cnt    = (int*) enc.cnt,

    .m_mode   = mode,

  };

  batch.encoder();

};

// ---   *   ---   *   ---
