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

void JOJ::pal_push(size_t key) {

  Tab_Lookup lkp=
    m_header.pal.has(key);

  if(!lkp.key_match) {

    m_header.pal.push(
      lkp,key,m_header.palcnt

    );

    m_header.palcnt++;

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

// ---   *   ---   *   ---
// rgba is heeeeeeeeeaavy

  case JOJ::YAUV:
    out=m_header.enc.color;
    Frac::Rounding_Mode=Frac::CORD;

    if(mode == Frac::ENCODE) {
      rgba2yauv(
        m_pixels,
        m_header.imsz_sq*4

      );

    };

    break;

// ---   *   ---   *   ---

  case JOJ::ORME:
    out=m_header.enc.shade;
    Frac::Rounding_Mode=Frac::CORD;

    break;

// ---   *   ---   *   ---
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
// should write a post read_mode instead...

  if(mode == Frac::DECODE) {

    yauv2rgba(
      m_pixels,
      m_header.imsz_sq*4

    );

  };

};

// ---   *   ---   *   ---
