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
  #include "ff/JOJ.hpp"

// ---   *   ---   *   ---
// *.joj to float buffer

JOJ::JOJ(std::string fpath)
:Bin(fpath,Bin::READ) {

};

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

  m_raw          = pixels;
  m_meta.enc     = enc;

  m_meta.imsz    = fast_sqrt2(sz);

  // remember size squared
  m_meta.imsz_sq = sz;

  // counters
  m_meta.palcnt  = 0;
  m_meta.imcnt   = 0;
  m_meta.mip     = 0;

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

JOJ::JOJ(

  std::string fpath,
  float* pixels,

  JOJ::Encoding enc

)

:Bin(

  fpath,
  Bin::READ

) {

  m_raw          = pixels;
  m_meta.enc     = enc;

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
  pix[2]=&buff[base+m_meta.imsz];
  pix[3]=&buff[base+m_meta.imsz+1];

};

// ---   *   ---   *   ---
// create ID to represent four pixels

uint64_t JOJ::pixel_block(
  JOJ::Pixel_Block& blk,
  JOJ::Pixel* (&pix)[4]

) {

  uint64_t key  = 0x00;
  uint64_t j    = 0x00;

  int      bits = near_pow2(

  enc_bitsize<char>(
    (char*) m_meta.c_enc.values,
    (int*) m_meta.c_enc.cnt

  ));

  for(int i=0;i<4;i++) {

    blk.color_id[i]=pix[i]->as_key(
      m_meta.c_enc

    );

    key|=blk.color_id[i]<<=j;
    j+=bits;

  };

  return key;

};

// ---   *   ---   *   ---
// tight up the buff

