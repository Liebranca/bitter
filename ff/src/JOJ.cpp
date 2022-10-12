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

:Bin(

  fpath,
  Bin::WRITE|Bin::TRUNC

) {

  m_raw            = pixels;
  m_meta.enc       = enc;

  m_meta.img_sz    = fast_sqrt2(sz);

  // remember size squared
  m_meta.img_sz_sq = sz;

  // counters
  m_meta.pal_cnt   = 0;
  m_meta.imcnt     = 0;

  // main data buffer
  m_pixels=std::unique_ptr<JOJ::Pixel>(
    new JOJ::Pixel[sz]

  );

  // ^compressed version
  m_blocks=std::unique_ptr<uint64_t>(
    new uint64_t[sz>>2]

  );

};

// ---   *   ---   *   ---
// ^from file to *.joj

JOJ::JOJ(

  std::string fpath,
  float* pixels,

  JOJ::Encoding enc

)

:Bin(

  fpath,
  Bin::READ

) {

  m_raw      = pixels;
  m_meta.enc = enc;

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
  pix[2]=&buff[base+m_meta.img_sz];
  pix[3]=&buff[base+m_meta.img_sz+1];

};

// ---   *   ---   *   ---
// create ID to represent four pixels

uint64_t JOJ::pixel_block(
  TAB::Symbol& blk,
  JOJ::Pixel* (&pix)[4]

) {

  uint64_t key  = 0x00;
  uint64_t j    = 0x00;

  int      bits = near_pow2(

  enc_bitsize<char>(
    (char*) m_meta.c_enc.values,
    (int*) m_meta.c_enc.cnt

  ));

// ---   *   ---   *   ---
// build block key from pixel data

  for(int i=0;i<4;i++) {

    uint64_t color=pix[i]->as_key(
      m_meta.c_enc

    );

    key|=color<<=j;
    j+=bits;

  };

  return key;

};

// ---   *   ---   *   ---
// tight up the buff

void JOJ::compress(void) {

  uint64_t limit   = m_meta.img_sz_sq>>1;
  uint64_t* blocks = m_blocks.get();

  Tab<uint64_t,TAB::Symbol> tab(
    m_meta.img_sz<<5

  );

  std::vector<uint64_t> keys;
  keys.reserve(limit);

  for(

    uint64_t base=0,i=0;

    base<m_meta.img_sz_sq-m_meta.img_sz;
    base+=2,i++

  ) {

// ---   *   ---   *   ---
// construct pixel block

    // skip uneven row
    base+=m_meta.img_sz*(
      base && !(base%m_meta.img_sz)

    );

    // get hashable 2x2 block
    TAB::Symbol blk;
    JOJ::Pixel* pix[4];

    this->pixel2x2(pix,base);

    // generate key
    uint64_t key=this->pixel_block(blk,pix);

// ---   *   ---   *   ---
// insert new block in table

    TAB::Lookup lkp=tab.has(key);

    if(!lkp.key_match) {

      blk.value=key;
      blk.idex=m_meta.pal_cnt;

      blk.freq=1;

      tab.push(lkp,key,blk);
      m_meta.pal_cnt++;

      // save key for later
      keys.push_back(key);

// ---   *   ---   *   ---
// up frequency of repeated block

    } else {

      blk=tab.get(lkp);
      blk.freq++;

      tab.set(lkp,blk);

    };

// ---   *   ---   *   ---
// set block to match value for ease
// of adjusting it later

    blocks[i]=blk.idex;

  };

// ---   *   ---   *   ---
// process the hashed image

  this->xlate_blocks(keys,tab);

};

// ---   *   ---   *   ---
// transforms blocks according to palette

void JOJ::xlate_blocks(
  std::vector<uint64_t>& keys,
  Tab<uint64_t,TAB::Symbol>& tab

) {

  uint64_t* blocks = m_blocks.get();

  // give shorter values to block
  // with higher frequency
  tab->sort(keys,m_meta.pal);

  // walk the image and replace key index
  // for sorted value of block
  uint64_t limit=m_meta.img_sz_sq>>2;
  for(uint64_t i=0;i<limit;i++) {

    uint64_t marker=blocks[i];
    blocks[i]=tab.get(keys[marker]).idex;

  };

};

// ---   *   ---   *   ---
// dump to disk

