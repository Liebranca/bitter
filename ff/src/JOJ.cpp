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
      out_p[i]

    );

    this->decode_nvec(
      pixels+(i*4),
      out_p[i]

    );

  };

  out.reset();

};

// ---   *   ---   *   ---
// stores unit vectors

void JOJ::encode_nvec(

  float* n,
  JOJ_PIXEL& j

) {

  float step = FRACL_F[m_enc.nvec_xy];
  int   bits = m_enc.nvec_xy+1;

  // encode x && y
  for(int i=0;i<2;i++) {
    j[i]=frac<char>(n[i],step,bits);

  };

  // switch precision level
  step = FRACL_F[m_enc.nvec_w];
  bits = m_enc.nvec_w+1;

  // encode w
  j.w=frac<char>(n[3],step,bits);

  // store handeness
  j.sign=n[2]<0;

};

// ---   *   ---   *   ---
// ^retrieve

void JOJ::decode_nvec(

  float* n,
  JOJ_PIXEL& j

) {

  float step = FRACL_F[m_enc.nvec_xy];
  int   bits = m_enc.nvec_xy+1;

  // decode x && y
  for(int i=0;i<2;i++) {
    n[i]=unfrac<char>(j[i],step,bits);
    n[i]=(n[i]*2.0)-1;

  };

  // switch precision level
  step = FRACL_F[m_enc.nvec_w];
  bits = m_enc.nvec_w+1;

  // decode w
  n[3]=unfrac<char>(j.w,step,bits);

  // calc z
  int sign=(j.sign) ? -1 : 1;

  float z=0;
  float d=1-(

    pow(n[0],2)
  + pow(n[1],2)

  );

  if(d>0) {z=sqrt(d)*sign;};

  // transform
  n[0]=(n[0]+1)*0.5;
  n[1]=(n[1]+1)*0.5;

  n[2]=(z+1)*0.5;

  // sign check
  if(

     (!j.sign && n[2]<0)
  || (j.sign && n[2]>0)

  ) {
    n[2]*=-1;

  };

};

// ---   *   ---   *   ---