void JOJ::compress(void) {

  uint64_t limit   = m_meta.imsz_sq>>1;
  uint64_t* blocks = m_blocks.get();

  Tab<uint64_t,JOJ::Pixel_Block> tab(
    m_meta.imsz<<5

  );

  std::vector<uint64_t> keys;
  keys.reserve(limit);

  for(

    uint64_t base=0,i=0;

    base<m_meta.imsz_sq-m_meta.imsz;
    base+=2,i++

  ) {

// ---   *   ---   *   ---
// construct pixel block

    // skip uneven row
    base+=m_meta.imsz*(
      base && !(base%m_meta.imsz)

    );

    // get hashable 2x2 block
    JOJ::Pixel_Block blk;
    JOJ::Pixel* pix[4];

    this->pixel2x2(pix,base);

    // generate key
    uint64_t key=this->pixel_block(blk,pix);

// ---   *   ---   *   ---
// insert new block in table

    Tab_Lookup lkp=tab.has(key);

    if(!lkp.key_match) {

      blk.value=key;
      blk.idex=m_meta.palcnt;

      blk.freq=1;

      tab.push(lkp,key,blk);
      m_meta.palcnt++;

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

  m_meta.pal.resize(m_meta.palcnt);
  this->xlate_blocks(keys,tab);

};

// ---   *   ---   *   ---
// transforms blocks according to palette

void JOJ::xlate_blocks(
  std::vector<uint64_t>& keys,
  Tab<uint64_t,JOJ::Pixel_Block>& tab

) {

  uint64_t* blocks = m_blocks.get();

  // give shorter values to block
  // with higher frequency
  this->sort_blocks(keys,tab);

  // walk the image and replace key index
  // for sorted value of block
  uint64_t limit  = m_meta.imsz_sq>>2;
  for(uint64_t i=0;i<limit;i++) {

    uint64_t marker=blocks[i];
    blocks[i]=tab.get(keys[marker]).idex;

  };

};

// ---   *   ---   *   ---
// dump to disk

void JOJ::write_header(void) {

  // pixel size * block size
  uint64_t bits=near_pow2(enc_bitsize<char>(
    (char*) m_meta.c_enc.values,
    (int*) m_meta.c_enc.cnt

  ))*4;

  uint64_t bytes=bits>>3;

  // size of palette in bytes
  uint64_t palsz=(m_meta.palcnt*bytes);

  // size of 2x2 block
  uint64_t idexsz=near_pow2(
    bitsize(m_meta.palcnt)

  )>>3;

  idexsz=(!idexsz) ? 1 : idexsz;

// ---   *   ---   *   ---
// use helper struct for convenience

  JOJ::Header hed={

    .palcnt = m_meta.palcnt,
    .imsz   = m_meta.imsz,

    .keysz  = bytes,
    .idexsz = idexsz

  };

  // get mem
  std::unique_ptr<char> out(
    new char[palsz+sizeof(hed)]

  );

  // write header
  uint8_t* out_p=(uint8_t*) out.get();
  *((JOJ::Header*) out_p)=hed;
  out_p+=sizeof(hed);

// ---   *   ---   *   ---
// write palette to file

  for(

    uint64_t i=0,k=0;

    i<palsz;
    i+=bytes,k++

  ) {

    uint64_t val=m_meta.pal[k];

    // break value into chars
    for(int j=0;j<bytes;j++) {
      out_p[i+j]=val&0xFF;
      val>>=8;

    };

  };

  Bin::write(out.get(),palsz+sizeof(hed));

};

// ---   *   ---   *   ---

void JOJ::write(void) {

  this->write_header();

  int      bits  = near_pow2(
    bitsize(m_meta.palcnt)

  );

  uint64_t size  = m_meta.imsz_sq>>2;
  uint64_t bytes = std::max(bits>>3,1);

  size*=bytes;

  std::unique_ptr<char> out(
    new char[size]

  );

// ---   *   ---   *   ---

  uint64_t* buff  = m_blocks.get();
  uint8_t*  out_p = (uint8_t*) out.get();

  for(

    uint64_t i=0,h=0;

    i<size;
    i+=bytes,h++

  ) {

    for(int j=0;j<bytes;j++) {
      out_p[i+j]=buff[h]&0xFF;
      buff[h]>>=8;

    };

  };

  Bin::write(out_p,size);

};

// ---   *   ---   *   ---

JOJ::Header JOJ::read_header(void) {

  JOJ::Header hed;

  auto buff=Bin::read(sizeof(hed));
  hed=*((JOJ::Header*) buff.get());

  m_meta.palcnt=hed.palcnt;
  m_meta.imsz=hed.imsz;

  m_meta.imsz_sq=
    m_meta.imsz*m_meta.imsz;

  buff.reset();

// ---   *   ---   *   ---

  m_meta.pal.reserve(m_meta.palcnt);

  buff=Bin::read(
    m_meta.palcnt*hed.keysz

  );

  uint8_t* keys=(uint8_t*) buff.get();

  for(uint64_t i=0;i<m_meta.palcnt;i++) {

    uint64_t key=0;

    for(uint64_t j=0;j<hed.keysz;j++) {
      key|=uint64_t(*keys++)<<(j*8);

    };

    m_meta.pal.push_back(key);

  };

  buff.reset();

  return hed;

};

// ---   *   ---   *   ---

void JOJ::read(void) {

  JOJ::Header hed=this->read_header();

  uint64_t blkcnt=m_meta.imsz_sq>>2;

  auto buff=Bin::read(hed.idexsz*blkcnt);

  // get mem
  m_pixels=std::unique_ptr<JOJ::Pixel>(
    new JOJ::Pixel[m_meta.imsz_sq]

  );

// ---   *   ---   *   ---
// translate palette to color

  JOJ::Pixel* pix_p = m_pixels.get();
  uint64_t*   data  = (uint64_t*) buff.get();

  uint64_t bits=(hed.keysz<<3)>>2;

// ---   *   ---   *   ---

  uint64_t mask=(1LL<<bits)-1;
  mask-=1*!mask;

  uint64_t idex_sz=hed.idexsz<<3;
  uint64_t idex_mask=(1LL<<idex_sz)-1;
  idex_mask-=1*!idex_mask;

  uint64_t bcnt=0;

// ---   *   ---   *   ---

  for(

    uint64_t i=0,base=0;

    i<blkcnt;
    i++,base+=2

  ) {

    uint64_t idex=*data;
    idex>>=bcnt;
    idex&=idex_mask;

    uint64_t key=m_meta.pal[idex];

    bcnt+=idex_sz;

    if(bcnt==64) {bcnt=0;data++;};

    base+=m_meta.imsz*(
      base && !(base%m_meta.imsz)

    );

// ---   *   ---   *   ---

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
// sort blocks by frequency

void JOJ::sort_blocks(
  std::vector<uint64_t>& keys,
  Tab<uint64_t,JOJ::Pixel_Block>& tab

) {

  uint64_t limit  = keys.size();
  uint64_t bottom = limit-1;

  for(uint64_t i=0;i<limit;i++) {

    uint64_t            top     = 0;
    JOJ::Pixel_Block* top_blk = NULL;

// ---   *   ---   *   ---
// walk remaining

    for(uint64_t key : keys) {

      // get entry
      JOJ::Pixel_Block& blk=
        tab.get(key);

      // skip already sorted
      if(!blk.freq) {
        continue;

      // discard low-frequency blocks
      // from even being sorted
      } else if(blk.freq<128) {
        blk.freq=0;
        blk.idex=bottom--;

        m_meta.pal[blk.idex]=
          blk.value;

        continue;

      };

      // compare
      if(blk.freq>top) {
        top=blk.freq;
        top_blk=&blk;

      };

    };

// ---   *   ---   *   ---
// assign idex to result and
// mark block as sorted

    if(top_blk==NULL) {break;};

    top_blk->idex=i;
    top_blk->freq=0;

    m_meta.pal[i]=top_blk->value;

    if(bottom<=i) {break;};

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
    .m_sz     = m_meta.imsz_sq*4,

    .m_enc    = (char*) enc.values,
    .m_cnt    = (int*) enc.cnt,

    .m_mode   = mode,

  };

  batch.encoder();

};

// ---   *   ---   *   ---
