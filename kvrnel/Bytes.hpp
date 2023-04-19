#ifndef __24_BYTES_H__
#define __24_BYTES_H__

// ---   *   ---   *   ---
// deps

  #include <cmath>
  #include <cstdio>
  #include <algorithm>

  #include "Style.hpp"
  #include "Evil.hpp"

// ---   *   ---   *   ---

char* stirr_p(int* src,int cnt=4);

// ---   *   ---   *   ---

// get first non-zero bit in mask
inline uint64_t bsf(uint64_t x) {
  return __builtin_ctzll(x);

};

// ^first zero ;>
inline uint64_t nbsf(uint64_t x) {
  return __builtin_ctzll(~x);

};

// ---   *   ---   *   ---
// finding/enforcing nearest power of 2

inline uint64_t fast_log2(uint64_t x) {
  return 63-__builtin_clzll(x);

};

inline uint64_t fast_sqrt2(uint64_t x) {
  return 1<<((63-__builtin_clzll(x))>>1);

};

inline uint64_t near_pow2(uint64_t x) {
  uint64_t y=63-__builtin_clzll(x);
  y+=(x & (x-1))!=0;

  return 1<<y;

};

inline uint64_t popcount(uint64_t x) {
  return __builtin_popcountll(x);

};

// ---   *   ---   *   ---
// integer exp by sq

uint64_t ipow(
  uint64_t x,
  uint64_t n

);

// ---   *   ---   *   ---
// float encoding helper ;>

namespace Frac {

  template <typename T>
  struct Bat {

  public:

    T*      m_bytes;
    float*  m_floats;

    uint64_t  m_sz;

    T*      m_enc;
    int*    m_cnt;

    bool    m_mode;

    void encoder(void);

// ---   *   ---   *   ---
// internals

  private:

    // m_bytes to m_floats
    void encode(

      float   step,
      int     nbits,

      bool    unsig=false

    );

    // ^inverse
    void decode(

      float   step,
      int     nbits,

      bool    unsig=false

    );

  };

// ---   *   ---   *   ---

  // unit vectors do better just
  // over-stretching to higher values
  cxr32 NVEC=1.25f;

  // regular coords are somewhat the inverse
  // it seems staying below the mid-point
  // works a lot better
  cxr32 CORD=0.64f;

  // eq to no rounding
  cxr32 LINE=1.00f;

  // always default to normals
  static float Rounding_Mode=NVEC;

// ---   *   ---   *   ---

  // common fractions
  cxr32 STEP[]={
    1.0f/0x00002,1.0f/0x00004,
    1.0f/0x00008,1.0f/0x00010,
    1.0f/0x00020,1.0f/0x00040,
    1.0f/0x00080,1.0f/0x00100,

    1.0f/0x00200,1.0f/0x00400,
    1.0f/0x00800,1.0f/0x01000,
    1.0f/0x02000,1.0f/0x04000,
    1.0f/0x08000,1.0f/0x10000,

  };

  // bit_count-1
  cx32 BITS[]={
    0x0,0x1,0x2,0x3,
    0x4,0x5,0x6,0x7,
    0x8,0x9,0xA,0xB,
    0xC,0xD,0xE,0xF

  };

  // ^max value for a bitcount
  cx16 MAXV[]={
    0x0001,0x0003,0x0007,
    0x000F,0x001F,0x003F,
    0x007F,0x00FF,0x01FF,
    0x03FF,0x07FF,0x0FFF,
    0x1FFF,0x3FFF,0x7FFF

  };

// ---   *   ---   *   ---
// syntax dummies ;>
// these are just so we don't have
// any "magic number" looking calls

  enum {

    SIZE_1BIT ,SIZE_2BIT,
    SIZE_3BIT ,SIZE_4BIT,

    SIZE_5BIT ,SIZE_6BIT,
    SIZE_7BIT ,SIZE_8BIT,

    SIZE_9BIT ,SIZE_10BIT,
    SIZE_11BIT,SIZE_12BIT,

    SIZE_13BIT,SIZE_14BIT,
    SIZE_15BIT,SIZE_16BIT

  };

  enum {

    STEP_1BIT ,STEP_2BIT,
    STEP_3BIT ,STEP_4BIT,

    STEP_5BIT ,STEP_6BIT,
    STEP_7BIT ,STEP_8BIT,

    STEP_9BIT ,STEP_10BIT,
    STEP_11BIT,STEP_12BIT,

    STEP_13BIT,STEP_14BIT,
    STEP_15BIT,STEP_16BIT

  };

  enum {SIGNED,UNSIGNED};
  enum {ENCODE,DECODE};

};

// ---   *   ---   *   ---
// get nth bit is set

template <typename T>
inline bool nthbit(T b,T n) {
  return (b&(1<<n))!=0;

};

// ---   *   ---   *   ---
// count N bits needed for value

template <typename T>
inline T bitsize(T x) {
  return fast_log2(near_pow2(x));

};

// ---   *   ---   *   ---

