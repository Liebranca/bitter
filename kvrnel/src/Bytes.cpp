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

  #include <cmath>
  #include <cstdio>

  #include <algorithm>

// ---   *   ---   *   ---
// get nth bit is set

template <typename T>
inline bool nthbit(T b,T n) {
  return (b&(1<<n))!=0;

};

// ---   *   ---   *   ---
// count N bits needed for value

template <typename T>
T bitsize(T x) {

  T bsize=0;
  T trail=0;

  for(T b=0;b<8;b++) {

    T bitset=nthbit<T>(x,b);

    if(bitset) {

      bsize+=trail+1;
      trail=0;

    } else {trail++;};

  };

  return bsize;

};

// ---   *   ---   *   ---
// give portion of a value's bits

template <typename T>
T bitslice(

  T b,

  T iStart,
  T iEnd

) {

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
// clamps floats along a linear range
// encodes them as integers

template <typename T>
T frac(

  float x,

  float step,
  int   nbits,

  bool  unsig

) {

  uint16_t mid  = 1<<(nbits);
  uint16_t max  = Frac::MAXV[nbits];

  uint16_t midp = (!unsig)
    ? mid
    : 1
    ;

  long     b   = roundf(x/step);
  float    top = step*(max-mid);

  top-=step*Frac::Rounding_Mode;

  b+=mid*!unsig;
  b-=1*(b==max && x<top);

  return (T) b;

};

// ---   *   ---   *   ---
// ^decodes frac'd floats

template <typename T>
float unfrac(

  T     b,

  float step,
  int   nbits,

  bool  unsig

) {

  uint16_t max=Frac::MAXV[nbits];
  uint16_t mid=1<<(nbits);

  uint16_t midp = (!unsig)
    ? mid
    : 0
    ;

  b+=1*(b==max);
  b-=mid*!unsig;

  return (b*step);

};

// ---   *   ---   *   ---
// m_bytes to m_floats

template <typename T>
inline void Frac::Bat<T>::encode(

  float   step,
  int     bits,

  bool    unsig

) {

  **m_bytes=frac<T>(
    **m_floats,step,bits,unsig

  );

};

// ---   *   ---   *   ---
// ^inverse

template <typename T>
inline void Frac::Bat<T>::decode(

  float   step,
  int     bits,

  bool    unsig

) {

  **m_floats=unfrac<T>(
    **m_bytes,step,bits,unsig

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

while(m_sz) {

  // switch precision level
  float step  = STEP[*m_enc++];
  int   bits  = BITS[*m_enc++];

  // true if field is unsigned
  bool  unsig = *m_enc++;

  // run through elements
  while(*m_bytes!=NULL) {

    (this->*branch_to)(step,bits,unsig);

    // go next
    m_bytes++;
    m_floats++;

  };

  // skip null
  m_bytes++;
  m_sz--;

}};

// ---   *   ---   *   ---
// color transform

void rgba2yauv(float* p) {

  // extract for clarity
  float r=p[0];
  float g=p[1];
  float b=p[2];
  float a=p[3];

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

  // overwrite
  p[0]=std::clamp(yauv[0],0.0,1.0);
  p[1]=std::clamp(yauv[1],0.0,1.0);

  p[2]=std::clamp(yauv[2],-0.5,0.5);
  p[3]=std::clamp(yauv[3],-0.5,0.5);

};

// ---   *   ---   *   ---
// ^inverse

void yauv2rgba(float* p) {

  float luma     = p[0]*1.164000f;

  float alpha    = p[1];
  float chroma_u = p[2];
  float chroma_v = p[3];

  p[0]=
    (luma)
  + (1.596f * chroma_v)
  ;

  p[1]=
    (luma)
  - (0.392f * chroma_u)
  - (0.813f * chroma_v)
  ;

  p[2]=
    (luma)
  + (2.017f * chroma_u)
  ;

  p[3]=alpha;

  p[0]=std::clamp(p[0],0.0,1.0);
  p[1]=std::clamp(p[1],0.0,1.0);
  p[2]=std::clamp(p[2],0.0,1.0);
  p[3]=std::clamp(p[3],0.0,1.0);

};

//// ---   *   ---   *   ---
//
//void ENCVRT(float* v,CRKVRT* c) {
//
//
//  float ff=FRACL_F[CFRAC_L];
//  uint fi=FRACL_I[CFRAC_L];
//
//                            // i dont care what
//                            // anybody says, Y is
//                            // UP
//  c->co_x=FLTOFRAC(
//    v[0],4,ff*CFRAC_D1,
//    fi/CFRAC_D1,fi
//
//  );c->co_y=FLTOFRAC(
//    v[2],4,ff*CFRAC_D1,
//    fi/CFRAC_D1,fi
//
//  );c->co_z=FLTOFRAC(
//    -v[1],4,ff*CFRAC_D1,
//    fi/CFRAC_D1,fi
//
//  );
//
//                            // same for normals
//  c->nr_x=FLTOFRAC(
//    v[3],1,ff*CFRAC_D0,
//    fi/CFRAC_D0,fi/CFRAC_D0
//
//  );c->nr_y=FLTOFRAC(
//    v[5],1,ff*CFRAC_D0,
//    fi/CFRAC_D0,fi/CFRAC_D0
//
//  );c->nr_z=-v[4]<0;
//
//                            // ... and tangents
//  c->tn_x=FLTOFRAC(
//    v[6],1,ff*CFRAC_D0,
//    fi/CFRAC_D0,fi/CFRAC_D0
//
//  );c->tn_y=FLTOFRAC(
//    v[8],1,ff*CFRAC_D0,
//    fi/CFRAC_D0,fi/CFRAC_D0
//
//  );c->tn_z=-v[7]<0;
//
//  c->bhand=v[9]<0;
//
//  c->u=FLTOFRAC(v[10],1,ff,fi,0);
//  c->v=FLTOFRAC(v[11],1,ff,fi,0);
//
//};
//
//// ---   *   ---   *   ---


//// ---   *   ---   *   ---
//
//uchar GREY4STP(float v) {
//  return((v>0.25&&v<=0.75)||v>=0.95)|((v>0.75)<<1);
//
//};
//
//void ENCGREY(float* p,JOJPIX* j) {
//
//
//  JOJGREY* o=(JOJGREY*) j;
//
//  float ff=FRACL_F[CFRAC_L];
//  uint fi=FRACL_I[CFRAC_L];
//
//  o->a=GREY4STP(p[0]);
//  o->b=GREY4STP(p[1]);
//  o->c=GREY4STP(p[2]);
//  o->d=GREY4STP(p[3]);
//
//};
//
//// ---   *   ---   *   ---
//
//void DECGREY(float* p,JOJPIX* j) {
//
//
//  JOJGREY* o=(JOJGREY*) j;
//
//  float ff=FRACL_F[CFRAC_L];
//  uint fi=FRACL_I[CFRAC_L];
//
//  p[0]=o->a*0.35f;
//  p[1]=o->b*0.35f;
//  p[2]=o->c*0.35f;
//  p[3]=o->d*0.35f;
//
//};
//
//// ---   *   ---   *   ---
//
//void ZJC_pack_rawvert(
//  VP3D* vert,
//  CRKVRT* data) {
//
//  ;
//
//  /* old fun. rewrite.
//  uint co=((FLTOFRAC(data->co [0],4,FRAC5,32,128))
//  |(FLTOFRAC(data->co [1],4,FRAC5,32,128)<<8)
//  |(FLTOFRAC(data->co [2],4,FRAC5,32,128)<<16));
//
//  uint nnn=((FLTOFRAC(data->normal
//    [0],1,FRAC5,32,32))
//  |(FLTOFRAC(data->normal [1],1,FRAC4,16,16)<<6)
//  |((data->normal [2]<0)<<11)
//  |(((int)data->bhand)<<12));
//
//  uint
//    ttt=((FLTOFRAC(data->tangent[0],1,FRAC5,32,32))
//  |(FLTOFRAC(data->tangent[1],1,FRAC5,32,32)<<6)
//  |((data->tangent[2]<0)<<12));
//
//  uint uvs=((FLTOFRAC(data->uv [0],1,FRAC7,64,64))
//  |(FLTOFRAC(data->uv [0],1,FRAC7,64,64)<<7));
//
//  vert->frac1=((co)|((nnn&0xFF)<<24));
//  vert->frac2=(((nnn&(31
//    <<8))>>8)|(ttt<<5)|(uvs<<18));*/
//}
////   ---     ---     ---     ---     ---
//// #:0xF;>

