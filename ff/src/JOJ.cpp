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

  m_pixels         = pixels;
  m_header.enc     = enc;

  m_header.imsz    = fast_sqrt2(sz);
  m_header.pal     = Tab<size_t,size_t>(
    m_header.imsz

  );

  m_header.imsz_sq = sz;

  m_header.palcnt  = 0;
  m_header.imcnt   = 0;
  m_header.mip     = 0;

  // allocate secondary buffer
  m_buff=std::unique_ptr<JOJ::Pixel>(
    new JOJ::Pixel[sz]

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

typedef struct {

  size_t idex;
  size_t freq;

} Symbol;

// ---   *   ---   *   ---

void JOJ::build_palette(void) {

  JOJ::Pixel* buff=m_buff.get();

  Tab<std::string,Symbol> out(m_header.imsz<<5);
  size_t ls_cnt=0;
  size_t total_blocks=0;

  std::vector<std::string> keys;
size_t ghost_addr=0;

  for(

    size_t base=0;

    base<m_header.imsz_sq;
    base+=2

  ) {

    // skip uneven row
    if(base && !(base%m_header.imsz)) {
      base+=m_header.imsz;

    };

    // down-left
    JOJ::Pixel* dl = &buff[base];

    // down-right
    JOJ::Pixel* dr = &buff[base+1];

    // up-left
    JOJ::Pixel* ul = &buff[base+m_header.imsz];

    // up-right
    JOJ::Pixel* ur = &buff[base+m_header.imsz+1];

// ---   *   ---   *   ---
// block-hashing

    size_t i0=this->push_palette(dl);
    size_t i1=this->push_palette(dr);
    size_t i2=this->push_palette(ul);
    size_t i3=this->push_palette(ur);

    std::string blk_s(sizeof(size_t)*4,'\0');
    *((size_t*) &blk_s[sizeof(size_t)*0])=i0;
    *((size_t*) &blk_s[sizeof(size_t)*1])=i1;
    *((size_t*) &blk_s[sizeof(size_t)*2])=i2;
    *((size_t*) &blk_s[sizeof(size_t)*3])=i3;

    Tab_Lookup lkp=out.has(blk_s);

    if(!lkp.key_match) {

      Symbol s={

        .idex=ls_cnt,
        .freq=1

      };

size_t* A=(size_t*) &blk_s[0];
if(A[0]==1 && A[1]==1 & A[2]==1 & A[3]==3) {
  ghost_addr=lkp.real;

};

      out.push(
        lkp,blk_s,s

      );

      ls_cnt++;

      keys.push_back(blk_s);

    } else {

      Symbol s=out.get(blk_s);

      s.freq++;
      out.set(blk_s,s);

    };

    total_blocks++;

// ---   *   ---   *   ---

  };

  printf("%u\n",total_blocks);

  for(std::string key : keys) {

    Tab_Lookup lkp=out.has(key);
    Symbol s=out.get(lkp);

    if(!s.idex && !s.freq) {

      fprintf(
        stderr,

        "GHOST %016X ? %016X\n",
        lkp.real,ghost_addr

      );

      out.report();

      exit(1);

    };

    printf("%u : %u\n",s.idex,s.freq);

  };

// ---   *   ---   *   ---

//  printf(
//    "\n\nPalette size: %u/%u (%.2f)\n",
//
//    m_header.palcnt,
//    m_header.imsz_sq,
//
//    1.0f - (
//
//      (float) m_header.palcnt
//    / (float) m_header.imsz_sq
//
//    )
//
//  );
//
//  printf("Total blocks: %u\n",ls_cnt);

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

  JOJ::Pixel* buff=m_buff.get();

  JOJ::SubEncoding enc=
    this->read_mode(imtype,mode);

// ---   *   ---   *   ---
// transform to and from compressed floats

  struct Frac::Bat<char> batch={

    .m_bytes  = buff[0].as_ptr(),
    .m_floats = m_pixels,
    .m_sz     = m_header.imsz_sq*4,

    .m_enc    = (char*) enc.values,
    .m_cnt    = (int*) enc.cnt,

    .m_mode   = mode,

  };

  batch.encoder();

// ---   *   ---   *   ---
// post-encode stuff

  if(mode==Frac::ENCODE) {
    this->build_palette();

  };

};

// ---   *   ---   *   ---
