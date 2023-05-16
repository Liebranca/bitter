// ---   *   ---   *   ---
// BYTES
// common binary ops
//
// LIBRE SOFTWARE
// Licenced under GNU GPL3
// be a bro and inherit
//
// CONTRIBUTORS
// lyeb,
//

// ---   *   ---   *   ---
// deps

  #include "kvrnel/Bytes.hpp"

// ---   *   ---   *   ---
// pow by squaring

uint64_t ipow(
  uint64_t x,
  uint64_t n

) {

  uint64_t out=1;

  while(n) {

    if(n&1) {
      out*=x;

    };

    n  >>= 1;
    x   *= x;

  };

  return out;
}

// ---   *   ---   *   ---

char* stirr_p(int* src,int cnt) {

  static char buff[64]={0};
  int i=0;

  while(i<cnt*sizeof(*src)) {

    char* ptr=(char*) src;

    for(int j=0;j<sizeof(*src);j++) {
      char c=*ptr;
      c=(c<0x20 || c>0x7E) ? 0x2E : c;

      buff[i++]=c;
      ptr++;

    };

    src++;

  };

  return buff;

};

// ---   *   ---   *   ---
// m_bytes to m_floats

template <typename T>
void Frac::Bat<T>::encode(

  float   step,
  int     bits,

  bool    unsig

) {

  *m_bytes=(*m_floats==Frac::BLANK)
    ? 0
    : frac<T>(*m_floats,step,bits,unsig)
    ;

};

// ---   *   ---   *   ---
// ^inverse

template <typename T>
void Frac::Bat<T>::decode(

  float   step,
  int     bits,

  bool    unsig

) {

  *m_floats=unfrac<T>(
    *m_bytes,step,bits,unsig

  );

};

// ---   *   ---   *   ---
// sequential frac or unfrac'ing

template <typename T>
void Frac::Bat<T>::encoder(void) {

auto branch_to=(m_mode==Frac::DECODE)
  ? &Frac::Bat<T>::decode
  : &Frac::Bat<T>::encode
  ;

T*   orig_enc = m_enc;
int* orig_cnt = m_cnt;

while(m_sz) {

  // switch precision level
  float step  = STEP[*m_enc++];
  int   bits  = BITS[*m_enc++];

  // true if field is unsigned
  bool  unsig = *m_enc++;

  // run through elements
  int limit=*m_cnt;
  for(int x=0;x<limit;x++) {

    (this->*branch_to)(step,bits,unsig);

    // next element
    m_bytes++;
    m_floats++;
    m_sz--;

  };

// ---   *   ---   *   ---
// next round

  m_cnt++;

  // reset
  if(!*m_cnt) {
    m_enc=orig_enc;
    m_cnt=orig_cnt;

  };

}};

// ---   *   ---   *   ---
// color transform

void rgba2yauv(float* p) {

  // extract for clarity
  float r=p[0];
  float g=p[1];
  float b=p[2];
  float a=p[3];

// ---   *   ---   *   ---
// discard pixels that approach
// transparent pitch black

  if(

     r < 0.08f
  && g < 0.08f
  && b < 0.08f
  && a < 0.08f

  ) {

    p[0]=Frac::BLANK;
    p[1]=Frac::BLANK;
    p[2]=Frac::BLANK;
    p[3]=Frac::BLANK;

    return;

  };

// ---   *   ---   *   ---
// transform color

  float yauv[]={

    // luma
    (0.257f * r)
  + (0.504f * g)
  + (0.098f * b),

    // alpha
    a,

    // chroma_u
   -(0.148f * r)
   -(0.291f * g)
   +(0.439f * b),

    // chroma_v
    (0.439f * r)
   -(0.368f * g)
   -(0.071f * b)

  };

// ---   *   ---   *   ---
// overwrite

  p[0]=std::clamp(yauv[0],0.0f,1.0f);
  p[1]=std::clamp(yauv[1],0.0f,1.0f);

  p[2]=std::clamp(yauv[2],-0.5f,0.5f);
  p[3]=std::clamp(yauv[3],-0.5f,0.5f);

};

// ---   *   ---   *   ---
// ^inverse

void yauv2rgba(float* p) {

  // extract for clarity
  float luma     = p[0]*1.164000f;
  float alpha    = p[1];
  float chroma_u = p[2];
  float chroma_v = p[3];

// ---   *   ---   *   ---
// convert

  float r=
    (luma)
  + (1.596f * chroma_v)
  ;

  float g=
    (luma)
  - (0.392f * chroma_u)
  - (0.813f * chroma_v)
  ;

  float b=
    (luma)
  + (2.017f * chroma_u)
  ;

  float a=alpha;

// ---   *   ---   *   ---
// clamp and overwrite

  p[0]=std::clamp(r,0.0f,1.0f);
  p[1]=std::clamp(g,0.0f,1.0f);
  p[2]=std::clamp(b,0.0f,1.0f);
  p[3]=std::clamp(a,0.0f,1.0f);

};

// ---   *   ---   *   ---
// checks that pixel has no data
// for non-color images

void nc_discard_chk(float* p) {

// TODO
//
// this would avoid storing redundancies
//
// however, we need to overwrite {0.5,0.5,1,0}
// into all and every such pixel at read time
// for it to work correctly
//
// should check m_c_enc eq m_enc.normal
// when rebuilding images at JOJ::read_pixels
// then call a filter if so; fairly trivial

//  if(
//
//     p[0] < 0.510f && p[0] > 0.490
//  && p[1] < 0.510f && p[1] > 0.490
//
//  && p[3] < 0.08f
//
//  ) {
//
//    p[0]=Frac::BLANK;
//    p[1]=Frac::BLANK;
//    p[2]=Frac::BLANK;
//    p[3]=Frac::BLANK;
//
//  };

};

void orme_discard_chk(float* p) {

  if(

     p[0] < 0.08f
  && p[1] < 0.08f
  && p[2] < 0.08f
  && p[3] < 0.08f

  ) {

    p[0]=Frac::BLANK;
    p[1]=Frac::BLANK;
    p[2]=Frac::BLANK;
    p[3]=Frac::BLANK;

    return;

  };

};

// ---   *   ---   *   ---
// normalizes normal map pixel

void fnorm(float* p) {

  float mag=sqrt(
    pow(p[0],2)
  + pow(p[1],2)
  + pow(p[2],2)

  );

  p[0]/=mag;
  p[1]/=mag;
  p[2]/=mag;

};

// ---   *   ---   *   ---
// ice

template struct Frac::Bat<char>;

// ---   *   ---   *   ---