void JOJ::write_header(
  uint64_t body_sz

) {

  // pixel size * block size
  uint64_t bits=near_pow2(enc_bitsize<char>(
    (char*) m_meta.c_enc.values,
    (int*) m_meta.c_enc.cnt

  ))*4;

  uint64_t bytes=bits>>3;

  // size of palette in bytes
  uint64_t pal_sz=(m_meta.pal_cnt*bytes);

  // size of 2x2 block
  uint64_t idex_sz=near_pow2(
    bitsize(m_meta.pal_cnt)

  )>>3;

  idex_sz=(!idex_sz) ? 1 : idex_sz;

// ---   *   ---   *   ---
// use helper struct for convenience

  JOJ::Header hed={

    .pal_sz  = pal_sz,
    .pal_cnt = m_meta.pal_cnt,
    .img_sz  = m_meta.img_sz,

    .key_sz  = bytes,
    .idex_sz = idex_sz,

    .body_sz = body_sz

  };

  // get mem
  std::unique_ptr<char> out(
    new char[pal_sz+sizeof(hed)]

  );

  // write header
  uint8_t* out_p=(uint8_t*) out.get();
  *((JOJ::Header*) out_p)=hed;
  out_p+=sizeof(hed);

// ---   *   ---   *   ---
// write palette to file

  xfer<typeof(m_meta.pal)>(
    out_p,m_meta.pal,
    pal_sz,bytes


  );

  Bin::write(out.get(),pal_sz+sizeof(hed));

};

// ---   *   ---   *   ---
// ^file body

void JOJ::write(void) {

  int      bits  = near_pow2(
    bitsize(m_meta.pal_cnt)

  );

  uint64_t size  = m_meta.img_sz_sq>>2;
  uint64_t bytes = std::max(bits>>3,1);

  size*=bytes;

  std::unique_ptr<uint8_t> out(
    new uint8_t[size]

  );

  this->write_header(size);

// ---   *   ---   *   ---
// pass block data to char*

  uint64_t* buff  = m_blocks.get();
  uint8_t*  out_p = out.get();

  xfer<uint64_t*>(
    out_p,buff,
    size,bytes

  );

// ---   *   ---   *   ---
// pass through zlib

  Zwrap z(Zwrap::DEFLATE|Zwrap::OUTPUT_BIN);

  z.set_src(out_p,size);
  z.set_dst(this,0,get_header_sz());

  z.flate();

};

// ---   *   ---   *   ---
// get file body descriptor

JOJ::Header JOJ::read_header(void) {

  JOJ::Header hed;

  auto buff=Bin::read(sizeof(hed));
  hed=*((JOJ::Header*) buff.get());

  m_meta.pal_cnt=hed.pal_cnt;
  m_meta.img_sz=hed.img_sz;

  m_meta.img_sz_sq=
    m_meta.img_sz*m_meta.img_sz;

  buff.reset();

// ---   *   ---   *   ---

  uint64_t keys_sz=m_meta.pal_cnt*hed.key_sz;
  buff=std::unique_ptr<uint8_t>(
    new uint8_t[keys_sz]

  );

  uint8_t* keys=(uint8_t*) buff.get();

  Zwrap z(Zwrap::INFLATE|Zwrap::INPUT_BIN);

  z.set_src(this,hed.body_sz,get_header_sz());
  z.set_dst(keys,keys_sz);

  z.flate();

// ---   *   ---   *   ---
// fill out palette

  m_meta.pal.reserve(m_meta.pal_cnt);

  for(uint64_t i=0;i<m_meta.pal_cnt;i++) {
    uint64_t key=0;

    for(uint64_t j=0;j<hed.key_sz;j++) {
      key|=uint64_t(*keys++)<<(j*8);

    };

    m_meta.pal.push_back(key);

  };

  buff.reset();

  return hed;

};

// ---   *   ---   *   ---
// ^read body

void JOJ::read(void) {

  JOJ::Header hed = this->read_header();

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
    out=m_meta.enc.normal;
    Frac::Rounding_Mode=Frac::NVEC;

    break;

  case JOJ::YAUV:
    out=m_meta.enc.color;
    Frac::Rounding_Mode=Frac::CORD;

    break;

  case JOJ::ORME:
    out=m_meta.enc.shade;
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

  m_meta.c_enc=enc;

// ---   *   ---   *   ---
// transform to and from compressed floats

  struct Frac::Bat<char> batch={

    .m_bytes  = buff[0].as_ptr(),
    .m_floats = m_raw,
    .m_sz     = m_meta.img_sz_sq*4,

    .m_enc    = (char*) enc.values,
    .m_cnt    = (int*) enc.cnt,

    .m_mode   = mode,

  };

  batch.encoder();

};

// ---   *   ---   *   ---
