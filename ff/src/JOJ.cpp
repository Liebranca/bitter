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
  #include "kvrnel/Tab.hpp"

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

  JOJ_ENCODING enc

)

:Bin(

  fpath,
  Bin::WRITE|Bin::TRUNC

) {

  m_pixels=pixels;
  m_enc=enc;

  std::unique_ptr<JOJ_PIXEL> out(
    new JOJ_PIXEL[sz]

  );

  JOJ_PIXEL* out_p=out.get();
  Frac::Rounding_Mode=Frac::CORD;

  Tab<size_t,size_t> palette(sz);
  size_t pal_cnt=0;

  for(size_t i=0,j=0;i<sz;i++,j+=4) {

    this->encode_color(
      pixels+j,
      out_p[i],

      Frac::ENCODE

    );

    size_t k=out_p[i].as_key();
    Tab_Lookup lkp=palette.has(k);

    if(!lkp.key_match) {
      palette.push(lkp,k,pal_cnt);
      pal_cnt++;

    };

  };

  printf("Palette size: %u\n",pal_cnt);
  out.reset();

};

// ---   *   ---   *   ---
// stores unit vectors

void JOJ::encode_nvec(

  float* n,
  JOJ_PIXEL& j,

  bool mode

) {

  char* bytes[]={
    &j.x,&j.z,NULL,
    &j.y,NULL,
    &j.curv,NULL

  };

  float* floats[]={n+0,n+2,n+1,n+3};

  struct Frac::Bat<char> batch={
    .m_bytes  = bytes,
    .m_enc    = m_enc.normal,
    .m_floats = floats,
    .m_sz     = 3,

    .m_mode   = mode,

  };

  batch.encoder();

};

// ---   *   ---   *   ---
// try encoding four pixels at a time someday

void JOJ::encode_color(
  float* p,
  JOJ_PIXEL& j,

  bool mode

) {

  if(mode==Frac::ENCODE) {
    rgba2yauv(p);

  };

  char* bytes[]={
    &j.luma,&j.alpha,NULL,
    &j.chroma_u,&j.chroma_v,NULL

  };

  float* floats[]={p+0,p+1,p+2,p+3};

  struct Frac::Bat<char> batch={
    .m_bytes  = bytes,
    .m_enc    = m_enc.color,
    .m_floats = floats,
    .m_sz     = 2,

    .m_mode   = mode,

  };

  batch.encoder();

  if(mode==Frac::DECODE) {
    yauv2rgba(p);

  };

};

// ---   *   ---   *   ---