// give portion of a value's bits
template <typename T>
T bitslice(T b,T iStart,T iEnd) {

  int x=0b0;
  int bit;

  for(T i=iStart;i<iEnd+1;i++) {

    bit=nthbit<T>(b,i);

    if(bit) {
      x|=bit<<(i-iStart);

    };

  };

  return x;

};

// ---   *   ---   *   ---
// serialization helper

template <typename T>
uint64_t bitpack(T* b,T* enc,int* cnt) {

  uint64_t out   = 0;
  int      total = 0;

  while(*cnt) {

    // elem size && count
    int bits   = Frac::BITS[enc[1]]+1;
    int limit  = *cnt;

    int mask   = (1<<bits)-1;

    // append masked bits to output
    for(int x=0;x<limit;x++) {
      out|=(*b&mask)<<total;
      total+=bits;

      b++;

    };

    // go next
    enc+=3;
    cnt++;

  };

  return out;

};

// ---   *   ---   *   ---
// ^recover

template <typename T>
void bitunpack(

  uint64_t  src,

  T*      b,

  T*      enc,
  int*    cnt

) {

  while(*cnt) {

    // elem size && count
    int bits   = Frac::BITS[enc[1]]+1;
    int limit  = *cnt;

    int mask   = (1<<bits)-1;

    // append masked bits to dst
    for(int x=0;x<limit;x++) {

      *b++=src&mask;
      src>>=bits;

    };

    // go next
    enc+=3;
    cnt++;

  };

};

// ---   *   ---   *   ---

template <typename T>
int enc_bitsize(T* enc,int* cnt) {

  int out=0;

  while(*cnt) {

    int bits   = Frac::BITS[enc[1]]+1;
    int limit  = *cnt;

    out+=bits*limit;

    enc+=3;
    cnt++;

  };

  return out;

};

// ---   *   ---   *   ---
// copy to same-sized buffer of smaller type

template <typename T>
void xfer(

  uint8_t*  dst,
  T         src,

  uint64_t  size,
  uint64_t  step

) {

  for(

    uint64_t i=0,h=0;

    i<size;
    i+=step,h++

  ) {

    for(int j=0;j<step;j++) {
      dst[i+j]=src[h]&0xFF;
      src[h]>>=8;

    };

  };

};

// ---   *   ---   *   ---

// quantize and pack float
template <typename T>
T frac(

  float x,

  float step,
  int   nbits,

  bool  unsig=false

) {

  uint16_t mid  = 1<<(nbits);
  uint16_t max  = Frac::MAXV[nbits];

  uint16_t midp = (! unsig)
    ? mid
    : 1
    ;

  long     b   = roundf(x/step);
  float    top = step*(max-mid);

  top -= step * Frac::Rounding_Mode;

  b   += mid * (! unsig);
  b   -= 1   * (b==max && x<top);

  bool over=b>max;

  b    = (max*over) + (b * (! over));

  return (T) b;

};

// ---   *   ---   *   ---
// ^unpack

template <typename T>
float unfrac(

  T     b,

  float step,
  int   nbits,

  bool  unsig=false

) {

  uint16_t max  = Frac::MAXV[nbits];
  uint16_t mid  = 1<<nbits;

  b += 1   * (b==max);
  b -= mid * (! unsig);

  return (b*step);

};

// ---   *   ---   *   ---
// color conversions

void rgba2yauv(float* p);
void yauv2rgba(float* p);

void fnorm(float* p);

// ---   *   ---   *   ---

//// *unpacked* geometry types
//// ie, how we read it from file on unzip
//// we'd pack them for actual in-memory usage
//
//typedef struct CRK_VERT_COMPRESSED {
//
//  char co_x;      // coordinates
//  char co_y;
//  char co_z;
//
//  char nr_x;      // normals
//  char nr_y;
//  char nr_z;
//
//  char tn_x;      // tangent
//  char tn_y;
//  char tn_z;
//
//  char bhand;     // bitangent handeness
//
//  char u;         // tex proj
//  char v;
//
//} CRKVRT;

//// encodes packed curv/orm into joj-like
//void ENCGREY(float* p,JOJPIX*j);
//
//// decodes joj-like into packed curv/orm
//void DECGREY(float* p,JOJPIX*j);
//
//// encode rgba into joj
//void ENCRGBA(float* p,JOJPIX*j);
//
//// decode joj into rgba
//void DECRGBA(float* p,JOJPIX*j);
//
////   ---     ---     ---     ---     ---
//
//// enconde verts into crk format
//void ENCVRT(float* v,CRKVRT*c);
//
////   ---     ---     ---     ---     ---
//
//typedef struct VertexPacked3D {
//
//  int frac1;                 // vpx, vpy, vpz, nnx
//  int frac2;                 // xxn, ttt, uvx, uvy
//
//} VP3D;
//
//void ZJC_pack_rawvert(VP3D*vert,CRKVRT*data);
//
////   ---     ---     ---     ---     ---

#endif //__24_BYTES_H__
