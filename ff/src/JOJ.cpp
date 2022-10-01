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

  for(size_t i=0;i<sz;i++) {

    this->encode_nvec(
      pixels+(i*4),
      out_p[i],

      Frac::ENCODE

    );

    this->encode_nvec(
      pixels+(i*4),
      out_p[i],

      Frac::DECODE

    );

  };

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

