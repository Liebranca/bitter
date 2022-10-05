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
  size_t sz,

  JOJ::Encoding enc

)

:Bin(

  fpath,
  Bin::WRITE|Bin::TRUNC

) {

  m_raw            = pixels;
  m_header.enc     = enc;

  m_header.imsz    = fast_sqrt2(sz);

  // reserve space for palettes
  m_header.pal     = Tab<size_t,size_t>(
    m_header.imsz

  );

  m_header.blkpal  = Tab<

    std::string,
    JOJ::Pixel_Block

  >(m_header.imsz<<5);

  // remember size squared
  m_header.imsz_sq = sz;

  // counters
  m_header.palcnt  = 0;
  m_header.imcnt   = 0;
  m_header.mip     = 0;

  // main data buffer
  m_pixels=std::unique_ptr<JOJ::Pixel>(
    new JOJ::Pixel[sz]

  );

  // ^compressed version
  m_blocks=std::unique_ptr<size_t>(
    new size_t[sz>>1]

  );

};

// ---   *   ---   *   ---

size_t JOJ::push_palette(JOJ::Pixel* b) {

  size_t out;
  size_t key=b->as_key();

  Tab_Lookup lkp=
    m_header.pal.has(key);

  if(!lkp.key_match) {

    m_header.pal.push(
      lkp,key,m_header.palcnt

    );

    out=m_header.palcnt++;

  } else {
    out=m_header.pal.get(lkp);

  };

  return out;

};

// ---   *   ---   *   ---
// calculate 2x2 from leftmost coordinate

void JOJ::pixel2x2(
  JOJ::Pixel* (&pix)[4],
  size_t base

) {

  // get source
  JOJ::Pixel* buff=m_pixels.get();

  // fetch a slice
  pix[0]=&buff[base];
  pix[1]=&buff[base+1];
  pix[2]=&buff[base+m_header.imsz];
  pix[3]=&buff[base+m_header.imsz+1];

};

// ---   *   ---   *   ---

std::string JOJ::pixel_block(
  JOJ::Pixel_Block& blk,
  JOJ::Pixel* (&pix)[4]

) {

  std::string key(sizeof(size_t)*4,'\0');

  for(int i=0;i<4;i++) {
    blk.color_id[i]=
      this->push_palette(pix[i]);

    *((size_t*) &key[sizeof(size_t)*i])=
      blk.color_id[i];

  };

  return key;

};

// ---   *   ---   *   ---
// tight up the buff

void JOJ::compress(void) {

  size_t ls_cnt  = 0;
  size_t limit   = m_header.imsz_sq/2;

  size_t* blocks = m_blocks.get();

  std::vector<std::string> keys;
  keys.reserve(limit);

  for(

    size_t base=0,i=0;

    base<m_header.imsz_sq;
    base+=2,i++

  ) {

// ---   *   ---   *   ---
// construct pixel block

    // skip uneven row
    if(base && !(base%m_header.imsz)) {
      base+=m_header.imsz;

    };

    // get hashable 2x2 block
    JOJ::Pixel_Block blk;
    JOJ::Pixel* pix[4];

    this->pixel2x2(pix,base);

    // generate key
    std::string key=
      this->pixel_block(blk,pix);

// ---   *   ---   *   ---
// insert new block in table

    Tab_Lookup lkp=m_header.blkpal.has(key);

    if(!lkp.key_match) {

      blk.value=ls_cnt;
      blk.freq=1;

      m_header.blkpal.push(lkp,key,blk);
      ls_cnt++;

      // save key for later
      keys.push_back(key);

// ---   *   ---   *   ---
// up frequency of repeated block

    } else {

      blk=m_header.blkpal.get(lkp);
      blk.freq++;

      m_header.blkpal.set(lkp,blk);

    };

// ---   *   ---   *   ---
// set block to match value for ease
// of adjusting it later

    blocks[i]=blk.value;

  };

// ---   *   ---   *   ---
// process the hashed image

  this->xlate_blocks(keys);


};

// ---   *   ---   *   ---
// transforms blocks according to palette

void JOJ::xlate_blocks(
  std::vector<std::string>& keys

) {

  size_t* blocks = m_blocks.get();

  // give shorter values to block
  // with higher frequency
  this->sort_blocks(keys);

  // walk the image and replace key index
  // for sorted value of block
  size_t limit  = m_header.imsz_sq/2;
  for(size_t i=0;i<limit;i++) {

    size_t marker=blocks[i];

    blocks[i]=m_header.blkpal.get(
      keys[marker]

    ).idex;

  };

};

// ---   *   ---   *   ---
// dump to disk

void JOJ::write(void) {

  size_t limit = m_header.imsz_sq/2;
  int    bits  = fast_sqrt2(near_pow2(limit));

  size_t size  = limit;

  bits/=8;
  size*=bits;

  std::unique_ptr<char> out(
    new char[size]

  );

// ---   *   ---   *   ---

  size_t* buff  = m_blocks.get();
  char*   out_p = out.get();

  for(

    size_t i=0,h=0;

    i<limit;
    i+=bits,h++

  ) {

    for(int j=0;j<bits;j++) {
      out_p[i+j]=buff[h]&0xFF;
      buff[h]=buff[h]>>8;

    };

  };

  Bin::write(out_p,size);

};

// ---   *   ---   *   ---
// sort blocks by frequency

void JOJ::sort_blocks(
  std::vector<std::string>& keys

) {

  size_t limit  = keys.size();
  size_t bottom = limit;

  for(size_t i=0;i<limit;i++) {

    size_t            top     = 0;
    JOJ::Pixel_Block* top_blk = NULL;

// ---   *   ---   *   ---
// walk remaining

    for(std::string key : keys) {

      // get entry
      JOJ::Pixel_Block& blk=
        m_header.blkpal.get(key);

      // skip already sorted
      if(!blk.freq) {
        continue;

      // discard low-frequency blocks
      // from even being sorted
      } else if(blk.freq<128) {
        blk.freq=0;
        blk.idex=bottom--;

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

    top_blk->idex=i;
    top_blk->freq=0;

    bottom--;
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
    out=m_header.enc.normal;
    Frac::Rounding_Mode=Frac::NVEC;

    break;

  case JOJ::YAUV:
    out=m_header.enc.color;
    Frac::Rounding_Mode=Frac::CORD;

    break;

  case JOJ::ORME:
    out=m_header.enc.shade;
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

// ---   *   ---   *   ---
// transform to and from compressed floats

  struct Frac::Bat<char> batch={

    .m_bytes  = buff[0].as_ptr(),
    .m_floats = m_raw,
    .m_sz     = m_header.imsz_sq*4,

    .m_enc    = (char*) enc.values,
    .m_cnt    = (int*) enc.cnt,

    .m_mode   = mode,

  };

  batch.encoder();

};

// ---   *   ---   *   ---
