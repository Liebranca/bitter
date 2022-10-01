#ifndef __24_BYTES_H__
#define __24_BYTES_H__

// ---   *   ---   *   ---
// deps

  #include "Style.hpp"
  #include "Evil.hpp"

// ---   *   ---   *   ---
// float encoding helper ;>

namespace Frac {

  template <typename T>
  struct Bat {

  public:
    T**     m_dst;
    T*      m_enc;

    float** m_src;
    size_t  m_sz;

    void encode(void);
    void decode(void);

  };

  // unit vectors do better just
  // over-stretching to higher values
  cxr32 NVEC=1.25f;

  // regular coords are somewhat the inverse
  // it seems staying below the mid-point
  // works a lot better
  cxr32 CORD=0.64f;

  // always default to normals
  static float Rounding_Mode=NVEC;

};

// ---   *   ---   *   ---

// get nth bit is set
template <typename T>
inline bool nthbit(T b,T n);

// count N bits needed for value
template <typename T>
T bitsize(T x);

// give portion of a value's bits
template <typename T>
T bitslice(T b,T iStart,T iEnd);

// quantize and pack float
template <typename T>
T frac(float x,float step,int nbits);

// ^unpack
template <typename T>
float unfrac(
  T b,

  float step,
  int nbits,

  int usig=0

);

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
